/*------------------------------------------------------------------
 * sprintf_s.h -- Safe Sprintf Interfaces
 *
 * August 2014, D Wheeler
 *
 * Copyright (c) 2014 by Intel Corp
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *------------------------------------------------------------------
 */
#ifndef SPRINTF_S_H_
#define SPRINTF_S_H_

#include "safe_lib_errno.h"


#define SNPRFNEGATE(x) (-1*(x))

#if 0
int snprintf_s_null(char *dest, rsize_t dmax, const char *format);
int snprintf_s_i(char *dest, rsize_t dmax, const char *format, int a);
int snprintf_s_c(char *dest, rsize_t dmax, const char *format, char c);
int snprintf_s_s(char *dest, rsize_t dmax, const char *format, char * s);
int snprintf_s_g(char *dest, rsize_t dmax, const char *format, double g);
int snprintf_s_p(char *dest, rsize_t dmax, const char *format, void *p);
int snprintf_s_ss(char *dest, rsize_t dmax, const char *format, char * s, char * s1);
int snprintf_s_sss(char *dest, rsize_t dmax, const char *format, char * s, char * s1, char * s2);
int snprintf_s_ssss(char *dest, rsize_t dmax, const char *format, char * s, char * s1, char * s2, char * s3);
int snprintf_s_ssssi(char *dest, rsize_t dmax, const char *format, char *s, char *s2, char *s3, char *s4, int a);
int snprintf_s_isis(char *dest, rsize_t dmax, const char *format, int a, char *s, int b, char *s1);
int snprintf_s_sis(char *dest, rsize_t dmax, const char *format, char *s, int a, char *s1);
int snprintf_s_space_s(char *dest, rsize_t dmax, const char *format, int num_of_space, char * s);

int snprintf_s_si(char *dest, rsize_t dmax, const char *format, char *s, int a);
int snprintf_s_sii(char *dest, rsize_t dmax, const char *format, char *s, int a, int b);
int snprintf_s_siii(char *dest, rsize_t dmax, const char *format, char *s, int a, int b, int );
int snprintf_s_l(char *dest, rsize_t dmax, const char *format, long a);
int snprintf_s_ll(char *dest, rsize_t dmax, const char *format, long long a);
int snprintf_s_sl(char *dest, rsize_t dmax, const char *format, char *s, long a);
int snprintf_s_sll(char *dest, rsize_t dmax, const char *format, char *s, long long a);
int snprintf_s_ii(char *dest, rsize_t dmax, const char *format, int a, int b);
int snprintf_s_iiii(char *dest, rsize_t dmax, const char *format, int a, int b, int c, int d);
#endif

int snprintf_debug_s_null(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format);
int snprintf_debug_s_i(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, int a);
int snprintf_debug_s_c(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, char c);
int snprintf_debug_s_s(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, char * s);
int snprintf_debug_s_g(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, double g);
int snprintf_debug_s_p(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, void *p);
int snprintf_debug_s_ss(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, char * s, char * s1);
int snprintf_debug_s_sss(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, char * s, char * s1, char * s2);
int snprintf_debug_s_ssss(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, char * s, char * s1, char * s2, char * s3);
int snprintf_debug_s_ssssi(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, char *s, char *s2, char *s3, char *s4, int a);
int snprintf_debug_s_isis(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, int a, char *s, int b, char *s1);
int snprintf_debug_s_sis(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, char *s, int a, char *s1);
int snprintf_debug_s_space_s(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, int num_of_space, char * s);
int snprintf_debug_s_si(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, char *s, int a);
int snprintf_debug_s_is(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, int a, char *s);
int snprintf_debug_s_sii(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, char *s, int a, int b);
int snprintf_debug_s_siii(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, char *s, int a, int b, int );
int snprintf_debug_s_l(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, long a);
int snprintf_debug_s_ll(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, long long a);
int snprintf_debug_s_sl(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, char *s, long a);
int snprintf_debug_s_sll(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, char *s, long long a);
int snprintf_debug_s_ii(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, int a, int b);
int snprintf_debug_s_iis(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, int a, int b, char *s);
int snprintf_debug_s_iiii(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, int a, int b, int c, int d);



#define snprintf_s_null(...)       snprintf_debug_s_null(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)                             
#define snprintf_s_i(...)          snprintf_debug_s_i(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)                             
#define snprintf_s_c(...)          snprintf_debug_s_c(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)                             
#define snprintf_s_s(...)          snprintf_debug_s_s(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)                             
#define snprintf_s_g(...)          snprintf_debug_s_g(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)                             
#define snprintf_s_p(...)          snprintf_debug_s_p(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)                             
#define snprintf_s_ss(...)         snprintf_debug_s_ss(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)                             
#define snprintf_s_sss(...)        snprintf_debug_s_sss(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)                             
#define snprintf_s_ssss(...)       snprintf_debug_s_ssss(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)                             
#define snprintf_s_ssssi(...)      snprintf_debug_s_ssssi(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)                               
#define snprintf_s_isis(...)       snprintf_debug_s_isis(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)                             
#define snprintf_s_sis(...)        snprintf_debug_s_sis(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)                             
#define snprintf_s_space_s(...)    snprintf_debug_s_space_s(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)                                   
                                                         
#define snprintf_s_si(...)         snprintf_debug_s_si(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)
#define snprintf_s_is(...)         snprintf_debug_s_is(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)  
#define snprintf_s_sii(...)        snprintf_debug_s_sii(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)                             
#define snprintf_s_siii(...)       snprintf_debug_s_siii(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)                             
#define snprintf_s_l(...)          snprintf_debug_s_l(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)                             
#define snprintf_s_ll(...)         snprintf_debug_s_ll(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)                             
#define snprintf_s_sl(...)         snprintf_debug_s_sl(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)                             
#define snprintf_s_sll(...)        snprintf_debug_s_sll(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)                             
#define snprintf_s_ii(...)         snprintf_debug_s_ii(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)                             
#define snprintf_s_iis(...)         snprintf_debug_s_iis(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)                             
#define snprintf_s_iiii(...)       snprintf_debug_s_iiii(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)  


#endif /* SPRINTF_S_H_ */
