
#ifndef	_INCLUDED_TREEVIEW_H
#define	_INCLUDED_TREEVIEW_H


class TreeView
{
public:
	// Constructor/Destructor
	TreeView(HWND hWndParent);
	~TreeView(void);

	// Add a VM unit to the treeview
	HTREEITEM	AddUnit(char *name, int level, void *data);

	// Add a class to the treeview
	HTREEITEM	AddClass(char *name, int level, void *data);

	// Add a function to the treeview
	HTREEITEM	AddFunction(char *name, int level, void *data);

	// Add an imported function to the treeview
	HTREEITEM	AddImport(char *name, int level, void *data);

	// Add an object to the treeview
	HTREEITEM	AddObject(char *name, int level, void *data);

	// Add a superclass to the treeview
	HTREEITEM	AddSuperClass(char *name, int level, void *data);

	// Add a variable to the treeview
	HTREEITEM	AddVariable(char *name, int level, void *data);

	// Add a state to the treeview
	HTREEITEM	AddState(char *name, int level, void *data);

	// Add an interface to the treeview
	HTREEITEM	AddInterface(char *name, int level, void *data);

	// Add either a virtual function or state table to the treeview
	HTREEITEM	AddTable(char *name, int level, void *data);

	// Add a table entry to the treeview
	HTREEITEM	AddTableEntry(char *name, int level, void *data);

	// Prepare for adding a new root item
	void		ResetPosition(void);
	
	// Remove an item and its children from the treeview
	void		RemoveItem(HTREEITEM item);

private:
	// Add an image to the treeview image list
	int			AddImage(int resource);

	// Add an item to the treeview
	HTREEITEM	AddItem(char *name, int image, int level, void *data);

	// Handle to the treeview control
	HWND		hWnd;

	// Handle to the icon image list
	HIMAGELIST	hImgList;

	// Image IDs to pass to treeview
	int			ImageIDs[10];
	
	TStack<HTREEITEM>	ItemStack;

	int			hLevel;

	HTREEITEM	hRootItem;

	HTREEITEM	hItem;
};


#endif	/* _INCLUDED_TREEVIEW_H */