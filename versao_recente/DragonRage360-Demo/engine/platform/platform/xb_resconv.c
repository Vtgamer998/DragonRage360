/* SPDX-License-Identifier: MIT
 * See xb_resconv.h. */
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "xb_resconv.h"

#ifdef XB_HOST_TEST
#define XB_LOG(...) (fprintf(stderr, __VA_ARGS__), fputc('\n', stderr))
#else
#include "xsdk/log.h"
#define XB_LOG(...) xsdk_log_write(__VA_ARGS__)
extern void (*gXbFilePostRead)(int ptid, int flid, uint64_t off, uint64_t fileSize, void *buf, size_t n);
#endif

#define MAX_RANGES 64
#define MAX_PENDING 32
#define MAX_SEEN 512

static struct { int first, last, packed; XbResFn fn; XbResCheckedFn checked; } sRanges[MAX_RANGES];
static int sRangeCount;
static struct { const uint8_t *buf; size_t n; int id; XbResFn fn; XbResCheckedFn checked; } sPending[MAX_PENDING];
static int sPendingNext;
static int *sSeen;
static size_t sSeenCount, sSeenCapacity;
static unsigned sConverted, sUnconverted, sPartial, sFailedConversions;
static int sLastError;
static struct { uintptr_t buf; size_t n; } sReads[64];
static unsigned sReadNext;
size_t XbRes_SourceSize(const void *src) {
    uintptr_t p=(uintptr_t)src;
    unsigned i;
    for(i=0;i<64;i++)if(sReads[i].n && p>=sReads[i].buf && p-sReads[i].buf<sReads[i].n)
        return sReads[i].n-(p-sReads[i].buf);
    return 0;
}
int XbRes_LastError(void) { return sLastError; }

int XbRes_Id(int ptid, int flid) {
    if (flid < 0 || flid > INT_MAX - 0xD48) { return INT_MIN; }
    if (ptid == 0) { return -flid; }
    if (ptid == 1) { return flid + 1; }
    if (ptid == 2) { return flid + 0xD48; }
    return INT_MIN;
}

int XbRes_Register(int firstId, int lastId, XbResFn fn, int packed) {
    int i;

    if (fn == NULL || firstId > lastId || sRangeCount >= MAX_RANGES) {
        return -1;
    }
    for (i = 0; i < sRangeCount; i++) {
        if (firstId <= sRanges[i].last && lastId >= sRanges[i].first) {
            return -1;
        }
    }
    sRanges[sRangeCount].first = firstId;
    sRanges[sRangeCount].last = lastId;
    sRanges[sRangeCount].packed = packed != 0;
    sRanges[sRangeCount].fn = fn;
    sRangeCount++;
    return 0;
}

int XbRes_RegisterChecked(int firstId, int lastId, XbResCheckedFn fn, int packed) {
    int i;
    if (!fn || firstId > lastId || sRangeCount >= MAX_RANGES) return -1;
    for (i=0;i<sRangeCount;i++)
        if (firstId<=sRanges[i].last && lastId>=sRanges[i].first) return -1;
    sRanges[sRangeCount].first=firstId; sRanges[sRangeCount].last=lastId;
    sRanges[sRangeCount].packed=packed!=0; sRanges[sRangeCount].checked=fn;
    sRangeCount++; return 0;
}

static void convert(XbResFn fn, XbResCheckedFn checked, void *buf, size_t n, int id) {
    int rc=0;
    if(checked) rc=checked(buf,n,id); else if(fn) fn(buf,n,id); else rc=-1;
    if(rc) {
        sLastError=rc; sFailedConversions++;
        XB_LOG("res: file id %d conversion failed (%d): resource rejected",id,rc);
    } else sConverted++;
}

static int seen_before(int id) {
    size_t i;

    for (i = 0; i < sSeenCount; i++) {
        if (sSeen[i] == id) {
            return 1;
        }
    }
    if (sSeenCount == sSeenCapacity) {
        size_t cap=sSeenCapacity ? sSeenCapacity*2 : MAX_SEEN;
        int *next=realloc(sSeen,cap*sizeof(*next));
        if(!next) { sLastError=-1; return 1; }
        sSeen=next; sSeenCapacity=cap;
    }
    sSeen[sSeenCount++] = id;
    return 0;
}

static void forget_overlapping(const uint8_t *buf, size_t n) {
    int i;

    /* a buffer that is read into again is no longer the file it was: drop what it overlaps */
    for (i = 0; i < MAX_PENDING; i++) {
        uintptr_t a=(uintptr_t)buf, b=(uintptr_t)sPending[i].buf;
        if (sPending[i].buf != NULL && (a<=b ? b-a<n : a-b<sPending[i].n)) {
            sPending[i].buf = NULL;
        }
    }
}

void XbRes_Forget(const void *buf,size_t n) {
    int i;
    forget_overlapping((const uint8_t *)buf, n);
    for(i=0;i<64;i++) {
        uintptr_t a=(uintptr_t)buf,b=sReads[i].buf;
        if(sReads[i].n && (a<=b ? b-a<n : a-b<sReads[i].n))sReads[i].n=0;
    }
}

static void post_read(int ptid, int flid, uint64_t off, uint64_t fileSize, void *buf, size_t n) {
    int id = XbRes_Id(ptid, flid), i;

    sLastError=0;
    if(!buf && n) { sLastError=-1; return; }
    XbRes_Forget(buf,n);
    sReads[sReadNext].buf=(uintptr_t)buf;sReads[sReadNext].n=n;
    sReadNext=(sReadNext+1)%64;
    if (id == INT_MIN) {
        return;
    }
    if ((off != 0 || (uint64_t)n < fileSize) && sPartial++ < 16) {
        XB_LOG("res: file id %d partial read: offset %llu, %u of %llu bytes; not converted",id,
               (unsigned long long)off,(unsigned)n,(unsigned long long)fileSize);
    }
    for (i = 0; i < sRangeCount; i++) {
        if (id < sRanges[i].first || id > sRanges[i].last) {
            continue;
        }
        if (off != 0 || (uint64_t)n < fileSize) {
            return;
        }
        if (sRanges[i].packed) {
            sPending[sPendingNext].buf = (const uint8_t *)buf;
            sPending[sPendingNext].n = n;
            sPending[sPendingNext].id = id;
            sPending[sPendingNext].fn = sRanges[i].fn;
            sPending[sPendingNext].checked = sRanges[i].checked;
            sPendingNext = (sPendingNext + 1) % MAX_PENDING;
        } else {
            convert(sRanges[i].fn,sRanges[i].checked,buf,(size_t)fileSize,id);
        }
        return;
    }
    sUnconverted++;
    if (!seen_before(id)) {
        XB_LOG("res: file id %d (partition %d index %d, %llu bytes) read with no converter", id, ptid, flid, (unsigned long long)fileSize);
    }
}

void XbRes_Install(void) {
#ifndef XB_HOST_TEST
    gXbFilePostRead = post_read;
#endif
}
/* host tests drive the hook directly */
void XbRes_TestRead(int ptid, int flid, uint64_t off, uint64_t fileSize, void *buf, size_t n) { post_read(ptid, flid, off, fileSize, buf, n); }

void XbRes_AfterBpe(const void *src, void *out, int32_t size) {
    int i;

    sLastError=0;
    if(size<0 || (!out && size)) { sLastError=-1; return; }

    for (i = 0; i < MAX_PENDING; i++) {
        if (sPending[i].buf == (const uint8_t *)src) {
            if(sPending[i].n<8 || XbRes_Le32(src)!=(uint32_t)size) {
                sLastError=-1;return;
            }
            convert(sPending[i].fn,sPending[i].checked,out,(size_t)size,sPending[i].id);
            return;
        }
    }
    {
        static unsigned said;
        if (said++ < 16) {
            XB_LOG("res: Bpe_Decode of a buffer that is not a registered packed file (src %p, %d bytes): not converted", src, (int)size);
        }
    }
}

void XbRes_Report(void) {
    XB_LOG("res: %u files converted, %u reads with no converter (%u different ids), %u partial reads", sConverted, sUnconverted, (unsigned)sSeenCount,
           sPartial);
    XB_LOG("res: %u conversions rejected",sFailedConversions);
}

void XbRes_Swap16(void *p, size_t count) {
    uint8_t *b = p, t;
    size_t i;

    for (i = 0; i < count; i++, b += 2) {
        t = b[0]; b[0] = b[1]; b[1] = t;
    }
}
void XbRes_Swap32(void *p, size_t count) {
    uint8_t *b = p, t;
    size_t i;

    for (i = 0; i < count; i++, b += 4) {
        t = b[0]; b[0] = b[3]; b[3] = t;
        t = b[1]; b[1] = b[2]; b[2] = t;
    }
}
void XbRes_Swap64(void *p, size_t count) {
    uint8_t *b = p, t;
    size_t i;
    int k;

    for (i = 0; i < count; i++, b += 8) {
        for (k = 0; k < 4; k++) {
            t = b[k]; b[k] = b[7 - k]; b[7 - k] = t;
        }
    }
}

int XbRes_SwapPlan(void *buf, size_t n, const char *plan) {
    uint8_t *b = buf;
    size_t pos = 0;
    int repeat = 0;
    size_t planLen = strlen(plan), recLen = 0, i;

    if (planLen > 0 && plan[planLen - 1] == '*') {
        repeat = 1;
        planLen--;
    }
    /* length of one pass of the plan */
    for (i = 0; i < planLen;) {
        char c = plan[i++];
        size_t cnt = 0, unit = c == 'H' ? 2 : c == 'I' ? 4 : c == 'Q' ? 8 : 1;
        int digits = 0;

        if (c != 'B' && c != 'H' && c != 'I' && c != 'Q' && c != 'x') {
            return -1;
        }
        while (i < planLen && plan[i] >= '0' && plan[i] <= '9') {
            if (cnt > (SIZE_MAX-(size_t)(plan[i]-'0'))/10) return -1;
            cnt = cnt * 10 + (size_t)(plan[i++] - '0');
            digits = 1;
        }
        cnt=digits ? cnt : 1;
        if(cnt>(SIZE_MAX-recLen)/unit) return -1;
        recLen += unit * cnt;
    }
    if (recLen == 0) {
        return -1;
    }
    do {
        for (i = 0; i < planLen;) {
            char c = plan[i++];
            size_t cnt = 0, unit = c == 'H' ? 2 : c == 'I' ? 4 : c == 'Q' ? 8 : 1;
            int digits = 0;

            while (i < planLen && plan[i] >= '0' && plan[i] <= '9') {
                cnt = cnt * 10 + (size_t)(plan[i++] - '0');
                digits = 1;
            }
            if (!digits) {
                cnt = 1;
            }
            if (pos > n || cnt > (n-pos)/unit) {
                return -1;
            }
            if (c == 'H') { XbRes_Swap16(b + pos, cnt); }
            else if (c == 'I') { XbRes_Swap32(b + pos, cnt); }
            else if (c == 'Q') { XbRes_Swap64(b + pos, cnt); }
            pos += unit * cnt;
        }
    } while (repeat && recLen <= n-pos);
    return pos == n ? 0 : -1;
}
