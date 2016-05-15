
#include "..\include\main.h"


// Variables for floating point optimisation
static float	f_Temp;
static int		i_Temp;


/* ----- Opcode Jump Table ---------------------------------------------------------------*/


void (CALLTYPE *VMThread::m_InstrFunc[MAX_NB_OPCODES])(VMThread *) =
{
	VMThread::Opcode_Nothing,
	VMThread::Opcode_PushImmediate,
	VMThread::Opcode_FPushImmediate,
	VMThread::Opcode_PushMemberValue,
	VMThread::Opcode_PushMemberAddr,
	VMThread::Opcode_PushLocalValue,
	VMThread::Opcode_PushLocalAddr,
	VMThread::Opcode_PushReturn,
	VMThread::Opcode_PopReturn,
	VMThread::Opcode_Pop,
	VMThread::Opcode_Add,
	VMThread::Opcode_Sub,
	VMThread::Opcode_Mul,
	VMThread::Opcode_Div,
	VMThread::Opcode_Mod,
	VMThread::Opcode_And,
	VMThread::Opcode_Or,
	VMThread::Opcode_Xor,
	VMThread::Opcode_Shl,
	VMThread::Opcode_Shr,
	VMThread::Opcode_Not,
	VMThread::Opcode_Neg,
	VMThread::Opcode_Inc,
	VMThread::Opcode_Dec,
	VMThread::Opcode_SetEqual,
	VMThread::Opcode_SetNotEqual,
	VMThread::Opcode_SetLess,
	VMThread::Opcode_SetLessEqual,
	VMThread::Opcode_SetGreater,
	VMThread::Opcode_SetGreaterEqual,
	VMThread::Opcode_SetEqualZero,
	VMThread::Opcode_Jump,
	VMThread::Opcode_JumpZero,
	VMThread::Opcode_JumpNotZero,
	VMThread::Opcode_JumpGreater,
	VMThread::Opcode_JumpGreaterEqual,
	VMThread::Opcode_JumpLess,
	VMThread::Opcode_JumpLessEqual,
	VMThread::Opcode_JumpRelative,
	VMThread::Opcode_Assign,
	VMThread::Opcode_AssignPush,
	VMThread::Opcode_AssignReplace,
	VMThread::Opcode_InStackFrame,
	VMThread::Opcode_OutStackFrame,
	VMThread::Opcode_AddStack,
	VMThread::Opcode_SubStack,
	VMThread::Opcode_Return,
	VMThread::Opcode_RPV,
	VMThread::Opcode_DecReplace,
	VMThread::Opcode_IncReplace,
	VMThread::Opcode_DecPop,
	VMThread::Opcode_IncPop,
	VMThread::Opcode_DecMove,
	VMThread::Opcode_IncMove,
	VMThread::Opcode_Call,
	VMThread::Opcode_CallVirtual,
	VMThread::Opcode_ArrayProcess,
	VMThread::Opcode_ArrayIndex,
	VMThread::Opcode_FloatToInt,
	VMThread::Opcode_FloatToShort,
	VMThread::Opcode_FloatToChar,
	VMThread::Opcode_FloatToUShort,
	VMThread::Opcode_FloatToUChar,
	VMThread::Opcode_IntToFloat,
	VMThread::Opcode_IntToShort,
	VMThread::Opcode_IntToChar,
	VMThread::Opcode_IntToUShort,
	VMThread::Opcode_IntToUChar,
	VMThread::Opcode_FAdd,
	VMThread::Opcode_FSub,
	VMThread::Opcode_FMul,
	VMThread::Opcode_FDiv,
	VMThread::Opcode_FMod,
	VMThread::Opcode_FNeg,
	VMThread::Opcode_FSetEqual,
	VMThread::Opcode_FSetNotEqual,
	VMThread::Opcode_FSetLess,
	VMThread::Opcode_FSetLessEqual,
	VMThread::Opcode_FSetGreater,
	VMThread::Opcode_FSetGreaterEqual,
	VMThread::Opcode_FSetEqualZero,
	VMThread::Opcode_FJumpZero,
	VMThread::Opcode_FJumpNotZero,
	VMThread::Opcode_FJumpGreater,
	VMThread::Opcode_FJumpGreaterEqual,
	VMThread::Opcode_FJumpLess,
	VMThread::Opcode_FJumpLessEqual,
	VMThread::Opcode_FDecReplace,
	VMThread::Opcode_FIncReplace,
	VMThread::Opcode_FDecPop,
	VMThread::Opcode_FIncPop,
	VMThread::Opcode_FDecMove,
	VMThread::Opcode_FIncMove,
	VMThread::Opcode_PushMemberArray,
	VMThread::Opcode_PushLocalArray,
	VMThread::Opcode_SetState,
	VMThread::Opcode_EndState,
	VMThread::Opcode_CallImport
};


VMThread::VMThread(void)
{
	// Initialise
	m_CodeSegment = NULL;
	m_DataSegment = NULL;
	m_Stack = NULL;
	m_FunctionTable = NULL;
	m_StateTable = NULL;
	m_CodeSize = 0;
	m_DataSize = 0;
	m_StackSize = 0;
	m_NbFunctions = 0;
	m_NbStates = 0;
	m_Object = NULL;
	m_regIP = m_regSP = m_regBP = m_regRV = 0;
	m_Flags = THREAD_FLAGS_ACTIVE;
}


VMThread::~VMThread(void)
{
	// Allocated stack
	delete [] m_Stack;
}


void VMThread::SetCodeSegment(char *code_segment, int limit)
{
	m_CodeSegment = code_segment;
	m_CodeSize = limit;
}


void VMThread::SetDataSegment(char *data_segment, int limit)
{
	m_DataSegment = data_segment;
	m_DataSize = limit;
}


void VMThread::SetFunctionTable(int *function_table, int nb_functions)
{
	m_FunctionTable = function_table;
	m_NbFunctions = nb_functions;
}


void VMThread::SetStateTable(int *state_table, int nb_states)
{
	m_StateTable = state_table;
	m_NbStates = nb_states;
}


void VMThread::SetImportTable(int *import_table, int nb_imports)
{
	m_ImportTable = import_table;
	m_NbImports = nb_imports;
}


void VMThread::SetStackSegment(int size)
{
	// Allocate the stack
	m_Stack = new char[m_StackSize = size];

	// Set the stack pointer to the end of the stack
	m_regSP = size - 4;
}


void VMThread::SetEntryPoint(int where)
{
	// Position the instruction pointer at the function address
	m_regIP = m_FunctionTable[where];

	// Push -1 for the 'ret' instruction to freeze upon
	STACK_PUSH(-1, int, this);
}


void VMThread::SetObject(VMObject *object)
{
	m_Object = object;
}


void VMThread::SetNativeObject(void *object)
{
	m_NativeObject = object;
}


void VMThread::StepInstructions(int nb_instructions)
{
	// Check for nasties
	if (m_CodeSegment == NULL || m_DataSegment == NULL)
		return;

	// For each instruction to step
	while (nb_instructions)
	{
		uBYTE	instruction;
		
		// Break if the thread isn't active
		if (!(m_Flags & THREAD_FLAGS_ACTIVE))
			break;

		// Get the instruction
		instruction = m_CodeSegment[m_regIP++];

		// Call the appropriate function
		m_InstrFunc[instruction](this);

		nb_instructions--;
	}
}


void CALLTYPE VMThread::Opcode_Nothing(VMThread *thread_ptr)
{
	// ... Do nothing ...
}


void CALLTYPE VMThread::Opcode_PushImmediate(VMThread *thread_ptr)
{
	/* -

	Take an integer directly from the code segment and push it on the stack.

	- */

	int		value;

	READ32(value, int, thread_ptr);
	STACK_PUSH(value, int, thread_ptr);
}


void CALLTYPE VMThread::Opcode_FPushImmediate(VMThread *thread_ptr)
{
	/* -

	Take a float directly from the code segment and push it on the stack.

	- */

	float	value;

	READ32(value, float, thread_ptr);
	STACK_PUSH(value, float, thread_ptr);
}


void CALLTYPE VMThread::Opcode_PushMemberValue(VMThread *thread_ptr)
{
	/* ---

	Read an address from the code, lookup the value at this address and push the value
	back onto the stack.

	(* Need a range check for the data segment *)

	--- */

	int		address, value;

	READ32(address, int, thread_ptr);
	value = *(int *)&(thread_ptr->m_DataSegment[address]);
	STACK_PUSH(value, int, thread_ptr);
}


void CALLTYPE VMThread::Opcode_PushMemberAddr(VMThread *thread_ptr)
{
	/* --

	Read an address from the code and push it directly onto the stack.

	(* Need a range check for the data segment *)

	-- */

	int		address;

	READ32(address, int, thread_ptr);
	STACK_PUSH((int)thread_ptr->m_DataSegment + address, int, thread_ptr);
}


void CALLTYPE VMThread::Opcode_PushLocalValue(VMThread *thread_ptr)
{
	/* ---

	Read the address from the code segment, add this to the BP register and read 32-bits
	from that address (should be within the stack), pushing it on the stack.

	(* Needs a stack range check *)

	--- */

	int		address, value;

	READ32(address, int, thread_ptr);
	//address += thread_ptr->m_regBP;
	address = thread_ptr->m_regBP - address;
	value = *(int *)address;
	STACK_PUSH(value, int, thread_ptr);
}


void CALLTYPE VMThread::Opcode_PushLocalAddr(VMThread *thread_ptr)
{
	/* ---

	Read the address from the code segment, add this to the BP register and push the result
	back onto the stack.

	(* Needs a stack range check *)

	--- */

	int		address;

	READ32(address, int, thread_ptr);
	//address += thread_ptr->m_regBP;
	address = thread_ptr->m_regBP - address;
	STACK_PUSH(address, int, thread_ptr);
}


void CALLTYPE VMThread::Opcode_PushReturn(VMThread *thread_ptr)
{
	/* -

	Push the return register onto the stack.

	- */

	STACK_PUSH(thread_ptr->m_regRV, int, thread_ptr);
}


void CALLTYPE VMThread::Opcode_PopReturn(VMThread *thread_ptr)
{
	/* -

	Pop a value off the stack and into the return register.

	- */

	thread_ptr->m_regRV = STACK_POP(int, thread_ptr);
}


void CALLTYPE VMThread::Opcode_Pop(VMThread *thread_ptr)
{
	/* -

	Pop a value off the stack into nowhere.

	- */

	int		value = STACK_POP(int, thread_ptr);
}


void CALLTYPE VMThread::Opcode_Add(VMThread *thread_ptr)
{
	/* --

	Pop two integer values off the stack and add them together, then push the result back
	onto the stack.

	-- */

	MATH_OP(int, +, thread_ptr);
}


void CALLTYPE VMThread::Opcode_Sub(VMThread *thread_ptr)
{
	/* --

	Pop two integer values off the stack and subtract the first one from the second one,
	then push the result back onto the stack.

	-- */

	MATH_OP_INV(int, -, thread_ptr);
}


void CALLTYPE VMThread::Opcode_Mul(VMThread *thread_ptr)
{
	/* --

	Pop two integer values off the stack and multiply them together, then push the result
	back onto the stack.

	-- */

	MATH_OP(int, *, thread_ptr);
}


void CALLTYPE VMThread::Opcode_Div(VMThread *thread_ptr)
{
	/* --

	Pop two integer values off the stack and divide the second one by the first one,
	then push the result back onto the stack.

	-- */

	MATH_OP_INV(int, /, thread_ptr);
}


void CALLTYPE VMThread::Opcode_Mod(VMThread *thread_ptr)
{
	/* --

	Pop two integer values off the stack and divide the second one by the first one, the
	remainder of the integer divide is then pushed back onto the stack.

	-- */

	MATH_OP_INV(int, %, thread_ptr);
}


void CALLTYPE VMThread::Opcode_And(VMThread *thread_ptr)
{
	/* --

	Pop two integer values off the stack and do a bitwise AND between the two, pushing
	the result back onto the stack.

	-- */

	MATH_OP(int, &, thread_ptr);
}


void CALLTYPE VMThread::Opcode_Or(VMThread *thread_ptr)
{
	/* --

	Pop two integer values off the stack and do a bitwise OR between the two, pushing
	the result back onto the stack.

	-- */

	MATH_OP(int, |, thread_ptr);
}


void CALLTYPE VMThread::Opcode_Xor(VMThread *thread_ptr)
{
	/* --

	Pop two integer values off the stack and do a bitwise XOR between the two, pushing
	the result back onto the stack.

	-- */

	MATH_OP(int, ^, thread_ptr);
}


void CALLTYPE VMThread::Opcode_Shl(VMThread *thread_ptr)
{
	/* --

	Pop two integer values off the stack and shift the second one left by the number of
	bit places stored in the first value, pushing the result back onto the stack.

	-- */

	MATH_OP_INV(int, <<, thread_ptr);
}


void CALLTYPE VMThread::Opcode_Shr(VMThread *thread_ptr)
{
	/* --

	Pop two integer values off the stack and shift the second one right by the number of
	bit places stored in the first value, pushing the result back onto th stack.

	-- */

	MATH_OP_INV(int, >>, thread_ptr);
}


void CALLTYPE VMThread::Opcode_Not(VMThread *thread_ptr)
{
	/* --

	Pop an integer value off the stack, perform a bitwise NOT on it and push the result
	back onto the stack.

	-- */

	int		value;

	value = STACK_POP(int, thread_ptr);
	STACK_PUSH((~value), int, thread_ptr);
}


void CALLTYPE VMThread::Opcode_Neg(VMThread *thread_ptr)
{
	/* --

	Pop an integer value off the stack, invert the sign and push the result back onto
	the stack.

	-- */

	int		value;

	value = STACK_POP(int, thread_ptr);
	STACK_PUSH((-value), int, thread_ptr);
}


void CALLTYPE VMThread::Opcode_Inc(VMThread *thread_ptr)
{
	/* --

	Pop an integer value off the stack, increment it and push the result back onto the
	stack.

	-- */

	int		value;

	value = STACK_POP(int, thread_ptr);
	STACK_PUSH((value + 1), int, thread_ptr);
}


void CALLTYPE VMThread::Opcode_Dec(VMThread *thread_ptr)
{
	/* --

	Pop an integer value off the stack, decrement it and push the result back onto the
	stack.

	-- */

	int		value;

	value = STACK_POP(int, thread_ptr);
	STACK_PUSH((value - 1), int, thread_ptr);
}


void CALLTYPE VMThread::Opcode_SetEqual(VMThread *thread_ptr)
{
	/* --

	Pop two integers off the stack and push true onto the stack if they are equal, push
	false otherwise.

	-- */

	MATH_OP(int, ==, thread_ptr);
}


void CALLTYPE VMThread::Opcode_SetNotEqual(VMThread *thread_ptr)
{
	/* --

	Pop two integers off the stack and push true onto the stack if they are not equal,
	push false otherwise.

	-- */

	MATH_OP(int, !=, thread_ptr);
}


void CALLTYPE VMThread::Opcode_SetLess(VMThread *thread_ptr)
{
	/* --

	Pop two integers off the stack and push true onto the stack if the second is less
	than the first, push false otherwise.

	-- */

	MATH_OP_INV(int, <, thread_ptr);
}


void CALLTYPE VMThread::Opcode_SetLessEqual(VMThread *thread_ptr)
{
	/* --

	Pop two integers off the stack and push true onto the stack if the second is less
	than or equal to the first, push false otherwise.

	-- */

	MATH_OP_INV(int, <=, thread_ptr);
}


void CALLTYPE VMThread::Opcode_SetGreater(VMThread *thread_ptr)
{
	/* --

	Pop two integers off the stack and push true onto the stack if the second is greater
	than the first, push false otherwise.

	-- */

	MATH_OP_INV(int, >, thread_ptr);
}


void CALLTYPE VMThread::Opcode_SetGreaterEqual(VMThread *thread_ptr)
{
	/* --

	Pop two integers off the stack and push true onto the stack if the second is greater
	than or equal to the first, push false otherwise,

	-- */

	MATH_OP_INV(int, >=, thread_ptr);
}


void CALLTYPE VMThread::Opcode_SetEqualZero(VMThread *thread_ptr)
{
	/* --

	Pop an integer off the stack and push true onto the stack if the value is equal to
	zero, push false otherwise.

	-- */

	int		value;

	value = STACK_POP(int, thread_ptr);
	STACK_PUSH((value == 0), int, thread_ptr);
}


void CALLTYPE VMThread::Opcode_Jump(VMThread *thread_ptr)
{
	/* -

	Read an address from the code and jump straight to it.

	- */

	int		address;

	READ32(address, int, thread_ptr);
	thread_ptr->m_regIP = address;
}


void CALLTYPE VMThread::Opcode_JumpZero(VMThread *thread_ptr)
{
	/* --

	Read an address from the code, pop an integer value off the stack and jump to the
	address if the value is equal to zero.

	-- */

	int		value;

	value = STACK_POP(int, thread_ptr);

	if (!value)
	{
		READ32(value, int, thread_ptr);
		thread_ptr->m_regIP = value;
	}
	else
	{
		thread_ptr->m_regIP += 4;
	}
}


void CALLTYPE VMThread::Opcode_JumpNotZero(VMThread *thread_ptr)
{
	/* --

	Read an address from the code, pop an integer value off the stack and jump to the
	address if the value is not equal to zero.

	-- */

	int		value;

	value = STACK_POP(int, thread_ptr);

	if (value)
	{
		READ32(value, int, thread_ptr);
		thread_ptr->m_regIP = value;
	}
	else
	{
		thread_ptr->m_regIP += 4;
	}
}


void CALLTYPE VMThread::Opcode_JumpGreater(VMThread *thread_ptr)
{
	/* --

	Read an address from the code, pop two integers off the stack and jump to the
	address if the second value is greater than the first value.

	-- */

	JUMP_OP_INV(int, >, thread_ptr);
}


void CALLTYPE VMThread::Opcode_JumpGreaterEqual(VMThread *thread_ptr)
{
	/* --

	Read an address from the code, pop two integers off the stack and jump to the
	address if the second value is greater than or equal to the first value.

	-- */

	JUMP_OP_INV(int, >=, thread_ptr);
}


void CALLTYPE VMThread::Opcode_JumpLess(VMThread *thread_ptr)
{
	/* --

	Read an address from the code, pop two integers off the stack and jump to the
	address if the second value is less than the first value.

	-- */

	JUMP_OP_INV(int, <, thread_ptr);
}


void CALLTYPE VMThread::Opcode_JumpLessEqual(VMThread *thread_ptr)
{
	/* --

	Read an address from the code, pop two integers off the stack and jump to the
	address if the second value is less than or equal to the first value.

	-- */

	JUMP_OP_INV(int, <=, thread_ptr);
}


void CALLTYPE VMThread::Opcode_JumpRelative(VMThread *thread_ptr)
{
	/* --

	Read an offset from the code and jump that far from the current position within the
	code.

	-- */

	int		offset;

	READ32(offset, int, thread_ptr);
	thread_ptr->m_regIP += offset;
}


void CALLTYPE VMThread::Opcode_Assign(VMThread *thread_ptr)
{
	/* ----

	Pop a value followed by an address off the stack, write the value to the address
	pointed to. It should not matter if the value is int or float since no value type
	casting is performed (only done on pointers), leading to a direct memory transfer
	of the data.

	---- */

	int		address, value;

	value = STACK_POP(int, thread_ptr);
	address = STACK_POP(int, thread_ptr);
	*(int *)address = value;
}


void CALLTYPE VMThread::Opcode_AssignPush(VMThread *thread_ptr)
{
	/* --

	Pop a value followed by an address off the stack, write the value to the address
	pointed to and push the address back onto the stack.

	-- */

	int		address, value;

	value = STACK_POP(int, thread_ptr);
	address = STACK_POP(int, thread_ptr);
	*(int *)address = value;
	STACK_PUSH(value, int, thread_ptr);
}


void CALLTYPE VMThread::Opcode_AssignReplace(VMThread *thread_ptr)
{
	/* -

	Pop an address off the stack and push back on the value stored at that address.

	- */

	int		address, value;

	address = STACK_POP(int, thread_ptr);
	value = *(int *)address;
	STACK_PUSH(value, int, thread_ptr);
}


void CALLTYPE VMThread::Opcode_InStackFrame(VMThread *thread_ptr)
{
	/* --

	Push the current value of the BP register on the stack and set BP equal to the
	current stack pointer.

	-- */

	STACK_PUSH(thread_ptr->m_regBP, int, thread_ptr);
	thread_ptr->m_regBP = (int)thread_ptr->m_Stack + thread_ptr->m_regSP;
}


void CALLTYPE VMThread::Opcode_OutStackFrame(VMThread *thread_ptr)
{
	/* -

	Pop a value off the stack and into the BP register.

	- */

	thread_ptr->m_regBP = STACK_POP(int, thread_ptr) - (int)thread_ptr->m_Stack;
}


void CALLTYPE VMThread::Opcode_AddStack(VMThread *thread_ptr)
{
	/* -

	Read a value from code and add it to the stack pointer.

	- */

	int		offset;

	READ32(offset, int, thread_ptr);
	thread_ptr->m_regSP += offset;
}


void CALLTYPE VMThread::Opcode_SubStack(VMThread *thread_ptr)
{
	/* -

	Read a value from code and subtract it from the stack pointer

	- */

	int		offset;

	READ32(offset, int, thread_ptr);
	thread_ptr->m_regSP -= offset;
}


void CALLTYPE VMThread::Opcode_Return(VMThread *thread_ptr)
{
	/* --

	Pop a return address off the stack and jump to it. If the address is -1 then the
	function was called from native code so there is nothing to return to, halt the thread.

	-- */

	int		address;

	address = STACK_POP(int, thread_ptr);
	if (address == -1)
		thread_ptr->m_Flags &= ~THREAD_FLAGS_ACTIVE;
	thread_ptr->m_regIP = address;
}


void CALLTYPE VMThread::Opcode_RPV(VMThread *thread_ptr)
{
	/* --

	Pop an address off the stack, read the value at that address, push the address
	followed by the value back onto the stack.

	-- */

	int		address, value;

	address = STACK_POP(int, thread_ptr);
	value = *(int *)address;
	STACK_PUSH(address, int, thread_ptr);
	STACK_PUSH(value, int, thread_ptr);
}


void CALLTYPE VMThread::Opcode_DecReplace(VMThread *thread_ptr)
{
	/* --

	Pop an address off the stack, decrement the integer value at that address, then
	push the value at that address back onto the stack.

	-- */

	int		address;

	address = STACK_POP(int, thread_ptr);
	(*(int *)address)--;
	STACK_PUSH(*(int *)address, int, thread_ptr);
}


void CALLTYPE VMThread::Opcode_IncReplace(VMThread *thread_ptr)
{
	/* --

	Pop an address off the stack, increment the integer value at that address, then
	push the value at that address back onto the stack.

	-- */

	int		address;

	address = STACK_POP(int, thread_ptr);
	(*(int *)address)++;
	STACK_PUSH(*(int *)address, int, thread_ptr);
}


void CALLTYPE VMThread::Opcode_DecPop(VMThread *thread_ptr)
{
	/* -

	Pop an address off the stack and decrement the integer value at that address.

	- */

	int		address;

	address = STACK_POP(int, thread_ptr);
	(*(int *)address)--;
}


void CALLTYPE VMThread::Opcode_IncPop(VMThread *thread_ptr)
{
	/* -

	Pop an address off the stack and increment the integer value at that address.

	- */

	int		address;

	address = STACK_POP(int, thread_ptr);
	(*(int *)address)++;
}


void CALLTYPE VMThread::Opcode_DecMove(VMThread *thread_ptr)
{
	/* -

	Pop an address off the stack, push the value at that address back onto the stack,
	then decrement the value at that address.

	- */

	int		address;

	address = STACK_POP(int, thread_ptr);
	STACK_PUSH(*(int *)address, int, thread_ptr);
	(*(int *)address)--;
}


void CALLTYPE VMThread::Opcode_IncMove(VMThread *thread_ptr)
{
	/* -

	Pop an address off the stack, push the value at that address back onto the stack,
	then decrement the value at that address.

	- */

	int		address;

	address = STACK_POP(int, thread_ptr);
	STACK_PUSH(*(int *)address, int, thread_ptr);
	(*(int *)address)++;
}


void CALLTYPE VMThread::Opcode_Call(VMThread *thread_ptr)
{
	/* --

	Read an address from the code, push the current instruction pointer onto the stack
	and jump to the address.

	-- */

	int		address;

	READ32(address, int, thread_ptr);
	STACK_PUSH(thread_ptr->m_regIP, int, thread_ptr);
	thread_ptr->m_regIP = address;
}


void CALLTYPE VMThread::Opcode_CallVirtual(VMThread *thread_ptr)
{
	/* ---

	Read a function index from the code, get the address from the current classes
	virtual table, push the current instruction pointer onto the stack and jump to
	the address.

	--- */

	int		index, address;

	READ32(index, int, thread_ptr);
	address = thread_ptr->m_FunctionTable[index];
	STACK_PUSH(thread_ptr->m_regIP, int, thread_ptr);
	thread_ptr->m_regIP = address;
}


void CALLTYPE VMThread::Opcode_ArrayProcess(VMThread *thread_ptr)
{
	/* --

	Read the variable address from the code, add 4 to it (size of the pointer) and
	store this value in the variable.

	-- */

	int		address;

	READ32(address, int, thread_ptr);
	thread_ptr->m_DataSegment[address] = address + 4 + (int)thread_ptr->m_DataSegment;
}


void CALLTYPE VMThread::Opcode_ArrayIndex(VMThread *thread_ptr)
{
	/* --

	Pops an index off the stack, multiplies it by 4, pops an address off the stack and
	adds this to it, pushing the result back onto the stack.

	-- */

	int		offset;

	offset = STACK_POP(int, thread_ptr) << 2;
	offset += STACK_POP(int, thread_ptr);
	STACK_PUSH(offset, int, thread_ptr);
}


void CALLTYPE VMThread::Opcode_FloatToInt(VMThread *thread_ptr)
{
	/* -

	Pops a floating point value off the stack and pushes it back on as an integer.

	- */

#ifdef	TARGET_x86

	__asm
	{
		// ECX contains the thread pointer here (first parameter)

		mov		eax, [ecx].m_regSP					// Move the stack pointer to EAX

		add		eax, [ecx].m_Stack					// Add the stack address to the pointer
		mov		edx, [eax]							// Move the value at the stack address
													//  plus the pointer to EDX
		mov		f_Temp, edx							// Store the value in the temporary float

		fld		dword ptr f_Temp					// Push the float onto the FPU stack
		fistp	dword ptr i_Temp					// Pop it off into the temporary int

		mov		edx, i_Temp							// Get the new value
		mov		[eax], edx							// Write it back to the stack position
	}

#else

	CONVERT_OP(float, int, int, thread_ptr);

#endif
}


void CALLTYPE VMThread::Opcode_FloatToShort(VMThread *thread_ptr)
{
	/* --

	Pop a floating point value off the stack and push it back onto the stack as an
	integer that has been ranged to a short.

	-- */

	CONVERT_OP(float, int, short, thread_ptr);
}


void CALLTYPE VMThread::Opcode_FloatToChar(VMThread *thread_ptr)
{
	/* --

	Pop a floating point value off the stack and push it back onto the stack as an
	integer that has been ranged to a char.

	-- */

	CONVERT_OP(float, int, char, thread_ptr);
}


void CALLTYPE VMThread::Opcode_FloatToUShort(VMThread *thread_ptr)
{
	/* --

	Pop a floating point value off the stack and push it back onto the stack as an
	integer that has been ranged to an unsigned short.

	-- */

	CONVERT_OP(float, int, unsigned short, thread_ptr);
}


void CALLTYPE VMThread::Opcode_FloatToUChar(VMThread *thread_ptr)
{
	/* --

	Pop a floating point value off the stack and push it back onto the stack as an
	integer that has been ranged to an unsigned char.

	-- */

	CONVERT_OP(float, int, unsigned char, thread_ptr);
}


void CALLTYPE VMThread::Opcode_IntToFloat(VMThread *thread_ptr)
{
	/* --

	Pop an integer value off the stack and push it back onto the stack as a floating
	point value.

	-- */

	CONVERT_OP(int, float, float, thread_ptr);
}


void CALLTYPE VMThread::Opcode_IntToShort(VMThread *thread_ptr)
{
	/* --

	Pop an integer value off the stack and push it back onto the stack as an integer
	ranged to a short.

	-- */

	CONVERT_OP(int, int, short, thread_ptr);
}


void CALLTYPE VMThread::Opcode_IntToChar(VMThread *thread_ptr)
{
	/* --

	Pop an integer value off the stack and push it back onto the stack as an integer
	ranged to a char.

	-- */

	CONVERT_OP(int, int, char, thread_ptr);
}


void CALLTYPE VMThread::Opcode_IntToUShort(VMThread *thread_ptr)
{
	/* --

	Pop an integer value off the stack and push it back onto the stack as an integer
	ranged to an unsigned short.

	-- */

	CONVERT_OP(int, int, unsigned short, thread_ptr);
}


void CALLTYPE VMThread::Opcode_IntToUChar(VMThread *thread_ptr)
{
	/* --

	Pop an integer value off the stack and push it back onto the stack as an integer
	ranged to an unsigned char.

	-- */

	CONVERT_OP(int, int, unsigned char, thread_ptr);
}


void CALLTYPE VMThread::Opcode_FAdd(VMThread *thread_ptr)
{
	/* --

	Pop two floating point values off the stack and add them together, then push the result
	back onto the stack.

	-- */

	MATH_OP(float, +, thread_ptr);
}


void CALLTYPE VMThread::Opcode_FSub(VMThread *thread_ptr)
{
	/* --

	Pop two floating point values off the stack and subtract the first one from the second
	one, then push the result back onto the stack.

	-- */

	MATH_OP_INV(float, -, thread_ptr);
}


void CALLTYPE VMThread::Opcode_FMul(VMThread *thread_ptr)
{
	/* --

	Pop two floating point values off the stack and multiply them together, then push the
	result back onto the stack.

	-- */

	MATH_OP(float, *, thread_ptr);
}


void CALLTYPE VMThread::Opcode_FDiv(VMThread *thread_ptr)
{
	/* --

	Pop two floating point values off the stack and divide the second one by the first one,
	then push the result back onto the stack.

	-- */

	MATH_OP_INV(float, /, thread_ptr);
}


void CALLTYPE VMThread::Opcode_FMod(VMThread *thread_ptr)
{
	// ... Non-existant ...
}


void CALLTYPE VMThread::Opcode_FNeg(VMThread *thread_ptr)
{
	/* --

	Pop a floating point value off the stack, invert the sign and push the result back onto
	the stack.

	-- */

	float	value;

	value = STACK_POP(float, thread_ptr);
	STACK_PUSH((-value), float, thread_ptr);
}


void CALLTYPE VMThread::Opcode_FSetEqual(VMThread *thread_ptr)
{
	/* --

	Pop two floating point values off the stack and push true onto the stack if they are
	equal, push false otherwise.

	-- */

	MATH_OP(float, ==, thread_ptr);
}


void CALLTYPE VMThread::Opcode_FSetNotEqual(VMThread *thread_ptr)
{
	/* --

	Pop two floating point values off the stack and push true onto the stack if they are
	not equal, push false otherwise.

	-- */

	MATH_OP(float, !=, thread_ptr);
}


void CALLTYPE VMThread::Opcode_FSetLess(VMThread *thread_ptr)
{
	/* --

	Pop two floating point values off the stack and push true onto the stack if the second
	is less than the first, push false otherwise.

	-- */

	MATH_OP_INV(float, <, thread_ptr);
}


void CALLTYPE VMThread::Opcode_FSetLessEqual(VMThread *thread_ptr)
{
	/* --

	Pop two floating point values off the stack and push true onto the stack if the second
	is less than or equal to the first, push false otherwise.

	-- */

	MATH_OP_INV(float, <=, thread_ptr);
}


void CALLTYPE VMThread::Opcode_FSetGreater(VMThread *thread_ptr)
{
	/* --

	Pop two floating point values off the stack and push true onto the stack if the second
	is greater than the first, push false otherwise.

	-- */

	MATH_OP_INV(float, >, thread_ptr);
}


void CALLTYPE VMThread::Opcode_FSetGreaterEqual(VMThread *thread_ptr)
{
	/* --

	Pop two floating point values off the stack and push true onto the stack if the second
	is greater than or equal to the first, push false otherwise,

	-- */

	MATH_OP_INV(int, >=, thread_ptr);
}


void CALLTYPE VMThread::Opcode_FSetEqualZero(VMThread *thread_ptr)
{
	/* --

	Pop a floating point value off the stack and push true onto the stack if the value is
	equal to zero, push false otherwise.

	-- */

	float	value;

	value = STACK_POP(float, thread_ptr);
	STACK_PUSH((value == 0), int, thread_ptr);
}


void CALLTYPE VMThread::Opcode_FJumpZero(VMThread *thread_ptr)
{
	/* --

	Read an address from the code, pop a floating point value off the stack and jump to the
	address if the value is equal to zero.

	-- */

	float	value;
	int		address;

	value = STACK_POP(float, thread_ptr);

	if (!value)
	{
		READ32(address, int, thread_ptr);
		thread_ptr->m_regIP = address;
	}
	else
	{
		thread_ptr->m_regIP += 4;
	}
}


void CALLTYPE VMThread::Opcode_FJumpNotZero(VMThread *thread_ptr)
{
	/* --

	Read an address from the code, pop a floating point value off the stack and jump to the
	address if the value is not equal to zero.

	-- */

	float	value;
	int		address;

	value = STACK_POP(float, thread_ptr);

	if (value)
	{
		READ32(address, int, thread_ptr);
		thread_ptr->m_regIP = address;
	}
	else
	{
		thread_ptr->m_regIP += 4;
	}
}


void CALLTYPE VMThread::Opcode_FJumpGreater(VMThread *thread_ptr)
{
	/* --

	Read an address from the code, pop two floating point values off the stack and jump
	to the address if the second value is greater than the first value.

	-- */

	JUMP_OP_INV(float, >, thread_ptr);
}


void CALLTYPE VMThread::Opcode_FJumpGreaterEqual(VMThread *thread_ptr)
{
	/* --

	Read an address from the code, pop two floating point values off the stack and jump
	to the address if the second value is greater than or equal to the first value.

	-- */

	JUMP_OP_INV(float, >=, thread_ptr);
}


void CALLTYPE VMThread::Opcode_FJumpLess(VMThread *thread_ptr)
{
	/* --

	Read an address from the code, pop two floating point values off the stack and
	jump to the address if the second value is less than the first value.

	-- */

	JUMP_OP_INV(float, <, thread_ptr);
}


void CALLTYPE VMThread::Opcode_FJumpLessEqual(VMThread *thread_ptr)
{
	/* --

	Read an address from the code, pop two floating point values off the stack and
	jump to the address if the second value is less than or equal to the first value.

	-- */

	JUMP_OP_INV(float, <=, thread_ptr);
}


void CALLTYPE VMThread::Opcode_FDecReplace(VMThread *thread_ptr)
{
	/* --

	Pop an address off the stack, decrement the floating point value at that address, then
	push the value at that address back onto the stack.

	-- */

	int		address;

	address = STACK_POP(int, thread_ptr);
	(*(float *)address)--;
	STACK_PUSH(*(float *)address, float, thread_ptr);
}


void CALLTYPE VMThread::Opcode_FIncReplace(VMThread *thread_ptr)
{
	/* --

	Pop an address off the stack, increment the floating point value at that address, then
	push the value at that address back onto the stack.

	-- */

	int		address;

	address = STACK_POP(int, thread_ptr);
	(*(float *)address)++;
	STACK_PUSH(*(float *)address, float, thread_ptr);
}


void CALLTYPE VMThread::Opcode_FDecPop(VMThread *thread_ptr)
{
	/* -

	Pop an address off the stack and decrement the floating point value at that address.

	- */

	int		address;

	address = STACK_POP(int, thread_ptr);
	(*(float *)address)--;
}


void CALLTYPE VMThread::Opcode_FIncPop(VMThread *thread_ptr)
{
	/* -

	Pop an address off the stack and increment the floating point value at that address.

	- */

	int		address;

	address = STACK_POP(int, thread_ptr);
	(*(float *)address)++;
}


void CALLTYPE VMThread::Opcode_FDecMove(VMThread *thread_ptr)
{
	/* -

	Pop an address off the stack, push the value at that address back onto the stack,
	then decrement the value at that address.

	- */

	int		address;

	address = STACK_POP(int, thread_ptr);
	STACK_PUSH(*(float *)address, float, thread_ptr);
	(*(float *)address)--;
}


void CALLTYPE VMThread::Opcode_FIncMove(VMThread *thread_ptr)
{
	/* -

	Pop an address off the stack, push the value at that address back onto the stack,
	then decrement the value at that address.

	- */

	int		address;

	address = STACK_POP(int, thread_ptr);
	STACK_PUSH(*(float *)address, float, thread_ptr);
	(*(float *)address)++;
}


void CALLTYPE VMThread::Opcode_PushMemberArray(VMThread *thread_ptr)
{
	/* ---

	Read an address from the code, lookup the value at this address and push the value
	plus the data segment back onto the stack.

	(* Need a range check for the data segment *)

	--- */

	int		address, value;

	READ32(address, int, thread_ptr);
	value = *(int *)&(thread_ptr->m_DataSegment[address]);
	value += (int)thread_ptr->m_DataSegment;
	STACK_PUSH(value, int, thread_ptr);
}


void CALLTYPE VMThread::Opcode_PushLocalArray(VMThread *thread_ptr)
{
	/* ----

	Read the address from the code segment, add this to the BP register and read 32-bits
	from that address (should be within the stack), pushing the value plus the data
	segment on the stack.

	(* Needs a stack range check *)

	---- */

	int		address, value;

	READ32(address, int, thread_ptr);
	address += thread_ptr->m_regBP;
	value = *(int *)address;
	value += (int)thread_ptr->m_DataSegment;
	STACK_PUSH(value, int, thread_ptr);
}


void CALLTYPE VMThread::Opcode_SetState(VMThread *thread_ptr)
{
	/* --

	Read the state index from the code segment, lookup the state address in the virtual
	table, reset the stack pointer and jump to the state code.

	-- */

	int		index;

	READ32(index, int, thread_ptr);
	thread_ptr->m_regSP = thread_ptr->m_StackSize - 4;
	thread_ptr->m_regIP = thread_ptr->m_StateTable[index];
}


void CALLTYPE VMThread::Opcode_EndState(VMThread *thread_ptr)
{
	/* -

	The state has ended so suspend the thread.

	- */

	thread_ptr->m_Flags &= ~THREAD_FLAGS_ACTIVE;
}


void CALLTYPE VMThread::Opcode_CallImport(VMThread *thread_ptr)
{
	int				index, ret_type, ret_value;
	NativeMethod	*method_ptr;
	VMImport		*import_ptr;
	int				nb_params;
	void			*object_ptr, *func_ptr, *stack_ptr;

	// Read the import function index
	READ32(index, int, thread_ptr);

	// Get the method that is being called
	method_ptr = (NativeMethod *)thread_ptr->m_ImportTable[index];
	func_ptr = (void *)method_ptr->m_Address;
	import_ptr = method_ptr->m_Import;
	nb_params = import_ptr->GetNbParams();
	ret_type = import_ptr->GetReturnType();
	object_ptr = thread_ptr->m_NativeObject;
	stack_ptr = &((char *)thread_ptr->m_Stack)[thread_ptr->m_regSP + 4];

	__asm
	{
		; Calculate the portion of the stack to copy

		mov		ecx, nb_params

		; Allocate the space on the stack

		mov		edx, ecx
		shl		edx, 2
		sub		esp, edx

		; The source is the VM stack, the destination is the program stack

		mov		edi, esp
		mov		esi, stack_ptr

		; Copy it

		rep		movsd

		; Do a C++ method call

		mov		ecx, object_ptr
		call	func_ptr

		; Put the return type in ECX

		mov		ecx, ret_type

		; If it's void, leave

		cmp		ecx, 0
		jz		no_return

		; Check to see if it's a floating point return

		cmp		ecx, 8
		jz		ret_float

		; All integer values are returned in EAX/AX/AL

		mov		ret_value, eax
		jmp		leave_asm

	ret_float:

		; Floating point values are on the FPU stack

		fstp	dword ptr [ret_value]
		jmp		leave_asm

		; No return type, clear the return register

	no_return:
		mov		eax, 0
		mov		ret_value, eax

	leave_asm:
	}

	// Pop all the parameters of the VM stack
	thread_ptr->m_regSP += (nb_params << 2);

	// Set the return value
	thread_ptr->m_regRV = ret_value;
}