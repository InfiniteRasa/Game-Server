// SalsaLib is just a central place for helper code files and also platform dependent code
// Currently contains:
// The typedefs for the type short-forms
// An hashtable implementation
// Stuff to read files and configuration files
// Thread abstraction

#ifndef __SALSALIB
#define __SALSALIB

#include <Windows.h>

typedef unsigned long long 	uint64;
typedef signed long long	sint64;

typedef unsigned int 	uint32;
typedef signed int 		sint32;

typedef unsigned short 	uint16;
typedef signed short 	sint16;

typedef unsigned char 	uint8;
typedef char 			sint8;	// not declared as signed char to avoid incompatibilities between strings of sint8* and char*

#include "ThreadsUtils.h"
#include "fileMgr.h"
#include "hashTable.h"


#endif