
#ifndef	_INCLUDED_IVIRTUALMACHINE_H
#define	_INCLUDED_IVIRTUALMACHINE_H



// === Helper Macros =======================================================================

#define	vm_DECLARE_EXPORTED_CLASS(c)						\
	public:													\
	static void RegisterMethods(void);						\
	static void *vm_Allocate(void) { return new c; }		\
	static void vm_Destroy(void *i) { delete (c *)i; }


#define vm_START_METHOD_REGISTER(c)							\
	typedef void (c::*MethodPtr)(void);						\
	MethodPtr method_ptr;									\
	iVirtualMachine *vm = iVirtualMachine::GetInstance();	\
	int address = 0


#define vm_REGISTER_METHOD(m, r, p)							\
	method_ptr = (MethodPtr)&m;								\
	__asm { mov ecx, method_ptr }							\
	__asm { mov address, ecx }								\
	vm->RegisterMethod(#m, address)


#define vm_END_METHOD_REGISTER


#define vm_REGISTER_CLASS(c, e)																\
	iVirtualMachine::GetInstance()->RegisterClass(#c, &c::vm_Allocate, &c::vm_Destroy, #e);	\
	c::RegisterMethods()

// =========================================================================================



// === Forward declarations ================================================================
class	VMUnit;
class	VMObject;
// =========================================================================================



// === Virtual Machine Interface ===========================================================

class iVirtualMachine
{
public:
	// Retrieve the only virtual machine instance
	static iVirtualMachine	*
		GetInstance(void);

	// Set the number of instructions to be processed per thread per frame
	virtual void
		SetInstructionFrame(int nb_instructions) = 0;

	// Process each thread for this frame
	virtual void
		ProcessFrame(void) = 0;

	// Load a virtual machine unit
	virtual VMUnit *
		LoadUnit(const char *filename) = 0;

	// Create a new object instance
	virtual VMObject *
		NewObject(const char *name) = 0;

	// Register an exported native class
	virtual void
		RegisterClass(char *name, void *(*alloc)(void), void (*release)(void *), char *extends) = 0;

	// Register an exported native method
	virtual void
		RegisterMethod(char *name, int address) = 0;

	// Retrieve the C++ object pointer, if the passed object inherits from one
	virtual void *
		GetNativeObject(VMObject *object_ptr) = 0;
};

// =========================================================================================


#endif	/* _INCLUDED_IVIRTUALMACHINE_H */