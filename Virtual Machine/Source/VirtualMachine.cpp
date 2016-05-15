
#include "..\include\main.h"


VirtualMachine::VirtualMachine(void)
{
	m_InstructionSlice = 0;
}

// --- Singleton -------------------------------------------------------------------------

iVirtualMachine *iVirtualMachine::GetInstance(void)
{
	return (VirtualMachine::GetInstance());
}


VirtualMachine *VirtualMachine::GetInstance(void)
{
	static VirtualMachine	vm;

	return (&vm);
}

// --------------------------------------------------------------------------------------

void VirtualMachine::SetInstructionFrame(int nb_instructions)
{
	m_InstructionSlice = nb_instructions;
}


void VirtualMachine::ProcessFrame(void)
{
	VMUnit	*unit_ptr;

	// Walk the unit list processing their threads
	for (unit_ptr = m_UnitList.GetFirst(); unit_ptr; unit_ptr = unit_ptr->next)
		unit_ptr->ProcessFrame(m_InstructionSlice);
}


VMUnit *VirtualMachine::LoadUnit(const char *filename)
{
	VMUnit		*unit_ptr;

	// Allocate the unit
	if ((unit_ptr = new VMUnit) == NULL)
		return (NULL);

	// Load the unit
	unit_ptr->Load(filename);

	// Add the unit to the list
	m_UnitList.AddLast(unit_ptr);

	// Add the unit to the hash table
	m_UnitHash.Add(unit_ptr->GetName(), unit_ptr);

	return (unit_ptr);
}


VMObject *VirtualMachine::NewObject(const char *name)
{
	VMObject	*object_ptr;
	VMUnit		*unit_ptr;
	char		unit_name[256];

	// Isolate the unit name
	strcpy(unit_name, name);
	*strchr(unit_name, '.') = 0;
	_strlwr(unit_name);

	// Get the intended unit
	if ((unit_ptr = (VMUnit *)m_UnitHash.GetEntry(unit_name)) == NULL)
		return (NULL);

	// Point to the class name
	name = strchr(name, '.') + 1;

	// Create the new object via the unit
	object_ptr = unit_ptr->NewObject(name, this);

	return (object_ptr);
}


void VirtualMachine::RegisterClass(char *name, void *(*alloc)(void), void (*release)(void *), char *extends)
{
	// Bail out if the class already exists
	if (m_NativeClasses.GetEntry(name))
		return;

	// Allocate the new native class
	m_CurNative = new NativeClass;

	// Does this class extend another one?
	if (strcmp(extends, "vm_InheritsNone"))
	{
		NativeClass		*class_ptr;

		// Get the class it extends
		class_ptr = (NativeClass *)m_NativeClasses.GetEntry(extends);

		// Do some inheritance
		if (class_ptr)
			m_CurNative->SetSuperClass(class_ptr);
	}

	// Copy the class data
	m_CurNative->SetName(name);
	m_CurNative->SetAllocator(alloc);
	m_CurNative->SetDestroyer(release);

	// Add it to the class list
	m_NativeClasses.Add(m_CurNative->GetName(), m_CurNative);
}


void VirtualMachine::RegisterMethod(char *name, int address)
{
	// Can't add to a NULL class!
	if (m_CurNative == NULL)
		return;

	// Add the method to the class
	m_CurNative->RegisterMethod(name, address);
}


void *VirtualMachine::GetNativeObject(VMObject *object_ptr)
{
	if (object_ptr)
		return (object_ptr->GetNativeObject());
	
	return (0);
}


NativeMethod *VirtualMachine::GetMethod(char *class_name, char *method_name)
{
	NativeClass	*class_ptr;

	// Get the class pointer
	if ((class_ptr = (NativeClass *)m_NativeClasses.GetEntry(class_name)) == NULL)
		return (NULL);

	// Return the method address
	return (class_ptr->GetMethod(method_name));
}