
#include "..\include\main.h"


VMObject::VMObject(void)
{
	m_Class = NULL;
	m_DataSegment = NULL;
	m_FunctionTable = NULL;
	m_StateTable = NULL;
	m_DataSize = 0;
	m_NbFunctions = 0;
	m_NbStates = 0;
	m_NativeObject = NULL;
	m_NativeClass = NULL;
}


VMObject::~VMObject(void)
{
	// Allocated data segment
	delete [] m_DataSegment;

	// Delete the C++ object
	if (m_NativeObject)
		m_NativeClass->DeleteInstance(m_NativeObject);
}


void VMObject::SetClass(VMClass *class_ptr)
{
	// Register
	m_Class = class_ptr;

	// Setup the object data segment from the class
	SetDataSegment(m_Class->GetDataSegment(), m_Class->GetDataSize());
	SetFunctionTable(m_Class->GetFunctionTable(), m_Class->GetNbFunctions());
	SetStateTable(m_Class->GetStateTable(), m_Class->GetNbStates());
	SetImportTable(m_Class->GetImportTable(), m_Class->GetNbImports());
}


void VMObject::SetDataSegment(char *data_segment, int limit)
{
	// Setup the data segment
	m_DataSize = limit;
	m_DataSegment = new char[limit];

	// Copy the data
	memcpy(m_DataSegment, data_segment, limit);
}


void VMObject::SetFunctionTable(int *function_table, int nb_functions)
{
	// The function table is static so it doesn't need to be allocated
	m_NbFunctions = nb_functions;
	m_FunctionTable = function_table;
}


void VMObject::SetStateTable(int *state_table, int nb_states)
{
	// The state table is static so it doesn't need to be allocated
	m_NbStates = nb_states;
	m_StateTable = state_table;
}


void VMObject::SetImportTable(int *import_table, int nb_imports)
{
	// The import table is static so it doesn't need to be allocated
	m_NbImports = nb_imports;
	m_ImportTable = import_table;
}


void VMObject::SetThread(VMThread *thread_ptr)
{
	// Register
	m_Thread = thread_ptr;

	// Set the thread data segment pointers
	m_Thread->SetDataSegment(m_DataSegment, m_DataSize);
	m_Thread->SetFunctionTable(m_FunctionTable, m_NbFunctions);
	m_Thread->SetStateTable(m_StateTable, m_NbStates);
	m_Thread->SetImportTable(m_ImportTable, m_NbImports);

	// Allocate the thread stack segment
	m_Thread->SetStackSegment(1000);

	// Set the thread entry point
	m_Thread->SetEntryPoint(m_Class->GetEntryPoint());
}


char *VMObject::GetDataSegment(void)
{
	return (m_DataSegment);
}


int VMObject::GetDataSize(void)
{
	return (m_DataSize);
}


int VMObject::GetNbFunctions(void)
{
	return (m_NbFunctions);
}


int VMObject::GetNbStates(void)
{
	return (m_NbStates);
}


void VMObject::SetNative(NativeClass *native)
{
	// If there is no native C++ class, nothing to do
	if (native == NULL)
		return;

	// Allocate the C++ instance
	m_NativeObject = native->NewInstance();
	m_NativeClass = native;
}


void *VMObject::GetNativeObject(void)
{
	return (m_NativeObject);
}