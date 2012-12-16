// Copyright ©2012 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in "bss_util.h"

#ifndef __BSS_DEFINES_H__
#define __BSS_DEFINES_H__

#include "bss_compiler.h"

// Version numbers
#define BSS_VERSION_MAJOR 0
#define BSS_VERSION_MINOR 3
#define BSS_VERSION_REVISION 85

//sometimes the std versions of these are a bit overboard, so this redefines the MS version, except it will no longer cause conflicts everywhere
#define bssmax(a,b)            (((a) > (b)) ? (a) : (b))
#define bssmin(a,b)            (((a) < (b)) ? (a) : (b))
#define bssclamp(var,min,max) (((var)>(max))?(max):(((var)<(min))?(min):(var)))

// Use the following defines to force TODO items as warnings in the compile. VC++ currently does not support "messages" despite having
// them as a category in the error list.
// Usage: #pragma message(TODO "Clean up code here")
#define MAKESTRING2(x) #x
#define MAKESTRING(x) MAKESTRING2(x)
#define TODO __FILE__ "(" MAKESTRING(__LINE__) ") : warning (TODO): "
#define NOTICE __FILE__ "(" MAKESTRING(__LINE__) ") : warning (NOTICE): "
#define WIDEN2(x) L ## x 
#define WIDEN(x) WIDEN2(x) 
#ifndef __WFILE__
#define __WFILE__ WIDEN(__FILE__) 
#endif

// These are random number generator #defines. Note that RANDINTGEN is susceptible to modulo bias, so if you need a true distribution cast
// RANDFLOATGEN to int. Actually if you need a true distribution just use the damn std random class in C++11 like you're supposed to.
#ifndef RANDFLOATGEN
#define RANDFLOATGEN(min,max) (((max) - (min)) * (double)rand()/(double)RAND_MAX + (min))
#endif
#ifndef RANDINTGEN
#define RANDINTGEN(min,max) ((min)+(rand()%((int)((max)-(min))))) // This is [min,max) instead of [min,max] to facilitate use of length as max
#endif
#ifndef RANDBOOLGEN
#define RANDBOOLGEN() (rand()>(RAND_MAX>>1))
#endif

//These can be used to define properties
#define CLASS_PROP(typeref, varname, funcname) CLASS_PROP_READONLY(typeref,varname,funcname) CLASS_PROP_WRITEONLY(typeref,varname,funcname)
#define CLASS_PROP_VAL(typeref, varname, funcname) CLASS_PROP_READONLY(typeref,varname,funcname) CLASS_PROP_WRITEONLY_VAL(typeref,varname,funcname)
#define CLASS_PROP_READONLY(typeref, varname, funcname) inline typeref Get##funcname() const { return varname; }
#define CLASS_PROP_WRITEONLY(typeref, varname, funcname) inline void Set##funcname(const typeref m##varname##) { varname=m##varname##; }
#define CLASS_PROP_WRITEONLY_VAL(typeref, varname, funcname) inline void Set##funcname(typeref m##varname##) { varname=m##varname##; }

#define SAFESHIFT(v,s) ((s>0)?(v<<s):(v>>(-s))) //positive number shifts left, negative number shifts right, prevents undefined behavior.
#define T_GETBIT(type, bit) ((type)(((type)1)<<(((type)bit)%(sizeof(type)<<3)))) // Gets a bitmask using its 0-based index.
//#define T_GETBITRANGE(type, low, high) ((type)((((2<<((high)-(low)))-1)<<(low))|(((2<<((high)-(low)))-1)>>(-(low)))|(((2<<((high)-(low)))-1)<<((low)%(sizeof(type)<<3)))))
// Gets a bitmask for a range of bits. This range can be negative to start from the left end instead of the right.
#define T_GETBITRANGE(type, low, high) ((type)(SAFESHIFT(((2<<((high)-(low)))-1),low)|(((2<<((high)-(low)))-1)<<((low)%(sizeof(type)<<3))))) 
#define MAKEVERSIONINT(major,minor,revision) ((major) + ((minor)<<8) + ((revision)<<16)) // Packs version information into a 32-bit integer
// This calculates the most significant bit in an 8-bit integer (CHAR) at compile time. It's really just a basterdized version of
// i |= i >> 1; i |= i >> 2; i |= i >> 4; i -= i >> 1; so that it happens during compile time.
#define T_CHARGETMSB(i) ((((i | (i>>1)) | ((i | (i>>1))>>2)) | (((i | (i>>1)) | ((i | (i>>1))>>2))>>4)) - ((((i | (i>>1)) | ((i | (i>>1))>>2)) | (((i | (i>>1)) | ((i | (i>>1))>>2))>>4))>>1)) // I CAN'T BELIEVE THIS WORKS（ ﾟДﾟ）

#if defined(BSS_PLATFORM_POSIX) || defined(BSS_PLATFORM_MINGW)
#define BSSPOSIX_WCHAR(s) s
#define BSSPOSIX_CHAR(s) cStr(s).c_str()
#elif defined(BSS_PLATFORM_WIN32)
#define BSSPOSIX_WCHAR(s) cStrW(s).c_str()
#define BSSPOSIX_CHAR(s) s
#endif

//unsigned shortcuts
#ifndef BSS_NOSHORTTYPES
#ifdef  __cplusplus
namespace bss_util { // This namespace can be inconvenient but its necessary because everything else on earth tries to define these too.
#endif
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned __int64 uint64;
typedef unsigned long ulong;
typedef unsigned long long ulonglong;
#ifdef  __cplusplus
}
#endif
#endif

#ifndef BSS_NO_FASTCALL
#define BSS_FASTCALL BSS_COMPILER_FASTCALL
#else
#define BSS_FASTCALL BSS_COMPILER_STDCALL
#endif

#endif
