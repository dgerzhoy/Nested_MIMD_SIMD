/*
 *
 * 
 *	Author: Daniel Gerzhoy
 *	email: dgerzhoy@umd.edu
 *
 *	This work is meant for academic use only. The author claims no ownership of any code herein, or responsibility for its use.
 */
#pragma once

//Begin FFT6 Header Stuff

#ifndef PI
  #define PI (3.141592653589793f)
#endif

#ifndef M_PI_2
#define M_PI_2   1.57079632679489661923  /* pi/2 */
#endif

typedef float real_type;

typedef struct { real_type re, im; } complex;


#define NUM_GROUPS 24
