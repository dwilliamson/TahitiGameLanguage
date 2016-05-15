
#ifndef	_INCLUDED_MAIN_H
#define	_INCLUDED_MAIN_H


// C-lib includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// Enable the call stack in case of errors
#define	CALLSTACK_ENABLED
#define CALLSTACK_CLASS			g_call_stack


// Library includes
#include "..\..\Common\include\Common.h"
#include "..\..\Platform Specific\include\Platform.h"


class VMCFile;
class VMOFile;
class VMUFile;
class CodeGenerator;
class Tokeniser;
class IDLParser;

class	Environment;
class	ParseTree;
class	ParseTreeNode;
class		Expression;
class		FunctionCall;
class		Constant;
class		Name;
class		StringLiteral;


// Local program includes
#include "..\include\globals.h"
#include "..\include\Token.h"
#include "..\include\Tokeniser.h"
#include "..\include\VarType.h"
#include "..\include\Variable.h"
#include "..\include\Function.h"
#include "..\include\State.h"
#include "..\include\Class.h"
#include "..\include\CodeGenerator.h"
#include "..\include\VMOFile.h"
#include "..\include\VMCFile.h"
#include "..\include\CompileError.h"
#include "..\include\IDLParser.h"

#include "..\include\Environment.h"
#include "..\include\ParseTree.h"
#include "..\include\ParseTreeNode.h"
#include "..\include\Expression.h"
#include "..\include\FunctionCall.h"
#include "..\include\Constant.h"
#include "..\include\Name.h"
#include "..\include\StringLiteral.h"

#include "..\include\VMUFile.h"


#endif	/* _INCLUDED_MAIN_H */