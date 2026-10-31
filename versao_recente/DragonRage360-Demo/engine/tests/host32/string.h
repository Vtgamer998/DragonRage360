#ifndef DR_TEST_STRING_H
#define DR_TEST_STRING_H
#include <stddef.h>
void *memcpy(void *,const void *,size_t);
void *memset(void *,int,size_t);
int memcmp(const void *,const void *,size_t);
size_t strlen(const char *);
int strcmp(const char *,const char *);
int strncmp(const char *,const char *,size_t);
#endif
