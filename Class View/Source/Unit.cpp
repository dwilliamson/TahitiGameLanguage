
#include "..\include\main.h"


Unit::Unit(char *filename)
{
	CFile	*file;
	int		x;
	Class	*cur_class;

	// Copy the filename
	strcpy(m_Name, filename);
	strcpy(m_Location, filename);

	// Obtain the unit filename
	strcpy(m_Name, strrchr(m_Name, '\\') + 1);
	*strchr(m_Name, '.') = 0;
	_strlwr(m_Name);

	// Open the unit file
	file = new CFile(filename, FILEOPEN_READ);

	// Position the file pointer where the class information is
	file->SeekTo(-4, FILESEEK_END);
	file->Read(&m_CodeSize, sizeof(int));
	file->SeekTo(m_CodeSize, FILESEEK_START);

	// Read the number of classes
	file->Read(&m_NbClasses, sizeof(int));

	for (x = 0; x < m_NbClasses; x++)
	{
		Class	*class_ptr;

		// Get the current class
		class_ptr = new Class(file);

		// Add the class to the list
		m_ClassList.Add(class_ptr->m_Name, class_ptr);
	}

	// Close the unit file
	delete file;

	// Enumerate all the classes in the unit
	while (cur_class = (Class *)m_ClassList.Enumerate())
	{
		// Does the class inherit?
		if (cur_class->m_Inherits)
		{
			// Get a pointer to the super-class
			cur_class->m_SClassPtr = (Class *)m_ClassList.GetEntry(cur_class->m_SuperClass);

			// Add this class as a child of the super-class
			cur_class->m_SClassPtr->m_Children.Add(cur_class);
		}

		// No
		else
		{
			// Must be a base class
			m_BaseClasses.Add(cur_class);
		}
	}
}


Unit::~Unit(void)
{
	Class	*class_ptr;

	// Delete all the classes
	while (class_ptr = (Class *)m_ClassList.Enumerate())
	{
		m_ClassList.Remove(class_ptr);
		delete class_ptr;
	}
}


void Unit::AddToTreeView(void)
{
	int		x;

	// Add this unit to the treeview
	hTreeItem = g_Tree->AddUnit(m_Name, 0, this);

	// For every base class
	for (x = 0; x < m_BaseClasses.GetPosition(); x++)
		m_BaseClasses(x)->AddToTreeView(1);
}


void Unit::RemoveFromTreeView(void)
{
	// Remove me
	g_Tree->RemoveItem(hTreeItem);
}


void Unit::PrintInfo(void)
{	
	// Type
	g_Edit->Printf("UNIT\n");
	g_Edit->Printf("\n");

	// Name info
	g_Edit->Printf("Name: %s\n", m_Name);
	g_Edit->Printf("Location: %s\n", m_Location);
	g_Edit->Printf("\n");

	// Misc.
	g_Edit->Printf("Number of classes: %d\n", m_NbClasses);
	g_Edit->Printf("Code size: %d\n", m_CodeSize);
}