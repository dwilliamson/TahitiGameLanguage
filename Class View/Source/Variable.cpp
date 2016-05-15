
#include "..\include\main.h"


Variable::Variable(void)
{
}


Variable::Variable(CFile *file)
{
	// Read the variable name
	file->ReadStringZ(m_Name);

	// Read the address of the variable
	file->Read(&m_Address, sizeof(int));

	// Read the variable type ID
	file->Read(&m_TypeID, sizeof(int));

	// Get the type name if it's custom
	if (m_TypeID == VARIABLE_TYPEID_CUSTOM)
		file->ReadStringZ(m_TypeName);

	// If the variable is an array
	m_Array = file->ReadByte();

	// Number of elements in the array
	if (m_Array)
		file->Read(&m_NbElements, sizeof(int));
	else
		m_NbElements = 0;
}


Variable::~Variable(void)
{
}


void Variable::PrintInfo(void)
{
	// Differentiate between an object reference and a default variable
	if (m_TypeID == VARIABLE_TYPEID_CUSTOM)
	{
		// Type
		g_Edit->Printf("OBJECT REFERENCE\n");
		g_Edit->Printf("\n");

		// Name info
		g_Edit->Printf("Name: %s\n", m_Name);
		g_Edit->Printf("\n");

		// The rest
		g_Edit->Printf("Address: %d\n", m_Address);
		g_Edit->Printf("Type ID: ");
		PRINT_VALUE(m_TypeID, VARIABLE_TYPEID_CUSTOM);
		g_Edit->Printf("\n");
		if (m_TypeID == VARIABLE_TYPEID_CUSTOM)
			g_Edit->Printf("Type name: %s\n", m_TypeName);
		if (m_Array)
			g_Edit->Printf("Type array elements: %d\n", m_NbElements);
		g_Edit->Printf("\n");
	}

	else
	{
		// Type
		g_Edit->Printf("VARIABLE\n");
		g_Edit->Printf("\n");

		// Name info
		g_Edit->Printf("Name: %s\n", m_Name);
		g_Edit->Printf("\n");

		// The rest
		g_Edit->Printf("Address: %d\n", m_Address);
		g_Edit->Printf("Type ID: ");
		PRINT_VALUE(m_TypeID, VARIABLE_TYPEID_VOID);
		PRINT_VALUE(m_TypeID, VARIABLE_TYPEID_CHAR);
		PRINT_VALUE(m_TypeID, VARIABLE_TYPEID_SHORT);
		PRINT_VALUE(m_TypeID, VARIABLE_TYPEID_INT);
		PRINT_VALUE(m_TypeID, VARIABLE_TYPEID_FLOAT);
		PRINT_VALUE(m_TypeID, VARIABLE_TYPEID_UCHAR);
		PRINT_VALUE(m_TypeID, VARIABLE_TYPEID_USHORT);
		PRINT_VALUE(m_TypeID, VARIABLE_TYPEID_UINT);
		PRINT_VALUE(m_TypeID, VARIABLE_TYPEID_CUSTOM);
		PRINT_VALUE(m_TypeID, VARIABLE_TYPEID_STRING);
		g_Edit->Printf("\n");
		if (m_Array)
			g_Edit->Printf("Type array elements: %d\n", m_NbElements);
		g_Edit->Printf("\n");
	}
}