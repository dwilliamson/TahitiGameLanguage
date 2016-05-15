
#ifndef	_INCLUDED_VARIABLE_H
#define	_INCLUDED_VARIABLE_H


#define VARIABLE_TYPEID_VOID			0x0000
#define VARIABLE_TYPEID_CHAR			0x0001
#define VARIABLE_TYPEID_SHORT			0x0002
#define VARIABLE_TYPEID_INT				0x0004
#define VARIABLE_TYPEID_FLOAT			0x0008
#define VARIABLE_TYPEID_UNSIGNED		0x0010
#define VARIABLE_TYPEID_UCHAR			(VARIABLE_TYPEID_CHAR | VARIABLE_TYPEID_UNSIGNED)
#define VARIABLE_TYPEID_USHORT			(VARIABLE_TYPEID_SHORT | VARIABLE_TYPEID_UNSIGNED)
#define VARIABLE_TYPEID_UINT			(VARIABLE_TYPEID_INT | VARIABLE_TYPEID_UNSIGNED)
#define VARIABLE_TYPEID_CUSTOM			0x0020
#define VARIABLE_TYPEID_STRING			0x0040
#define VARIABLE_TYPEID_BINARY			0x1000			/* Non-arithmetic */


class Variable : public CHashCell, public TreeViewEntry
{
public:
	// Constructor/Destructor
	Variable(void);
	Variable(CFile *file);
	virtual ~Variable(void);

	// Print information about this treeview entry
	void	PrintInfo(void);

	// Variable name
	char	m_Name[256];

	// Address of variable
	int		m_Address;

	// Variable type ID
	int		m_TypeID;

	// Type name if it's custom
	char	m_TypeName[256];

	// Is this variable an array?
	int		m_Array;

	// Number of elements in the array
	int		m_NbElements;
};


#endif	/* _INCLUDED_VARIABLE_H */