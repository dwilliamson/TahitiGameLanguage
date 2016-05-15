
#ifndef	_INCLUDED_VMTHREAD_H
#define	_INCLUDED_VMTHREAD_H


#define	THREAD_FLAGS_ACTIVE		0x0001


class VMThread
{
public:
	// Constructor/Destructor
	VMThread(void);
	~VMThread(void);

	// --- Initialisation -------------------------------------------------

	// Set the threads code segment
	void	SetCodeSegment(char *code_segment, int limit);

	// Set the threads data segment
	void	SetDataSegment(char *data_segment, int limit);

	// Set the threads virtual function table
	void	SetFunctionTable(int *function_table, int nb_functions);

	// Set the threads virtual state table
	void	SetStateTable(int *state_table, int nb_states);

	// Set the threads import table
	void	SetImportTable(int *import_table, int nb_imports);

	// Setup the threads stack segment
	void	SetStackSegment(int size);

	// Start the thread at the entry point
	void	SetEntryPoint(int index);

	// Set the object that controls this thread
	void	SetObject(VMObject *object);

	// Set the native C++ object
	void	SetNativeObject(void *object);

	// --------------------------------------------------------------------

	// Step a given number of instructions
	void	StepInstructions(int nb_instructions);

	// Pointers to other threads in the linked list
	VMThread	*next, *prev;

private:
	// Pointer to the native object
	void	*m_NativeObject;

	// Pointer to the thread code segment
	char	*m_CodeSegment;

	// Size of the code segment
	int		m_CodeSize;

	// Pointer to the thread data segment
	char	*m_DataSegment;

	// Size of the data segment
	int		m_DataSize;

	// Stack for the current thread
	void	*m_Stack;

	// Size of the stack
	int		m_StackSize;

	// Pointer to the virtual function table
	int		*m_FunctionTable;

	// Number of functions in the table
	int		m_NbFunctions;

	// Pointer to the virtual state table
	int		*m_StateTable;

	// Number of states in the table
	int		m_NbStates;

	// Function import table
	int		*m_ImportTable;

	// Number of imported functions
	int		m_NbImports;

	// Pointer to the object which controls this thread
	VMObject	*m_Object;

	// Instruction pointer
	uDWORD	m_regIP;

	// Stack pointer
	uDWORD	m_regSP;

	// Function stack frame stack pointer
	uDWORD	m_regBP;

	// Return value
	uDWORD	m_regRV;

	// Thread flags
	int		m_Flags;

	/* ---------- ---------- */

	// Array of instruction function pointers
	static void	(CALLTYPE *m_InstrFunc[MAX_NB_OPCODES])(VMThread *thread_ptr);

	// All the operator functions
	static void	CALLTYPE Opcode_Nothing(VMThread *thread_ptr);
	static void	CALLTYPE Opcode_PushImmediate(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FPushImmediate(VMThread *thread_ptr);
	static void	CALLTYPE Opcode_PushMemberValue(VMThread *thread_ptr);
	static void CALLTYPE Opcode_PushMemberAddr(VMThread *thread_ptr);
	static void CALLTYPE Opcode_PushLocalValue(VMThread *thread_ptr);
	static void CALLTYPE Opcode_PushLocalAddr(VMThread *thread_ptr);
	static void CALLTYPE Opcode_PushReturn(VMThread *thread_ptr);
	static void CALLTYPE Opcode_PopReturn(VMThread *thread_ptr);
	static void CALLTYPE Opcode_Pop(VMThread *thread_ptr);
	static void CALLTYPE Opcode_Add(VMThread *thread_ptr);
	static void CALLTYPE Opcode_Sub(VMThread *thread_ptr);
	static void CALLTYPE Opcode_Mul(VMThread *thread_ptr);
	static void CALLTYPE Opcode_Div(VMThread *thread_ptr);
	static void CALLTYPE Opcode_Mod(VMThread *thread_ptr);
	static void CALLTYPE Opcode_And(VMThread *thread_ptr);
	static void CALLTYPE Opcode_Or(VMThread *thread_ptr);
	static void CALLTYPE Opcode_Xor(VMThread *thread_ptr);
	static void CALLTYPE Opcode_Shl(VMThread *thread_ptr);
	static void CALLTYPE Opcode_Shr(VMThread *thread_ptr);
	static void CALLTYPE Opcode_Not(VMThread *thread_ptr);
	static void CALLTYPE Opcode_Neg(VMThread *thread_ptr);
	static void CALLTYPE Opcode_Inc(VMThread *thread_ptr);
	static void CALLTYPE Opcode_Dec(VMThread *thread_ptr);
	static void CALLTYPE Opcode_SetEqual(VMThread *thread_ptr);
	static void CALLTYPE Opcode_SetNotEqual(VMThread *thread_ptr);
	static void CALLTYPE Opcode_SetLess(VMThread *thread_ptr);
	static void CALLTYPE Opcode_SetLessEqual(VMThread *thread_ptr);
	static void CALLTYPE Opcode_SetGreater(VMThread *thread_ptr);
	static void CALLTYPE Opcode_SetGreaterEqual(VMThread *thread_ptr);
	static void CALLTYPE Opcode_SetEqualZero(VMThread *thread_ptr);
	static void CALLTYPE Opcode_Jump(VMThread *thread_ptr);
	static void CALLTYPE Opcode_JumpZero(VMThread *thread_ptr);
	static void CALLTYPE Opcode_JumpNotZero(VMThread *thread_ptr);
	static void CALLTYPE Opcode_JumpGreater(VMThread *thread_ptr);
	static void CALLTYPE Opcode_JumpGreaterEqual(VMThread *thread_ptr);
	static void CALLTYPE Opcode_JumpLess(VMThread *thread_ptr);
	static void CALLTYPE Opcode_JumpLessEqual(VMThread *thread_ptr);
	static void CALLTYPE Opcode_JumpRelative(VMThread *thread_ptr);
	static void CALLTYPE Opcode_Assign(VMThread *thread_ptr);
	static void CALLTYPE Opcode_AssignPush(VMThread *thread_ptr);
	static void CALLTYPE Opcode_AssignReplace(VMThread *thread_ptr);
	static void CALLTYPE Opcode_InStackFrame(VMThread *thread_ptr);
	static void CALLTYPE Opcode_OutStackFrame(VMThread *thread_ptr);
	static void CALLTYPE Opcode_AddStack(VMThread *thread_ptr);
	static void CALLTYPE Opcode_SubStack(VMThread *thread_ptr);
	static void CALLTYPE Opcode_Return(VMThread *thread_ptr);
	static void CALLTYPE Opcode_RPV(VMThread *thread_ptr);
	static void CALLTYPE Opcode_DecReplace(VMThread *thread_ptr);
	static void CALLTYPE Opcode_IncReplace(VMThread *thread_ptr);
	static void CALLTYPE Opcode_DecPop(VMThread *thread_ptr);
	static void CALLTYPE Opcode_IncPop(VMThread *thread_ptr);
	static void CALLTYPE Opcode_DecMove(VMThread *thread_ptr);
	static void CALLTYPE Opcode_IncMove(VMThread *thread_ptr);
	static void CALLTYPE Opcode_Call(VMThread *thread_ptr);
	static void CALLTYPE Opcode_CallVirtual(VMThread *thread_ptr);
	static void CALLTYPE Opcode_ArrayProcess(VMThread *thread_ptr);
	static void CALLTYPE Opcode_ArrayIndex(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FloatToInt(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FloatToShort(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FloatToChar(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FloatToUShort(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FloatToUChar(VMThread *thread_ptr);
	static void CALLTYPE Opcode_IntToFloat(VMThread *thread_ptr);
	static void CALLTYPE Opcode_IntToShort(VMThread *thread_ptr);
	static void CALLTYPE Opcode_IntToChar(VMThread *thread_ptr);
	static void CALLTYPE Opcode_IntToUShort(VMThread *thread_ptr);
	static void CALLTYPE Opcode_IntToUChar(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FAdd(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FSub(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FMul(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FDiv(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FMod(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FNeg(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FSetEqual(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FSetNotEqual(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FSetLess(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FSetLessEqual(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FSetGreater(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FSetGreaterEqual(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FSetEqualZero(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FJumpZero(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FJumpNotZero(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FJumpGreater(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FJumpGreaterEqual(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FJumpLess(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FJumpLessEqual(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FDecReplace(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FIncReplace(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FDecPop(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FIncPop(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FDecMove(VMThread *thread_ptr);
	static void CALLTYPE Opcode_FIncMove(VMThread *thread_ptr);
	static void CALLTYPE Opcode_PushMemberArray(VMThread *thread_ptr);
	static void CALLTYPE Opcode_PushLocalArray(VMThread *thread_ptr);
	static void CALLTYPE Opcode_SetState(VMThread *thread_ptr);
	static void CALLTYPE Opcode_EndState(VMThread *thread_ptr);
	static void CALLTYPE Opcode_CallImport(VMThread *thread_ptr);
};


#endif	/* _INCLUDED_VMTHREAD_H */