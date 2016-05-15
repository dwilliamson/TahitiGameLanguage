
#include "..\include\main.h"


#define IMAGE_CLASS			0
#define IMAGE_FUNCTION		1
#define IMAGE_IMPORT		2
#define IMAGE_OBJECT		3
#define IMAGE_SUPERCLASS	4
#define IMAGE_UNITCLOSED	5
#define IMAGE_STATE			6
#define IMAGE_VARIABLE		7
#define IMAGE_INTERFACE		8
#define IMAGE_TABLE			9
#define IMAGE_NONE			0xFF


TreeView::TreeView(HWND hWndParent)
{
	RECT	rcClient;

	// Get the dimensions of the parent window
	GetClientRect(hWndParent, &rcClient);

	// Take away a considerable portion of the right side for selection info
	rcClient.right = max(0, rcClient.right - 200 - 10);
	rcClient.bottom = max(0, rcClient.bottom - 20);

	// Create the treeview window
	hWnd = CreateWindowEx(WS_EX_CLIENTEDGE,
		WC_TREEVIEW,
		"Tree View",
		WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS,
		10,
		10,
		rcClient.right,
		rcClient.bottom,
		hWndParent,
		(HMENU)0x400,
		hInstMain,
		NULL);

	// Create the image list
	if ((hImgList = ImageList_Create(16, 16, FALSE, 8, 0)) == NULL)
		return;

	// Add all the images to the image list
	ImageIDs[IMAGE_CLASS] = AddImage(IDB_CLASS);
	ImageIDs[IMAGE_FUNCTION] = AddImage(IDB_FUNCTION);
	ImageIDs[IMAGE_IMPORT] = AddImage(IDB_IMPORT);
	ImageIDs[IMAGE_OBJECT] = AddImage(IDB_OBJECT);
	ImageIDs[IMAGE_SUPERCLASS] = AddImage(IDB_SUPERCLASS);
	ImageIDs[IMAGE_UNITCLOSED] = AddImage(IDB_UNITCLOSED);
	ImageIDs[IMAGE_STATE] = AddImage(IDB_STATE);
	ImageIDs[IMAGE_VARIABLE] = AddImage(IDB_VARIABLE);
	ImageIDs[IMAGE_INTERFACE] = AddImage(IDB_INTERFACE);
	ImageIDs[IMAGE_TABLE] = AddImage(IDB_TABLE);

	// Check to see all the images were added
	if (ImageList_GetImageCount(hImgList) != 10)
		return;

	// Associate the image list with the tree view control
	TreeView_SetImageList(hWnd, hImgList, TVSIL_NORMAL);

	// Set the previous item to be the first in the list
	hRootItem = TVI_ROOT;
	hItem = TVI_FIRST;
	hLevel = 0;
}


TreeView::~TreeView(void)
{
	// Release the image list
	ImageList_Destroy(hImgList);
}


int TreeView::AddImage(int resource)
{
	HBITMAP		hBmp;
	int			id;

	// Load the bitmap
	hBmp = LoadBitmap(hInstMain, MAKEINTRESOURCE(resource));

	// Add it to the image list
	id = ImageList_Add(hImgList, hBmp, (HBITMAP)NULL);

	// Release the resource
	DeleteObject(hBmp);

	return (id);
}


HTREEITEM TreeView::AddItem(char *name, int image, int level, void *data)
{
	TVITEM			tvi;
	TVINSERTSTRUCT	tvins;

	tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

	// Set the text of the item
	tvi.pszText = name;
	tvi.cchTextMax = strlen(name);

	// Set the entry images
	if (image == IMAGE_NONE)
	{
		tvi.iImage = 20;
		tvi.iSelectedImage = 20;
	}
	else
	{
		tvi.iImage = ImageIDs[image];
		tvi.iSelectedImage = ImageIDs[image];
	}

	// App-specific data
	tvi.lParam = (LPARAM)data;

	tvins.item = tvi;
	tvins.hInsertAfter = TVI_LAST;

	if (level > hLevel)
	{	
		ItemStack.Push(hRootItem);
		hRootItem = hItem;
	}
	else if (level < hLevel)
	{
		while (level != hLevel)
		{
			hRootItem = ItemStack.Pop();
			hLevel--;
		}
	}

	tvins.hParent = hRootItem;
	hLevel = level;

	// Add the item to the tree view control
	hItem = (HTREEITEM)SendMessage(hWnd, TVM_INSERTITEM, 0, (LPARAM)(LPTVINSERTSTRUCT)&tvins);

	return (hItem);
}


HTREEITEM TreeView::AddUnit(char *name, int level, void *data)
{
	return (AddItem(name, IMAGE_UNITCLOSED, level, data));
}


HTREEITEM TreeView::AddClass(char *name, int level, void *data)
{
	return (AddItem(name, IMAGE_CLASS, level, data));
}


HTREEITEM TreeView::AddFunction(char *name, int level, void *data)
{
	return (AddItem(name, IMAGE_FUNCTION, level, data));
}


HTREEITEM TreeView::AddImport(char *name, int level, void *data)
{
	return (AddItem(name, IMAGE_IMPORT, level, data));
}


HTREEITEM TreeView::AddObject(char *name, int level, void *data)
{
	return (AddItem(name, IMAGE_OBJECT, level, data));
}


HTREEITEM TreeView::AddSuperClass(char *name, int level, void *data)
{
	return (AddItem(name, IMAGE_SUPERCLASS, level, data));
}


HTREEITEM TreeView::AddVariable(char *name, int level, void *data)
{
	return (AddItem(name, IMAGE_VARIABLE, level, data));
}


HTREEITEM TreeView::AddState(char *name, int level, void *data)
{
	return (AddItem(name, IMAGE_STATE, level, data));
}


HTREEITEM TreeView::AddInterface(char *name, int level, void *data)
{
	return (AddItem(name, IMAGE_INTERFACE, level, data));
}


HTREEITEM TreeView::AddTable(char *name, int level, void *data)
{
	return (AddItem(name, IMAGE_TABLE, level, data));
}


HTREEITEM TreeView::AddTableEntry(char *name, int level, void *data)
{
	return (AddItem(name, IMAGE_NONE, level, data));
}


void TreeView::ResetPosition(void)
{
	// Set the previous item to be the first in the list
	hRootItem = TVI_ROOT;
	hLevel = 0;

	// Reset the stack
	ItemStack.Flush();
}


void TreeView::RemoveItem(HTREEITEM item)
{
	// Win32 stuff
	TreeView_DeleteItem(hWnd, item);
}