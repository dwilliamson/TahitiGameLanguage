
#ifndef	_INCLUDED_TABLEENTRY_H
#define	_INCLUDED_TABLEENTRY_H


class TableEntry : public TreeViewEntry
{
public:
	// Constructor
	TableEntry(int address) { m_Address = address; };

	// Print information about this treeview entry
	void	PrintInfo(void);

	// Address in the code segment
	int		m_Address;
};


#endif