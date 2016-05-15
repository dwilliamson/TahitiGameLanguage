
#include "..\include\main.h"


VMFunction::VMFunction(CFile *file)
{
	char	string[256];
	int		x;

	// Read the function name
	file->ReadStringZ(string);
	m_Name = strnew(string);

	// Read the function index
	file->Read(&m_Index, sizeof(int));

	// Read the location of the function
	file->Read(&m_Location, sizeof(int));

	// Read the return type ID
	file->Read(&m_RetTypeID, sizeof(int));

	// Read the return type name
	if (m_RetTypeID == VARIABLE_TYPEID_CUSTOM)
	{
		file->ReadStringZ(string);
		m_RetTypeName = strnew(string);
	}
	else
	{
		m_RetTypeName = NULL;
	}

	// Read the type array info
	if (m_RetArray = file->ReadByte())
		file->Read(&m_RetElements, sizeof(int));
	else
		m_RetElements = 0;

	// Read the number of passed parameters
	file->Read(&m_NbParams, sizeof(int));

	// Discard the variable list
	for (x = 0; x < m_NbParams; x++)
	{
		int		value;

		file->Read(&value, sizeof(int));

		if (value == VARIABLE_TYPEID_CUSTOM)
			file->ReadStringZ(string);

		if (value = file->ReadByte())
			file->Read(&value, sizeof(int));
	}
}


VMFunction::~VMFunction(void)
{
	// Allocated function name
	if (m_Name)
		delete [] m_Name;

	// Allocated return type name
	if (m_RetTypeName)
		delete [] m_RetTypeName;
}