#pragma once

const float MATH_PI = 3.141592654f;
const float MATH_EPSILON = 0.0005f;//1.0e-5;
const double MATH_EPSILON2 = 0.0000000001f;//1.0e-10;
#define SQR(x)		( (x) * (x) )
#define LIMIT_RANGE(low, value, high)	{	if (value < low)	value = low;	else if(value > high)	value = high;	}
#define FLOAT_EQ(x,v)	(fabs((x)-(v)) < (MATH_EPSILON))
#define ABS( x )	( (x) >= 0 ? (x) : -(x) )
#define ANGLE2RAD(angle) ((MATH_PI*(angle)) * (0.00555555f))
#define RAD2ANGLE(radian) ((radian) * (180.0f/MATH_PI))

#define SAFE_RELEASE(p) {if (p) { p->Release(); p=NULL;} }
#define RET(exp)		{if((exp)) return; }			// exp가 true이면 리턴
#define RET2(exp)		{if((exp)) {assert(0); return;} }			// exp가 true이면 리턴
#define RETV(exp,val)	{if((exp)) return val; }
#define RETV2(exp,val)	{if((exp)) {assert(0); return val;} }
#define ASSERT_RET(exp)		{assert(exp); RET(!(exp) ); }
#define ASSERT_RETV(exp,val)		{assert(exp); RETV(!(exp),val ); }
#define BRK(exp)		{if((exp)) break; }			// exp가 break


#include <Windows.h>
#include <d3d11.h>
#include <dxgitype.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
using namespace DirectX;

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "vector3.h"
#include "matrix44.h"
#include "quaternion.h"
#include "vertexbuffer.h"
#include "shader11.h"

using std::string;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
