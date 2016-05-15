
#ifndef	_INCLUDED_VMUNIT_H
#define	_INCLUDED_VMUNIT_H


class VMUnit : CHashCell
{
public:
	// Load a particular unit
	void		Load(const char *filename);

	// Get the unit name
	char		*GetName(void);

	// Create an object of one of the classes in this unit
	VMObject	*NewObject(const char *name, VirtualMachine *machine);

	// Process each thread in the unit for this frame
	void		ProcessFrame(int nb_instructions);

	// Get a class pointer from the unit
	VMClass		*GetClass(char *name);

	friend class	VirtualMachine;

	// Pointers for the linked list
	VMUnit		*prev, *next;

private:
	// Private constructor/destructor
	VMUnit(void);
	~VMUnit(void);

	// Name of the unit
	char	*m_Name;

	// Unit code segment
	char	*m_CodeSegment;

	// Size of the code segment
	int		m_CodeSize;

	// List of classes belonging to this unit
	TLinkedList<VMClass>	m_ClassList;

	// List of instantiated classes (objects)
	TLinkedList<VMObject>	m_ObjectList;

	// List of active threads
	TLinkedList<VMThread>	m_ThreadList;

	// Hash table of classes for quick lookup
	THash<>		m_ClassHash;
};


#endif	/* _INCLUDED_VMUNIT_H */