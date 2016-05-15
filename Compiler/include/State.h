
#ifndef	_INCLUDED_STATE_H
#define _INCLUDED_STATE_H


class State : public CHashCell
{
public:
	// Constructor/Destructor
	State(void);
	~State(void);

	// Code position control
	void		SetPosition(int where);
	int			GetPosition(void);

	// Code execution start control
	void		SetCodeStart(int where);
	int			GetCodeStart(void);

	// Index control
	void		SetIndex(int value);
	int			GetIndex(void);

	// List of all functions in the state
	THash<>		functions;

	// Write the class information to file
	void		Write(CFile *file);

	// Read the class information from file
	void		Read(CFile *file);

	// Write VMU tailored information about the class
	void		WriteVMUInfo(CFile *file);

private:
	// Position in code where the state starts
	int			position;

	// Where to start executing code in this state
	int			code_start;

	// Index of this state in the class listing
	int			index;
};


#endif	/* _INCLUDED_STATE_H */