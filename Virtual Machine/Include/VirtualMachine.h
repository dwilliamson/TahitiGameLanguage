
#ifndef	_INCLUDED_VIRTUALMACHINE_H
#define	_INCLUDED_VIRTUALMACHINE_H


class VirtualMachine : public iVirtualMachine
{
public:
	// Retrieve the only virtual machine instance
	static VirtualMachine	*GetInstance(void);

	// Set the number of instructions to be processed per thread per frame
	void		SetInstructionFrame(int nb_instructions);

	// Process each thread for this frame
	void		ProcessFrame(void);

	// Load a virtual machine unit
	VMUnit		*LoadUnit(const char *filename);

	// Create a new object instance
	VMObject	*NewObject(const char *name);

	// Add an exported native class
	void		RegisterClass(char *name, void *(*alloc)(void), void (*release)(void *), char *extends);

	// Add an exported native method
	void		RegisterMethod(char *name, int address);

	// Retrieve the C++ object pointer if the passed object inherits from one
	void		*GetNativeObject(VMObject *object_ptr);

	// Get the an exported native method
	NativeMethod	*GetMethod(char *class_name, char *method_name);

	friend class	VMUnit;

private:
	// Private constructor so the class can't be created
	VirtualMachine(void);

	// Number of instructions to be processed per thread per frame
	int		m_InstructionSlice;

	// List of units
	TLinkedList<VMUnit>		m_UnitList;

	// Unit hash table for quick lookup
	THash<>		m_UnitHash;

	// All the native classes to be exported
	THash<>		m_NativeClasses;

	// Current native class (for method addition)
	NativeClass	*m_CurNative;
};


#endif	/* _INCLUDED_VIRTUALMACHINE_H */