/* SPDX-License-Identifier: GPL-3.0-or-later
 * Compile this translation unit WITH the pass; the checker uses the original
 * bit helpers directly. Parameters/return values remain ordinary C floats.
 */
float probe_add(float a,float b){return a+b;}
float probe_sub(float a,float b){return a-b;}
float probe_mul(float a,float b){return a*b;}
float probe_div(float a,float b){return a/b;}
float probe_mix(float a,float b,float c){return a+b*c;}
int probe_lt(float a,float b){return a<b;}
int probe_eq(float a,float b){return a==b;}
int probe_le_branch(float a,float b){if(a<=b)return 7;return 3;}
int probe_fix(float a){return (int)a;}
unsigned probe_fixu(float a){return (unsigned)a;}
float probe_float(int a){return (float)a;}
float probe_floatu(unsigned a){return (float)a;}
float probe_neg(float a){return -a;}
float probe_abs(float a){return a<0?-a:a;}
double probe_double(float a){return (double)a;}
float probe_single(double a){return (float)a;}
int probe_dlt(double a,double b){return a<b;}
int probe_deq(double a,double b){return a==b;}
double probe_dadd(double a,double b){return a+b;}
