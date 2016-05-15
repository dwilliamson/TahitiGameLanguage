
#ifndef	_INCLUDED_UNIT_H
#define	_INCLUDED_UNIT_H


class Unit : public TreeViewEntry
{
public:
	// Constructor/Destructor
	Unit(char *filename);
	virtual ~Unit(void);

	// Print information about this treeview entry
	void	PrintInfo(void);

	// Add the unit to the application treeview
	void	AddToTreeView(void);

	// Remove the unit from the application treeview
	void	RemoveFromTreeView(void);

	// Unit name
	char	m_Name[256];

	// Fully justified path of the file
	char	m_Location[MAX_PATH];

	// Number of classes in the unit
	int		m_NbClasses;

	// Size of the code segment
	int		m_CodeSize;

	// List of classes in the unit
	THash<>	m_ClassList;

	// List of unit base classes
	TArray<Class *>	m_BaseClasses;

	// Tree item representing this unit
	HTREEITEM	hTreeItem;
};


#endif	/* _INCLUDED_UNIT_H */