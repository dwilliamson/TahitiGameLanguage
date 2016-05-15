
#include "..\include\main.h"


VMUnit::VMUnit(void)
{
	m_Name = NULL;
	m_CodeSegment = NULL;
	m_CodeSize = 0;
}


VMUnit::~VMUnit(void)
{
	VMClass		*class_ptr;
	VMThread	*thread_ptr;
	VMObject	*object_ptr;

	// Allocated name
	if (m_Name)
		delete [] m_Name;

	// Allocated code segment
	if (m_CodeSegment)
		delete [] m_CodeSegment;

	// Delete all the active threads
	while (thread_ptr = m_ThreadList.GetFirst())
	{
		m_ThreadList.Remove(thread_ptr);
		delete thread_ptr;
	}

	// Delete all the class instances
	while (object_ptr = m_ObjectList.GetFirst())
	{
		m_ObjectList.Remove(object_ptr);
		delete object_ptr;
	}

	// Delete all the classes
	while (class_ptr = m_ClassList.GetFirst())
	{
		m_ClassList.Remove(class_ptr);
		delete class_ptr;
	}
}


void VMUnit::Load(const char *filename)
{
	CFile	*file;
	int		x, nb_classes;
	char	string[256];
	VMClass	*class_ptr;

	// Open the unit file
	if ((file = new CFile(filename, FILEOPEN_READ)) == NULL)
		return;

	// Get the code segment size
	file->SeekTo(-4, FILESEEK_END);
	file->Read(&m_CodeSize, sizeof(int));
	file->SeekTo(0, FILESEEK_START);

	// Allocate the code segment
	if ((m_CodeSegment = new char[m_CodeSize]) == NULL)
		return;

	// Read in the code segment
	file->Read(m_CodeSegment, m_CodeSize);

	// Get the number of classes
	file->Read(&nb_classes, sizeof(int));

	for (x = 0; x < nb_classes; x++)
	{
		// Get the class information
		if ((class_ptr = new VMClass(file)) == NULL)
			return;

		class_ptr->SetUnit(this);

		// Add it to the lists
		m_ClassList.AddLast(class_ptr);
		m_ClassHash.Add(class_ptr->GetName(), class_ptr);
	}

	// Close the unit file
	delete file;

	// Classes are loaded, build the import tables
	for (class_ptr = m_ClassList.GetFirst(); class_ptr; class_ptr = class_ptr->next)
		class_ptr->BuildImportTable();

	// Create the unit name
	strcpy(string, filename);
	*strchr(string, '.') = 0;
	_strlwr(string);
	m_Name = strnew(string);
}


char *VMUnit::GetName(void)
{
	return (m_Name);
}


VMObject *VMUnit::NewObject(const char *name, VirtualMachine *machine)
{
	VMClass		*class_ptr;
	VMObject	*object_ptr;
	VMThread	*thread_ptr;
	NativeClass	*native_ptr;
	char		class_name[256];

	// Get the class pointer
	if ((class_ptr = (VMClass *)m_ClassHash.GetEntry(name)) == NULL)
		return (NULL);

	// Abstract classes cannot be instantiated
	if (class_ptr->GetFlags() & CLASS_FLAGS_ABSTRACT)
		return (NULL);

	// Create the thread and attach the unit code segment
	thread_ptr = new VMThread;
	thread_ptr->SetCodeSegment(m_CodeSegment, m_CodeSize);

	// Create the object and attach the class and thread
	object_ptr = new VMObject;
	object_ptr->SetClass(class_ptr);
	object_ptr->SetThread(thread_ptr);

	// Keep track of the thread and object
	m_ThreadList.AddLast(thread_ptr);
	m_ObjectList.AddLast(object_ptr);

	// Set the associated native class
	strcpy(class_name, name);
	while ((native_ptr = (NativeClass *)VirtualMachine::GetInstance()->m_NativeClasses.GetEntry(class_name)) == NULL)
	{
		class_ptr = GetClass(class_name);
		if (!class_ptr->m_Inherits) break;							// IMPORT NOT FOUND!
		strcpy(class_name, class_ptr->m_SuperClass);
	}

	// Set the native class
	object_ptr->SetNative(native_ptr);
	thread_ptr->SetNativeObject(object_ptr->GetNativeObject());

	return (object_ptr);
}


void VMUnit::ProcessFrame(int nb_instructions)
{
	VMThread	*thread_ptr;

	// Step each thread
	for (thread_ptr = m_ThreadList.GetFirst(); thread_ptr; thread_ptr = thread_ptr->next)
		thread_ptr->StepInstructions(nb_instructions);
}


VMClass *VMUnit::GetClass(char *name)
{
	return ((VMClass *)m_ClassHash.GetEntry(name));
}