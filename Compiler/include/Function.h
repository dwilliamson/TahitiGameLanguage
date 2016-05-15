
#ifndef	_INCLUDED_FUNCTION_H
#define	_INCLUDED_FUNCTION_H


#define FFLAGS_PROTECTION_MASK			0x0007
#define FFLAGS_PUBLIC					0x0001
#define FFLAGS_PROTECTED				0x0002
#define FFLAGS_PRIVATE					0x0004

#define FFLAGS_FINAL					0x0008
#define FFLAGS_IMPORTED					0x0010


class Function : public CHashCell
{
public:
	// Constructor
	Function(void);

	int			GetNumParams(void);

	Variable	*AddParameter(VarType &type, char *name);
	Variable	*AddLocal(VarType &type, char *name);

	// Get the function parameter by index
	Variable	*GetParameter(int which);

	void		WriteReturn(VMOFile *object);

	// Function index within class
	int			GetIndex(void);
	void		SetIndex(int value);

	// Flag control
	void		SetFlag(int which);
	void		ClearFlag(int which);
	int			GetFlags(void);

	int			GetCurAddress(void);
	int			GetCurPAddress(void);

	// Set the function return type
	void		SetReturnType(VarType &type);

	// Get the type of variable that the function returns
	VarType		GetReturnType(void);

	// Location control
	int			GetCodeLocation(void);
	void		SetCodeLocation(int where);

	// Re-arrange the parameter addresses so that they are relative to EBP
	void		FixAddresses(void);
	
	// Make the addresses differ by units of one
	void		SingulariseAddresses(void);

	// Parameter as well as local list
	THash<>		local_list;

	// Whether there is a final return in the function
	int			had_return;

	// Write the class information to file
	void		Write(CFile *file);

	// Read the class information from file
	void		Read(CFile *file);

	// Write VMU tailored information about the class
	void		WriteVMUInfo(CFile *file);

private:
	// List of parameters (in the correct order)
	TArray<Variable *>	param_list;

	// Type of the return value, if any
	VarType		ret_type;

	// Number of parameters passed through the function
	int			nb_params;

	// Current address
	int			cur_address;

	// Current parameter address
	int			cur_paddress;

	// Location of this function in the file object
	int			location;

	// Just some stuff
	int			flags;

	// Index of function within class
	int			index;
};


#endif	/* _INCLUDED_FUNCTION_H */