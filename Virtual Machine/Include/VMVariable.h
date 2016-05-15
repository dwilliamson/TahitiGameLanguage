
#ifndef	_INCLUDED_VMVARIABLE_H
#define	_INCLUDED_VMVARIABLE_H


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
#define VARIABLE_TYPEID_BINARY			0x1000			/* Non-arithmetic */


class VMVariable
{
public:
	// Constructor/Destructor
	VMVariable(void);
	VMVariable(CFile *file);
	~VMVariable(void);

	// Set the variable type ID
	void	SetTypeID(int type_id);

	// Set the variable type name
	void	SetTypeName(char *type_name);

	// Set the number of elements in the type array
	void	SetArrayElements(int nb_elements);

	// Pointers for the linked list
	VMVariable	*next, *prev;

private:
	// Name of the variable
	char	*m_Name;

	// Address within the data segment
	int		m_Address;

	// Variable type ID
	int		m_TypeID;

	// Name of the variable type
	char	*m_TypeName;

	// Is this variable an array?
	int		m_Array;

	// Number of elements in the array
	int		m_NbElements;
};


#endif	/* _INCLUDED_VMVARIABLE_H */