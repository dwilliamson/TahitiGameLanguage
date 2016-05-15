
#ifndef	_INCLUDED_MAIN_H
#define	_INCLUDED_MAIN_H


#define PRINT_FLAGS(x, y)	if (x & y) g_Edit->Printf(#y "\n")
#define PRINT_VALUE(x, y)	if (x == y) g_Edit->Printf(#y)


#define DISPLAY_UNIT			0x0001
#define DISPLAY_CLASS			0x0002
#define DISPLAY_FUNCTION		0x0004
#define DISPLAY_IMPORT			0x0008
#define DISPLAY_OBJECT			0x0010
#define DISPLAY_STATE			0x0020
#define DISPLAY_VARIABLE		0x0040
#define DISPLAY_INTERFACE		0x0080
#define DISPLAY_TABLE			0x0100
#define DISPLAY_ALL				0xFFFF


// C-lib includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// Library includes
#include "..\..\Common\include\Common.h"
#include "..\..\Platform Specific\include\Platform.h"


// Win32 includes
#include <windows.h>
#include <commctrl.h>


// Local program includes
#include "resource.h"
#include "TreeView.h"
#include "TreeViewEntry.h"
#include "EditBox.h"
#include "OptionBox.h"
#include "Variable.h"
#include "Function.h"
#include "State.h"
#include "TableEntry.h"
#include "Table.h"
#include "Class.h"
#include "Unit.h"


extern	HINSTANCE	hInstMain;
extern	HWND		hWndMain;

extern	TreeView	*g_Tree;
extern	EditBox		*g_Edit;
extern	int			g_Display;


#endif	/* _INCLUDED_MAIN_H */