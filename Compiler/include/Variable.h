
#ifndef	_INCLUDED_VARIABLE_H
#define	_INCLUDED_VARIABLE_H


#define VARIABLE_SCOPE_MEMBER			0x0001
#define VARIABLE_SCOPE_LOCAL			0x0002
#define VARIABLE_SCOPE_PARAMETER		0x0004


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
#define VARIABLE_TYPEID_STRINGLIT		0x0080
#define VARIABLE_TYPEID_BINARY			0x1000			/* Non-arithmetic */


//#define VFLAGS_PROTECTION_MASK			0x0007
//#define VFLAGS_PUBLIC					0x0001
//#define VFLAGS_PROTECTED				0x0002
//#define VFLAGS_PRIVATE					0x0004


union dynamic
{
	char			c;			// Char type
	short			s;			// Short type
	int				i;			// Integer type
	unsigned char	uc;			// Unsigned char type
	unsigned short	us;			// Unsigned short type
	unsigned int	ui;			// Unsigned int type
	float			f;			// Floating point type
	unsigned int	p;			// Custom type (pointer)
};


class Variable : public CHashCell
{
public:
	// Constructor
	Variable(void);

	// Scope control
	void	SetScope(int value);
	int		GetScope(void);

	// Array size control
	void	SetArrayElements(int value);
	int		GetArrayElements(void);

	// Address control
	void	SetAddress(int value);
	int		GetAddress(void);

	// Set the variable type
	void	SetType(VarType &type);

	// Make the variable type by ID
	void	MakeType(int id);

	// Type control
	VarType	&GetType(void);

	// Sub-type control
	void	SetFlag(int value);
	void	ClearFlag(int value);
	int		GetFlags(void);

	// Default value control
	void	SetDefaultValue(void *value);
	void	GetDefaultValue(void *value);

	// Default array value control
	void	SetElementValue(int index, void *value);
	void	GetElementValue(int index, void *value);

	// Write class information to file
	void	Write(CFile *file);

	// Read class information from file
	void	Read(CFile *file);

	// Write VMU tailored class information to file
	void	WriteVMUInfo(CFile *file);

private:
	// What scope the variable exists within
	int		scope;

	// Size in bytes it takes to store this type
	int		size;

	// Address of variable
	int		address;

	// Type of variable
	VarType	type;

	// Variable flags
	int		flags;

	// What the original value for the MEMBER VARIABLE is
	dynamic		default_value;

	// Default value for arrays
	dynamic		*array_values;
};


#endif	/* _INCLUDED_VARIABLE_H */