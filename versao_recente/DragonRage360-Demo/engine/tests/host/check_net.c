#include <arpa/inet.h>
#include <stdarg.h>
#include <stdio.h>
#include "net_check.h"
void xsdk_log_write(const char *format,...) {va_list a;va_start(a,format);vfprintf(stderr,format,a);fputc('\n',stderr);va_end(a);}
int xsdk_net_address(struct in_addr *address) {return inet_pton(AF_INET,"127.0.0.1",address)==1?0:1;}
int main(void) {char message[700];demo_net_check(message,sizeof(message));puts(message);return 0;}
