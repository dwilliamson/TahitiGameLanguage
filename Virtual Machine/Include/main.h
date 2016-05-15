
#ifndef	_INCLUDED_MAIN_H
#define	_INCLUDED_MAIN_H


#ifdef	_DEBUG

#else
	// Turn x86 specific optimisations on
	#define	TARGET_x86
#endif


// Enable register calling convention in an optimised build
#ifdef	TARGET_x86
	#define CALLTYPE	__fastcall
#else
	#define CALLTYPE
#endif


// C-lib includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// Library includes
#include "..\..\Common\include\Common.h"
#include "..\..\Platform Specific\include\Platform.h"


// Forward declarations
class VirtualMachine;
class VMUnit;
class VMThread;
class VMClass;
class VMObject;
class VMVariable;
class VMFunction;
class VMImport;
class VMState;
class NativeClass;
class NativeMethod;


// Local program includes
#include "..\include\opcodes.h"
#include "..\include\utils.h"
#include "..\include\iVirtualMachine.h"
#include "..\include\VirtualMachine.h"
#include "..\include\VMUnit.h"
#include "..\include\VMThread.h"
#include "..\include\VMClass.h"
#include "..\include\VMObject.h"
#include "..\include\VMVariable.h"
#include "..\include\VMFunction.h"
#include "..\include\VMImport.h"
#include "..\include\VMState.h"
#include "..\include\NativeClass.h"
#include "..\include\NativeMethod.h"


#endif	/* _INCLUDED_MAIN_H */