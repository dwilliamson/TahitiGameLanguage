
#include "..\include\main.h"


VMImport::VMImport(CFile *file)
{
	char	string[256];
	int		x;

	// Read the function name
	file->ReadStringZ(string);
	m_Name = strnew(string);

	// Read the function index
	file->Read(&m_Index, sizeof(int));

	// Read the location of the function (which is garbage)
	file->Read(&x, sizeof(int));

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

	// Read the parameter list
	for (x = 0; x < m_NbParams; x++)
	{
		VMVariable	*var;
		int		value;

		// Create the variable
		var = new VMVariable;

		// Variable Type ID
		file->Read(&value, sizeof(int));
		var->SetTypeID(value);

		// Variable type name if it's custom
		if (value == VARIABLE_TYPEID_CUSTOM)
		{
			file->ReadStringZ(string);
			var->SetTypeName(string);
		}

		// Array information
		if (value = file->ReadByte())
		{
			file->Read(&value, sizeof(int));
			var->SetArrayElements(value);
		}
	}
}


char *VMImport::GetName(void)
{
	return (m_Name);
}


int VMImport::GetNbParams(void)
{
	return (m_NbParams);
}


int VMImport::GetReturnType(void)
{
	return (m_RetTypeID);
}