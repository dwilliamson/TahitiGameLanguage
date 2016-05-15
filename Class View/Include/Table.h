
#ifndef	_INCLUDED_TABLE_H
#define	_INCLUDED_TABLE_H


#define TABLE_TYPE_FUNCTIONS	1
#define TABLE_TYPE_STATES		2


class Table : public TreeViewEntry
{
public:
	// Constructor/Destructor
	Table(CFile *file, int nb_entries, int type);
	virtual ~Table(void);

	// Print information about this treeview entry
	void	PrintInfo(void);

	// Add the table to the application treeview
	void	AddToTreeView(int level);

	// Type of table
	int		m_Type;

	// Number of entries in the table
	int		m_NbEntries;

	// All the entries
	TArray<TableEntry *>	m_Entries;
};


#endif	/* _INCLUDED_TABLE_H */