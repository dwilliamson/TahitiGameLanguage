
#include "..\include\main.h"


NativeClass::NativeClass(void)
{
	m_Allocate = NULL;
	m_Destroy = NULL;
	m_SuperClass = NULL;
}


void NativeClass::RegisterMethod(char *name, int address)
{
	NativeMethod	*method_ptr;
	int				index;

	// Check to see if this is a virtual function over-ride
	if (m_SuperClass && (index = m_SuperClass->GetMethodIndex(name)) != -1)
	{
		// Over-ride the address of classes version of the function
		m_MethodList(index)->m_Address = address;
	}
	else
	{
		int		methods;

		// Get the current number of methods
		methods = m_MethodList.GetPosition();

		// Allocate a new method
		method_ptr = new NativeMethod;

		// Copy the method information
		method_ptr->SetName(name);
		method_ptr->m_Address = address;
		method_ptr->m_Index = methods;

		// Add the method to the lists
		m_MethodList.Add(method_ptr);
		m_MethodHash.Add(method_ptr->GetName(), method_ptr);
	}
}


void *NativeClass::NewInstance(void)
{
	// Check
	if (m_Allocate == NULL)
		return (NULL);

	// Call the allocation
	return (m_Allocate());
}


void NativeClass::DeleteInstance(void *instance)
{
	// Check
	if (m_Destroy == NULL)
		return;

	// Call the destroy
	m_Destroy(instance);
}


void NativeClass::SetSuperClass(NativeClass *super_class)
{
	int		x;

	// Register
	m_SuperClass = super_class;

	// Inherit the vtable from above
	for (x = 0; x < super_class->m_MethodList.GetPosition(); x++)
	{
		NativeMethod	*method_ptr, *super_method;

		// Allocate a new method
		method_ptr = new NativeMethod;

		// Get the current super-class method
		super_method = super_class->m_MethodList(x);

		// Copy the data
		method_ptr->SetName(super_method->GetName());
		method_ptr->m_Address = super_method->m_Address;
		method_ptr->m_Index = super_method->m_Index;

		// Add the method to the lists
		m_MethodList.Add(method_ptr);
		m_MethodHash.Add(method_ptr->GetName(), method_ptr);
	}
}


int NativeClass::GetMethodIndex(char *name)
{
	NativeMethod	*method_ptr;

	// Return the method index if it exists
	if (method_ptr = (NativeMethod *)m_MethodHash.GetEntry(name))
		return (method_ptr->m_Index);

	// Not found
	return (-1);
}


NativeMethod *NativeClass::GetMethod(char *name)
{
	NativeMethod	*method_ptr;

	// Get the method address
	if ((method_ptr = (NativeMethod *)m_MethodHash.GetEntry(name)) == NULL)
		return (NULL);

	return (method_ptr);
}