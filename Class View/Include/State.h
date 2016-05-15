
#ifndef	_INCLUDED_STATE_H
#define	_INCLUDED_STATE_H


class State : public CHashCell, public TreeViewEntry
{
public:
	// Constructor/Destructor
	State(CFile *file);
	virtual ~State(void);

	// Print information about this treeview entry
	void	PrintInfo(void);

	// Add the state to the application treeview
	void	AddToTreeView(int level);

	// State name
	char	m_Name[256];

	// Position in code where state is located
	int		m_Position;

	// Where the 'begin' label is
	int		m_CodeStart;

	// Index within the class
	int		m_Index;

	// Number of functions in the state
	int		m_NbFunctions;

	// List of functions in the state
	THash<>	m_FunctionList;
};


#endif	/* _INCLUDED_STATE_H */