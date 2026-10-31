/* SPDX-License-Identifier: GPL-3.0-or-later */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
extern float probe_add(float,float),probe_sub(float,float),probe_mul(float,float),probe_div(float,float),probe_mix(float,float,float);
extern int probe_lt(float,float),probe_eq(float,float),probe_le_branch(float,float),probe_fix(float);
extern unsigned probe_fixu(float);
extern float probe_float(int),probe_floatu(unsigned),probe_neg(float),probe_abs(float),probe_single(double);
extern double probe_double(float),probe_dadd(double,double);
extern int probe_dlt(double,double),probe_deq(double,double);
extern uint32_t dr_ps2_addsf_bits(uint32_t,uint32_t),dr_ps2_subsf_bits(uint32_t,uint32_t),dr_ps2_mulsf_bits(uint32_t,uint32_t),dr_ps2_divsf_bits(uint32_t,uint32_t);
extern uint32_t dr_ps2_floatsf_bits(int32_t),dr_ps2_floatunssf_bits(uint32_t),dr_ps2_fixunssf_bits(uint32_t);
extern int dr_ps2_cmpsf_bits(uint32_t,uint32_t),dr_ps2_fixsf_bits(uint32_t);
static float f(uint32_t x){float a;memcpy(&a,&x,4);return a;}
static uint32_t u(float a){uint32_t x;memcpy(&x,&a,4);return x;}
static unsigned checks;
static void check(uint32_t got,uint32_t want,const char *op,uint32_t a,uint32_t b){
    checks++;if(got!=want){fprintf(stderr,"%s(%08x,%08x): %08x != %08x\n",op,a,b,got,want);exit(1);}
}
#define CHECK(op,expected) check(u(probe_##op(f(a),f(b))),expected,#op,a,b)
int main(void){
    static const uint32_t edge[]={0,0x80000000,1,0x007fffff,0x00800000,0x3dcccccc,0x3dcccccd,0x3f800000,0xbf800000,0x7f7fffff,0xff7fffff,0x7fffffff,0x7f800000,0x7fc12345};
    uint32_t seed=0x31415926;
    for(unsigned i=0;i<100000+sizeof(edge)/sizeof(edge[0]);i++){
        seed=seed*1664525u+1013904223u;uint32_t a=i<sizeof(edge)/sizeof(edge[0])?edge[i]:seed;
        seed=seed*1664525u+1013904223u;uint32_t b=seed,c=edge[i%(sizeof(edge)/sizeof(edge[0]))];
        CHECK(add,dr_ps2_addsf_bits(a,b));CHECK(sub,dr_ps2_subsf_bits(a,b));
        CHECK(mul,dr_ps2_mulsf_bits(a,b));CHECK(div,dr_ps2_divsf_bits(a,b));
        check(u(probe_mix(f(a),f(b),f(c))),dr_ps2_addsf_bits(a,dr_ps2_mulsf_bits(b,c)),"mix",a,b);
        int cmp=dr_ps2_cmpsf_bits(a,b);
        check(probe_lt(f(a),f(b)),cmp<0,"lt",a,b);check(probe_eq(f(a),f(b)),cmp==0,"eq",a,b);
        check(probe_le_branch(f(a),f(b)),cmp<=0?7:3,"branch",a,b);
        check((uint32_t)probe_fix(f(a)),(uint32_t)dr_ps2_fixsf_bits(a),"fix",a,b);
        check(probe_fixu(f(a)),dr_ps2_fixunssf_bits(a),"fixu",a,b);
        check(u(probe_float((int32_t)a)),dr_ps2_floatsf_bits((int32_t)a),"float",a,b);
        check(u(probe_floatu(a)),dr_ps2_floatunssf_bits(a),"floatu",a,b);
        check(u(probe_neg(f(a))),a^0x80000000u,"neg",a,b);
    }
    /* Double stays IEEE: NaN must not compare equal or less. */
    check(probe_deq(NAN,1.0),0,"nan eq",0,0);check(probe_dlt(NAN,1.0),0,"nan lt",0,0);
    check(probe_deq(1.0,1.0),1,"double eq",0,0);
    check(probe_dadd(1.25,2.5)==3.75,1,"double add",0,0);
    check(u(probe_single(probe_double(1.5f))),u(1.5f),"roundtrip",0,0);
    printf("PASS: %u comparisons via hard-float calls against original integer-bit helpers (host only)\n",checks);return 0;
}
