
#ifndef	_INCLUDED_VMSTATE_H
#define	_INCLUDED_VMSTATE_H


class VMState
{
public:
	// Constructor/Destructor
	VMState(CFile *file);
	~VMState(void);

	// Pointers for the linked list
	VMState	*prev, *next;

private:
	// Name of the state
	char	*m_Name;

	// Where the state is located in code
	int		m_Position;

	// Location of the 'begin' label
	int		m_CodeStart;

	// Index within the class
	int		m_Index;

	// List of functions in the state
	TLinkedList<VMFunction>	m_FunctionList;
};


#endif	/* _INCLUDED_VMSTATE_H */