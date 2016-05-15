
#ifndef	_INCLUDED_VMUFILE_H
#define	_INCLUDED_VMUFILE_H


class VMUFile
{
public:
	// Constructor/Destructor
	VMUFile(const char *filename);
	~VMUFile(void);

	// Add an object file to the unit file
	void		AddObjectCode(Class *class_ptr);

	// Add class information to the unit file
	void		AddClassInfo(Class *class_ptr);

	// Write a class data segment
	void		WriteDataSegment(Class *class_ptr, char *data, int size);

	// Write an int at a specified position in file yet retain the current position
	void		BackpatchInt(int what, int where);

	// Write a value to the unit
	void		WriteInt(int value);

private:
	// Output file
	CFile		*file;

	// Size of the code segment
	int			code_size;
};


#endif	/* _INCLUDED_VMUFILE_H */