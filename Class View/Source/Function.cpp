
#include "..\include\main.h"


Function::Function(CFile *file)
{
	int		x;

	// Read the function name
	file->ReadStringZ(m_Name);

	// Read the function index
	file->Read(&m_Index, sizeof(int));

	// Read the location of the function
	file->Read(&m_Location, sizeof(int));

	// Read the return type ID
	file->Read(&m_RetTypeID, sizeof(int));

	// Read the return type name if it's custom
	if (m_RetTypeID == VARIABLE_TYPEID_CUSTOM)
		file->ReadStringZ(m_RetTypeName);

	// Read the type array info
	if (m_RetArray = file->ReadByte())
		file->Read(&m_RetElements, sizeof(int));
	else
		m_RetElements = 0;

	// Read the number of passed parameters
	file->Read(&m_NbParams, sizeof(int));

	for (x = 0; x < m_NbParams; x++)
	{
		Variable	*var;

		// Allocate this variable
		var = new Variable;

		// Read the type ID
		file->Read(&var->m_TypeID, sizeof(int));

		// Read the type string if it's custom
		if (var->m_TypeID == VARIABLE_TYPEID_CUSTOM)
			file->ReadStringZ(var->m_TypeName);

		// Read the array information
		if (var->m_Array = file->ReadByte())
			file->Read(&var->m_NbElements, sizeof(int));
		else
			var->m_NbElements = 0;

		// Add it to the list
		m_ParamList.Add(var);
	}
}


Function::~Function(void)
{
	int			x;

	// Delete all the function parameters
	for (x = 0; x < m_ParamList.GetPosition(); x++)
		delete m_ParamList(x);
}


void Function::PrintInfo(void)
{
	int		x;

	// Type
	if (m_Import) g_Edit->Printf("IMPORTED FUNCTION\n");
	else g_Edit->Printf("FUNCTION\n");
	g_Edit->Printf("\n");

	// Name info
	g_Edit->Printf("Name: %s\n", m_Name);
	g_Edit->Printf("\n");

	// Compiled info
	g_Edit->Printf("Index within class: %d\n", m_Index);
	if (!m_Import)
		g_Edit->Printf("Location: %d\n", m_Location);
	g_Edit->Printf("\n");

	// Return type info
	g_Edit->Printf("Return type ID: ");
	PRINT_VALUE(m_RetTypeID, VARIABLE_TYPEID_VOID);
	PRINT_VALUE(m_RetTypeID, VARIABLE_TYPEID_CHAR);
	PRINT_VALUE(m_RetTypeID, VARIABLE_TYPEID_SHORT);
	PRINT_VALUE(m_RetTypeID, VARIABLE_TYPEID_INT);
	PRINT_VALUE(m_RetTypeID, VARIABLE_TYPEID_FLOAT);
	PRINT_VALUE(m_RetTypeID, VARIABLE_TYPEID_UCHAR);
	PRINT_VALUE(m_RetTypeID, VARIABLE_TYPEID_USHORT);
	PRINT_VALUE(m_RetTypeID, VARIABLE_TYPEID_UINT);
	PRINT_VALUE(m_RetTypeID, VARIABLE_TYPEID_CUSTOM);
	g_Edit->Printf("\n");
	if (m_RetTypeID == VARIABLE_TYPEID_CUSTOM)
		g_Edit->Printf("Return type name: %s\n", m_RetTypeName);
	if (m_RetArray)
		g_Edit->Printf("Return type array elements: %d\n", m_RetElements);
	g_Edit->Printf("\n");

	// Parameter info
	g_Edit->Printf("Number of parameters: %d\n", m_NbParams);
	for (x = 0; x < m_NbParams; x++)
	{
		// Open block
		g_Edit->Printf("{");

		// Type info
		PRINT_VALUE(m_ParamList(x)->m_TypeID, VARIABLE_TYPEID_VOID);
		PRINT_VALUE(m_ParamList(x)->m_TypeID, VARIABLE_TYPEID_CHAR);
		PRINT_VALUE(m_ParamList(x)->m_TypeID, VARIABLE_TYPEID_SHORT);
		PRINT_VALUE(m_ParamList(x)->m_TypeID, VARIABLE_TYPEID_INT);
		PRINT_VALUE(m_ParamList(x)->m_TypeID, VARIABLE_TYPEID_FLOAT);
		PRINT_VALUE(m_ParamList(x)->m_TypeID, VARIABLE_TYPEID_UCHAR);
		PRINT_VALUE(m_ParamList(x)->m_TypeID, VARIABLE_TYPEID_USHORT);
		PRINT_VALUE(m_ParamList(x)->m_TypeID, VARIABLE_TYPEID_UINT);
		PRINT_VALUE(m_ParamList(x)->m_TypeID, VARIABLE_TYPEID_CUSTOM);
		if (m_ParamList(x)->m_TypeID == VARIABLE_TYPEID_CUSTOM)
		{
			g_Edit->Printf(",");
			g_Edit->Printf("Return type name: %s\n", m_ParamList(x)->m_TypeName);
		}
		if (m_ParamList(x)->m_Array)
		{
			g_Edit->Printf(",");
			g_Edit->Printf("Return type array elements: %d\n", m_ParamList(x)->m_NbElements);
		}

		// Close block
		g_Edit->Printf("}\n");
	}
	g_Edit->Printf("\n");
}