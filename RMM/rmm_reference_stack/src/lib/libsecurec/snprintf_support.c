/*------------------------------------------------------------------
 * snprintf_support.c
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
#include "safeclib_private.h"
#include "safe_str_constraint.h"
#include "libsecurec/safe_str_lib.h"
#include "libsecurec/snprintf_s.h"

#include <stdlib.h>
#include <stdio.h>


#define FMT_CHAR    'c'
#define FMT_WCHAR   'C'
#define FMT_SHORT   'h'
#define FMT_INT		'd'
#define FMT_LONG	'l'
#define FMT_STRING	's'
#define FMT_WSTRING	'S'
#define FMT_DOUBLE	'g'
#define FMT_LDOUBLE	'G'
#define FMT_VOID    'p'
#define FMT_PCHAR	'1'
#define FMT_PSHORT	'2'
#define FMT_PINT	'3'
#define FMT_PLONG	'4'



#define MAX_FORMAT_ELEMENTS    16

#define CHK_FORMAT(X,Y)   (((X)==(Y))?1:0)


unsigned int
parse_format(const char *format, char pformatList[], unsigned int maxFormats)
{
	unsigned int  numFormats = 0;
	unsigned int  index = 0;
	unsigned int  start = 0;
	char		  lmod = 0;

	while (index < RSIZE_MAX_STR && format[index] != '\0' && numFormats < maxFormats)
	{
		if (format[index] == '%') {
			while(1) {
				if(format[index+1] != '.' && format[index+1] != '*')
			        break;
				index++;
			}
			start = index; // remember where the format string started
			// Check for flags
			switch( format[++index]) {
			case '\0': continue; // skip - end of format string
			case '%' : continue; // skip - actually a percent character
			case '#' : // convert to alternate form
			case '0' : // zero pad
			case '-' : // left adjust
			case ' ' : // pad with spaces
			case '+' : // force a sign be used
				index++; // skip the flag character
				break;
			}
			// check for and skip the optional field width
			while ( format[index] != '\0' && format[index] >= '0' && format[index] <= '9') {
				index++;
			}
			// Check for an skip the optional precision
			if ( format[index] != '\0' && format[index] == '.') {
				index++; // skip the period
				while ( format[index] != '\0' && format[index] >= '0' && format[index] <= '9') {
					index++;
				}
			}
			// Check for and skip the optional length modifiers
			lmod = ' ';
			switch( format[index]) {
			case 'h' : if ( format[++index] == 'h') {
				           ++index; //also recognize the 'hh' modifier
				           lmod = 'H'; // for char
			           } else {
			               lmod = 'h'; // for short
			           }
					   break;
			case 'l' : if ( format[++index] == 'l') {
				           ++index; //also recognize the 'll' modifier
				           lmod = 'd'; // for long long
			           } else {
			               lmod = 'l'; // for long
			           }
			           break;
			case 'L' : lmod = 'L'; break;
			case 'j' :
			case 'z' :
			case 't' : index++;
			           break;
			}

			// Recognize and record the actual modifier
			switch( format[index]) {
			case 'c' :
				 if ( lmod == 'l') {
					 pformatList[numFormats] = FMT_WCHAR; // store the format character
				 } else {
					 pformatList[numFormats] = FMT_CHAR;
				 }
				 numFormats++;
				 index++; // skip the format character
				 continue;

			case 'd' : case 'i' : // signed
			case 'o' : case 'u' : // unsigned
			case 'x' : case 'X' : // unsigned
				 if ( lmod == 'H') {
					 pformatList[numFormats] = FMT_CHAR; // store the format character
				 } else if ( lmod == 'l') {
				 	 pformatList[numFormats] = FMT_LONG; // store the format character
				 } else if ( lmod == 'h') {
					 pformatList[numFormats] = FMT_SHORT; // store the format character
				 } else{
					 pformatList[numFormats] = FMT_INT;
				 }
				 numFormats++;
				 index++; // skip the format character
				 continue;

			case 'e' : case 'E' :
			case 'f' : case 'F' :
			case 'g' : case 'G' :
			case 'a' : case 'A' :
				 if ( lmod == 'L') {
					 pformatList[numFormats] = FMT_LDOUBLE; // store the format character
				 } else{
					 pformatList[numFormats] = FMT_DOUBLE;
				 }
				 numFormats++;
				 index++; // skip the format character
				 continue;

			case 's' :
				 if ( lmod == 'l' || lmod == 'L') {
					 pformatList[numFormats] = FMT_WSTRING; // store the format character
				 } else {
					 pformatList[numFormats] = FMT_STRING;
				 }
				 numFormats++;
				 index++; // skip the format character
				 continue;

			case 'p' :
				 pformatList[numFormats] = FMT_VOID;
				 numFormats++;
				 index++; // skip the format character
				 continue;

			case 'n' :
				 if ( lmod == 'H') {
					 pformatList[numFormats] = FMT_PCHAR; // store the format character
				 } else if ( lmod == 'l') {
					 pformatList[numFormats] = FMT_PLONG; // store the format character
				 } else if ( lmod == 'h') {
					 pformatList[numFormats] = FMT_PSHORT; // store the format character
				 } else{
					 pformatList[numFormats] = FMT_PINT;
				 }
				 numFormats++;
				 index++; // skip the format character
				 continue;
			case 'm' :
			     // Does not represent an argument in the call stack
			     index++; // skip the format character
			     continue;
			default:
				printf("failed to recognize format string [");
				for (;start<index; start++) { printf("%c", format[start]); }
				puts("]");
				break;
			}
		}
		index++; // move past this character
	}

	return numFormats;
}

unsigned int
check_integer_format(const char format)
{
	unsigned int  retValue = 0; // default failure
	switch( format) {
		case FMT_CHAR  :
		case FMT_SHORT :
		case FMT_INT   :
			retValue = 1;
			break;
	}
	return retValue;
}

static int printerr(const char *file, int line, const char *function, errno_t err)
{

		char error_info[1024];

        sprintf(error_info, "echo \"%s:%d:%s---------error code:%d\" >> /tmp/snprintf", file, line, function, err);
        system(error_info);

		return 0;
}


#if 0
inline int snprintf_debug_s_null(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format)
{
	char pformatList[MAX_FORMAT_ELEMENTS];

	// Determine the number of format options in the format string
	unsigned int  nfo = parse_format(format, &pformatList[0], MAX_FORMAT_ELEMENTS);

	// Check that there are not too many format options
	if ( nfo != 0 ) {
		dest[0] = '\0';
		return SNPRFNEGATE(ESBADFMT);
	}

	//return snprintf(dest, dmax, format);
	return snprintf(dest, dmax, format);
}
#endif


inline int snprintf_debug_s_i(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, int a)
{
	char pformatList[MAX_FORMAT_ELEMENTS];
	unsigned int index = 0;

	// Determine the number of format options in the format string
	unsigned int  nfo = parse_format(format, &pformatList[0], MAX_FORMAT_ELEMENTS);

	// Check that there are not too many format options
	if ( nfo != 1 ) {
		dest[0] = '\0';
		printerr(file, line, function, ESBADFMT); return SNPRFNEGATE(ESBADFMT);
	}
	// Check that the format is for an integer type
	if ( check_integer_format(pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	return snprintf(dest, dmax, format, a);
}

inline int snprintf_debug_s_p(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, void *p)
{
	char pformatList[MAX_FORMAT_ELEMENTS];
	unsigned int index = 0;

	// Determine the number of format options in the format string
	unsigned int  nfo = parse_format(format, &pformatList[0], MAX_FORMAT_ELEMENTS);

	// Check that there are not too many format options
	if ( nfo != 1 ) {
		dest[0] = '\0';
		printerr(file, line, function, ESBADFMT); return SNPRFNEGATE(ESBADFMT);
	}
	// Check that the format is for an integer type
	if ( CHK_FORMAT(FMT_VOID, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	return snprintf(dest, dmax, format, p);
}


inline int snprintf_debug_s_s(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, char * s)
{
	char pformatList[MAX_FORMAT_ELEMENTS];
	unsigned int index = 0;

	// Determine the number of format options in the format string
	unsigned int  nfo = parse_format(format, &pformatList[0], MAX_FORMAT_ELEMENTS);

	// Check that there are not too many format options
	if ( nfo != 1 ) {
		dest[0] = '\0';
		printerr(file, line, function, ESBADFMT); return SNPRFNEGATE(ESBADFMT);
	}
	// Check that the format is for an integer type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	return snprintf(dest, dmax, format, s);
}

inline int snprintf_debug_s_c(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, char c)
{
	char pformatList[MAX_FORMAT_ELEMENTS];
	unsigned int index = 0;

	// Determine the number of format options in the format string
	unsigned int  nfo = parse_format(format, &pformatList[0], MAX_FORMAT_ELEMENTS);

	// Check that there are not too many format options
	if ( nfo != 1 ) {
		dest[0] = '\0';
		printerr(file, line, function, ESBADFMT); return SNPRFNEGATE(ESBADFMT);
	}
	// Check that the format is for an integer type
	if ( CHK_FORMAT(FMT_CHAR, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	return snprintf(dest, dmax, format, c);
}


inline int snprintf_debug_s_g(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, double g)
{
	char pformatList[MAX_FORMAT_ELEMENTS];
	unsigned int index = 0;

	// Determine the number of format options in the format string
	unsigned int  nfo = parse_format(format, &pformatList[0], MAX_FORMAT_ELEMENTS);

	// Check that there are not too many format options
	if ( nfo != 1 ) {
		dest[0] = '\0';
		printerr(file, line, function, ESBADFMT); return SNPRFNEGATE(ESBADFMT);
	}
	// Check that the format is for an integer type
	if ( CHK_FORMAT(FMT_DOUBLE, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	return snprintf(dest, dmax, format, g);
}



inline int snprintf_debug_s_space_s(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, int num_of_space, char * s)
{
	char pformatList[MAX_FORMAT_ELEMENTS];
	unsigned int index = 0;

	// Determine the number of format options in the format string
	unsigned int  nfo = parse_format(format, &pformatList[0], MAX_FORMAT_ELEMENTS);

	// Check that there are not too many format options
	if ( nfo != 1 ) {
		dest[0] = '\0';
		printerr(file, line, function, ESBADFMT); return SNPRFNEGATE(ESBADFMT);
	}
	// Check that the format is for an integer type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	return snprintf(dest, dmax, format, num_of_space, s);
}



inline int snprintf_debug_s_l(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, long a)
{
	char pformatList[MAX_FORMAT_ELEMENTS];
	unsigned int index = 0;

	// Determine the number of format options in the format string
	unsigned int  nfo = parse_format(format, &pformatList[0], MAX_FORMAT_ELEMENTS);

	// Check that there are not too many format options
	if ( nfo != 1 ) {
		dest[0] = '\0';
		printerr(file, line, function, ESBADFMT); return SNPRFNEGATE(ESBADFMT);
	}
	// Check that the format is for an long type
	if ( CHK_FORMAT(FMT_LONG, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	return snprintf(dest, dmax, format, a);
}


inline int snprintf_debug_s_ll(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, long long a)
{
	char pformatList[MAX_FORMAT_ELEMENTS];
	unsigned int index = 0;

	// Determine the number of format options in the format string
	unsigned int  nfo = parse_format(format, &pformatList[0], MAX_FORMAT_ELEMENTS);

	// Check that there are not too many format options
	if ( nfo != 1 ) {
		dest[0] = '\0';
		printerr(file, line, function, ESBADFMT); return SNPRFNEGATE(ESBADFMT);
	}
	// Check that the format is for an integer type
	if ( check_integer_format(pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	return snprintf(dest, dmax, format, a);
}


inline int snprintf_debug_s_si(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, char *s, int a)
{
	char pformatList[MAX_FORMAT_ELEMENTS];
	unsigned int index = 0;

	// Determine the number of format options in the format string
	unsigned int  nfo = parse_format(format, &pformatList[0], MAX_FORMAT_ELEMENTS);

	// Check that there are not too many format options
	if ( nfo != 2 ) {
		dest[0] = '\0';
		printerr(file, line, function, ESBADFMT); return SNPRFNEGATE(ESBADFMT);
	}
	// Check first format is of string type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check that the format is for an integer type
	if ( check_integer_format(pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	return snprintf(dest, dmax, format, s, a);
}

inline int snprintf_debug_s_is(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, int a, char *s)
{
	char pformatList[MAX_FORMAT_ELEMENTS];
	unsigned int index = 0;

	// Determine the number of format options in the format string
	unsigned int  nfo = parse_format(format, &pformatList[0], MAX_FORMAT_ELEMENTS);

	// Check that there are not too many format options
	if ( nfo != 2 ) {
		dest[0] = '\0';
		printerr(file, line, function, ESBADFMT); return SNPRFNEGATE(ESBADFMT);
	}

	// Check that the format is for an integer type
	if ( check_integer_format(pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;
	
	// Check first format is of string type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	return snprintf(dest, dmax, format, a, s);
}


inline int snprintf_debug_s_sii(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, char *s, int a, int b)
{
	char pformatList[MAX_FORMAT_ELEMENTS];
	unsigned int index = 0;

	// Determine the number of format options in the format string
	unsigned int  nfo = parse_format(format, &pformatList[0], MAX_FORMAT_ELEMENTS);

	// Check that there are not too many format options
	if ( nfo != 3 ) {
		dest[0] = '\0';
		printerr(file, line, function, ESBADFMT); return SNPRFNEGATE(ESBADFMT);
	}
	// Check first format is of string type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check that the format is for an integer type
	if ( check_integer_format(pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	return snprintf(dest, dmax, format, s, a, b);
}

inline int snprintf_debug_s_siii(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, char *s, int a, int b, int c)
{
	char pformatList[MAX_FORMAT_ELEMENTS];
	unsigned int index = 0;

	// Determine the number of format options in the format string
	unsigned int  nfo = parse_format(format, &pformatList[0], MAX_FORMAT_ELEMENTS);

	// Check that there are not too many format options
	if ( nfo != 4 ) {
		dest[0] = '\0';
		printerr(file, line, function, ESBADFMT); return SNPRFNEGATE(ESBADFMT);
	}
	// Check first format is of string type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check that the format is for an integer type
	if ( check_integer_format(pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	return snprintf(dest, dmax, format, s, a, b, c);
}


inline int snprintf_debug_s_sll(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, char *s, long long a)
{
	char pformatList[MAX_FORMAT_ELEMENTS];
	unsigned int index = 0;

	// Determine the number of format options in the format string
	unsigned int  nfo = parse_format(format, &pformatList[0], MAX_FORMAT_ELEMENTS);

	// Check that there are not too many format options
	if ( nfo != 2 ) {
		dest[0] = '\0';
		printerr(file, line, function, ESBADFMT); return SNPRFNEGATE(ESBADFMT);
	}
	// Check first format is of string type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check that the format is for an integer type
	if ( check_integer_format(pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	return snprintf(dest, dmax, format, s, a);
}

inline int snprintf_debug_s_sis(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, char *s, int a, char *s1)
{
	char pformatList[MAX_FORMAT_ELEMENTS];
	unsigned int index = 0;

	// Determine the number of format options in the format string
	unsigned int  nfo = parse_format(format, &pformatList[0], MAX_FORMAT_ELEMENTS);

	// Check that there are not too many format options
	if ( nfo != 3 ) {
		dest[0] = '\0';
		printerr(file, line, function, ESBADFMT); return SNPRFNEGATE(ESBADFMT);
	}
	// Check first format is of string type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check that the format is for an integer type
	if ( check_integer_format(pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check first format is of string type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;
	
	return snprintf(dest, dmax, format, s, a, s1);
}

inline int snprintf_debug_s_iis(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, int a, int b, char *s1)
{
	char pformatList[MAX_FORMAT_ELEMENTS];
	unsigned int index = 0;

	// Determine the number of format options in the format string
	unsigned int  nfo = parse_format(format, &pformatList[0], MAX_FORMAT_ELEMENTS);

	// Check that there are not too many format options
	if ( nfo != 3 ) {
		dest[0] = '\0';
		printerr(file, line, function, ESBADFMT); return SNPRFNEGATE(ESBADFMT);
	}

	// Check that the format is for an integer type
	if ( check_integer_format(pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check that the format is for an integer type
	if ( check_integer_format(pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check first format is of string type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;
	
	return snprintf(dest, dmax, format, a, b, s1);
}

inline int snprintf_debug_s_isis(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, int a, char *s, int b, char *s1)
{
	char pformatList[MAX_FORMAT_ELEMENTS];
	unsigned int index = 0;

	// Determine the number of format options in the format string
	unsigned int  nfo = parse_format(format, &pformatList[0], MAX_FORMAT_ELEMENTS);

	// Check that there are not too many format options
	if ( nfo != 4 ) {
		dest[0] = '\0';
		printerr(file, line, function, ESBADFMT); return SNPRFNEGATE(ESBADFMT);
	}

	// Check that the format is for an integer type
	if ( check_integer_format(pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check first format is of string type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check that the format is for an integer type
	if ( check_integer_format(pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check first format is of string type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;
	

	return snprintf(dest, dmax, format, a, s, b, s1);
}


inline int snprintf_debug_s_ss(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, char *s, char *s2)
{
	char pformatList[MAX_FORMAT_ELEMENTS];
	unsigned int index = 0;

	// Determine the number of format options in the format string
	unsigned int  nfo = parse_format(format, &pformatList[0], MAX_FORMAT_ELEMENTS);

	// Check that there are not too many format options
	if ( nfo != 2 ) {
		dest[0] = '\0';
		printerr(file, line, function, ESBADFMT); return SNPRFNEGATE(ESBADFMT);
	}
	// Check first format is of string type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check that the format is for an string type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	return snprintf(dest, dmax, format, s, s2);
}

inline int snprintf_debug_s_sss(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, char *s, char *s2, char *s3)
{
	char pformatList[MAX_FORMAT_ELEMENTS];
	unsigned int index = 0;

	// Determine the number of format options in the format string
	unsigned int  nfo = parse_format(format, &pformatList[0], MAX_FORMAT_ELEMENTS);

	// Check that there are not too many format options
	if ( nfo != 3 ) {
		dest[0] = '\0';
		printerr(file, line, function, ESBADFMT); return SNPRFNEGATE(ESBADFMT);
	}
	// Check first format is of string type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check that the format is of string type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check that the format is of string type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	return snprintf(dest, dmax, format, s, s2, s3);
}

inline int snprintf_debug_s_ssss(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, char *s, char *s2, char *s3, char *s4)
{
	char pformatList[MAX_FORMAT_ELEMENTS];
	unsigned int index = 0;

	// Determine the number of format options in the format string
	unsigned int  nfo = parse_format(format, &pformatList[0], MAX_FORMAT_ELEMENTS);

	// Check that there are not too many format options
	if ( nfo != 4 ) {
		dest[0] = '\0';
		printerr(file, line, function, ESBADFMT); return SNPRFNEGATE(ESBADFMT);
	}
	// Check first format is of string type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check that the format is of string type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check that the format is of string type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check that the format is of string type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	return snprintf(dest, dmax, format, s, s2, s3, s4);
}


inline int snprintf_debug_s_ssssi(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, char *s, char *s2, char *s3, char *s4, int a)
{
	char pformatList[MAX_FORMAT_ELEMENTS];
	unsigned int index = 0;

	// Determine the number of format options in the format string
	unsigned int  nfo = parse_format(format, &pformatList[0], MAX_FORMAT_ELEMENTS);

	// Check that there are not too many format options
	if ( nfo != 5 ) {
		dest[0] = '\0';
		printerr(file, line, function, ESBADFMT); return SNPRFNEGATE(ESBADFMT);
	}
	// Check first format is of string type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check that the format is of string type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check that the format is of string type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check that the format is of string type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check that the format is for an integer type
	if ( check_integer_format(pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	return snprintf(dest, dmax, format, s, s2, s3, s4, a);
}




inline int snprintf_debug_s_ii(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, int a, int b)
{
	char pformatList[MAX_FORMAT_ELEMENTS];
	unsigned int index = 0;

	// Determine the number of format options in the format string
	unsigned int  nfo = parse_format(format, &pformatList[0], MAX_FORMAT_ELEMENTS);

	// Check that there are not too many format options
	if ( nfo != 2 ) {
		dest[0] = '\0';
		printerr(file, line, function, ESBADFMT); return SNPRFNEGATE(ESBADFMT);
	}
	// Check that the format is for an integer type
	if ( check_integer_format(pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check that the format is for an integer type
	if ( check_integer_format(pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	return snprintf(dest, dmax, format, a, b);
}

inline int snprintf_debug_s_iiii(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, int a, int b, int c, int d)
{
	char pformatList[MAX_FORMAT_ELEMENTS];
	unsigned int index = 0;

	// Determine the number of format options in the format string
	unsigned int  nfo = parse_format(format, &pformatList[0], MAX_FORMAT_ELEMENTS);

	// Check that there are not too many format options
	if ( nfo != 4 ) {
		dest[0] = '\0';
		printerr(file, line, function, ESBADFMT); return SNPRFNEGATE(ESBADFMT);
	}
	// Check that the format is for an integer type
	if ( check_integer_format(pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check that the format is for an integer type
	if ( check_integer_format(pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check that the format is for an integer type
	if ( check_integer_format(pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check that the format is for an integer type
	if ( check_integer_format(pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	return snprintf(dest, dmax, format, a, b, c, d);
}



inline int snprintf_debug_s_sl(const char *file, int line, const char *function, char *dest, rsize_t dmax, const char *format, char *s, long a)
{
	char pformatList[MAX_FORMAT_ELEMENTS];
	unsigned int index = 0;

	// Determine the number of format options in the format string
	unsigned int  nfo = parse_format(format, &pformatList[0], MAX_FORMAT_ELEMENTS);

	// Check that there are not too many format options
	if ( nfo != 2 ) {
		dest[0] = '\0';
		printerr(file, line, function, ESBADFMT); return SNPRFNEGATE(ESBADFMT);
	}
	// Check first format is of string type
	if ( CHK_FORMAT(FMT_STRING, pformatList[index]) == 0) {
		dest[0] = '\0';
		printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	// Check that the format is for an integer type
	if ( CHK_FORMAT(FMT_LONG, pformatList[index]) == 0) {
			dest[0] = '\0';
			printerr(file, line, function, ESFMTTYP); return SNPRFNEGATE(ESFMTTYP);
	}
	index++;

	return snprintf(dest, dmax, format, s, a);
}
