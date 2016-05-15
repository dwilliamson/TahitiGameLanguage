
#include "..\include\main.h"


VMVariable::VMVariable(void)
{
	m_Name = NULL;
	m_TypeName = NULL;
	m_Address = NULL;
	m_Array = 0;
	m_NbElements = 0;
}


VMVariable::VMVariable(CFile *file)
{
	char	string[256];

	// Read the variable name
	file->ReadStringZ(string);
	m_Name = strnew(string);

	// Read the address of the variable
	file->Read(&m_Address, sizeof(int));

	// Read the variable type ID
	file->Read(&m_TypeID, sizeof(int));

	// Get the type name if it's custom
	if (m_TypeID == VARIABLE_TYPEID_CUSTOM)
	{
		file->ReadStringZ(string);
		m_TypeName = strnew(string);
	}
	else
	{
		m_TypeName = NULL;
	}

	// If the variable is an array
	m_Array = file->ReadByte();

	// Number of elements in the array
	if (m_Array)
		file->Read(&m_NbElements, sizeof(int));
	else
		m_NbElements = 0;
}


VMVariable::~VMVariable(void)
{
	// Allocated name
	if (m_Name)
		delete [] m_Name;

	// Allocated type name
	if (m_TypeName)
		delete [] m_TypeName;
}


void VMVariable::SetTypeID(int type_id)
{
	m_TypeID = type_id;
}


void VMVariable::SetTypeName(char *type_name)
{
	m_TypeName = strnew(type_name);
}


void VMVariable::SetArrayElements(int nb_elements)
{
	m_Array = 1;
	m_NbElements = nb_elements;
}