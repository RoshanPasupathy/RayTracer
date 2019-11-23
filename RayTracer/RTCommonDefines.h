#ifndef _RT_COMMON_DEFINES_H_
#define _RT_COMMON_DEFINES_H_

#define MAX_SHAPES 2000
#define MAX_THREADS 16
#define PI 3.14

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

enum acctype
{
	RT_NONE,
	RT_BVH,
	RT_AC_BVH,
};

acctype g_acctype = RT_NONE;

#endif
