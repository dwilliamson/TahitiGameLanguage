
#include "..\include\Common.h"


CCallStack::CCallStack(void)
{
	// Set stuff
	enum_pos = 0;
}


int CCallStack::AddFunction(const char *name)
{
	return (string_list.Add(name));
}


const char *CCallStack::EnumerateFunctions(void)
{
	// Clamp and wrap the pointer return
	if (enum_pos == string_list.GetPosition())
	{
		enum_pos = 0;
		return (NULL);
	}

	return (string_list(enum_pos++));
}