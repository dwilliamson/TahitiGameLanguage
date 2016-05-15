
#include "..\include\main.h"


VMUFile::VMUFile(const char *filename)
{
	infunc(VMUFile::VMUFile);

	// Open the output file
	if ((file = new CFile(filename, FILEOPEN_WRITE)) == NULL)
		throw CError("Couldn't allocate file structure");

	// Register the current VMU file
	g_Unit = this;

	code_size = 0;

	outfunc;
}


VMUFile::~VMUFile(void)
{
	infunc(VMUFile::~VMUFile);

	// Write the size of the code segment
	file->Write(&code_size, sizeof(int));

	// Close the output file
	delete file;

	outfunc;
}


void VMUFile::AddObjectCode(Class *class_ptr)
{
	infunc(VMUFile::AddObjectCode);

	CFile	*object_file;
	int		size, position = 0;
	int		value, shift;

	// Interfaces don't need to be added since they have no VM code body
	if (class_ptr->GetFlags() & CLASS_FLAGS_INTERFACE)
		return;

	// Open the input object file
	if ((object_file = new CFile(class_ptr->GetObjectFile(), FILEOPEN_READ)) == NULL)
		throw CError("Couldn't allocate file structure");

	// This is the value to shift jumps by
	shift = file->GetPosition();

	// Shift all addresses according to their position in the code segment
	class_ptr->UpdateCodeAddresses(shift);

	// Read the length of the code
	object_file->SeekTo(-4, FILESEEK_END);
	object_file->Read(&size, sizeof(int));
	object_file->SeekTo(0, FILESEEK_START);

	// Modify the code segment size
	code_size += size;

	// Transfer each byte of the object file
	while (position < size)
	{
		int		opcode;

		// Read the currrent opcode
		opcode = object_file->ReadByte();
		position++;

		// Determine how to write it out
		switch (opcode)
		{
			// Single-byte operations, just transfer
			case (OPCODE_NOP):
			case (OPCODE_ADD):
			case (OPCODE_SUB):
			case (OPCODE_MUL):
			case (OPCODE_DIV):
			case (OPCODE_MOD):
			case (OPCODE_AND):
			case (OPCODE_OR):
			case (OPCODE_XOR):
			case (OPCODE_SETE):
			case (OPCODE_SETNE):
			case (OPCODE_SETL):
			case (OPCODE_SETLE):
			case (OPCODE_SETG):
			case (OPCODE_SETGE):
			case (OPCODE_SETEZ): 
			case (OPCODE_NOT):
			case (OPCODE_NEG):
			case (OPCODE_SHL):
			case (OPCODE_SHR):
			case (OPCODE_INC):
			case (OPCODE_DEC):
			case (OPCODE_ASSIGN):
			case (OPCODE_ASSIGN_PUSH):
			case (OPCODE_ASSIGN_REPLACE):
			case (OPCODE_INSTF):
			case (OPCODE_OUTSTF):
			case (OPCODE_RET):
			case (OPCODE_ARRAYI):
			case (OPCODE_RPV):
			case (OPCODE_DECR):
			case (OPCODE_INCR):
			case (OPCODE_DECP):
			case (OPCODE_INCP):
			case (OPCODE_DECM):
			case (OPCODE_INCM):
			case (OPCODE_PUSH_RETURN):
			case (OPCODE_POP_RETURN):
			case (OPCODE_POP):
			case (OPCODE_FTOI):
			case (OPCODE_FTOS):
			case (OPCODE_FTOC):
			case (OPCODE_FTOUS):
			case (OPCODE_FTOUC):
			case (OPCODE_ITOF):
			case (OPCODE_ITOS):
			case (OPCODE_ITOC):
			case (OPCODE_ITOUS):
			case (OPCODE_ITOUC):
			case (OPCODE_FADD):
			case (OPCODE_FSUB):
			case (OPCODE_FMUL):
			case (OPCODE_FDIV):
			case (OPCODE_FMOD):
			case (OPCODE_FNEG):
			case (OPCODE_FSETE):
			case (OPCODE_FSETNE):
			case (OPCODE_FSETL):
			case (OPCODE_FSETLE):
			case (OPCODE_FSETG):
			case (OPCODE_FSETGE):
			case (OPCODE_FSETEZ):
			case (OPCODE_FDECR):
			case (OPCODE_FINCR):
			case (OPCODE_FDECP):
			case (OPCODE_FINCP):
			case (OPCODE_FDECM):
			case (OPCODE_FINCM):
			case (OPCODE_ENDSTATE):
				file->WriteByte(opcode);
				break;

			// Operations with accompanying 4-byte values, just transfer
			case (OPCODE_PUSH_IMMEDIATE):
			case (OPCODE_PUSH_MEMBER_VALUE):
			case (OPCODE_PUSH_MEMBER_ADDRESS):
			case (OPCODE_PUSH_LOCAL_VALUE):
			case (OPCODE_PUSH_LOCAL_ADDRESS):
			case (OPCODE_ADDST):
			case (OPCODE_SUBST):
			case (OPCODE_CALL):
			case (OPCODE_CALLV):
			case (OPCODE_ARRAYP):
			case (OPCODE_FPUSH_IMMEDIATE):
			case (OPCODE_PUSH_MEMBER_ARRAY):
			case (OPCODE_PUSH_LOCAL_ARRAY):
			case (OPCODE_SETSTATE):
			case (OPCODE_CALLIM):
				object_file->Read(&value, sizeof(int));
				file->WriteByte(opcode);
				file->Write(&value, sizeof(int));
				position += 4;
				break;

			// Jump operations that need shifting
			case (OPCODE_JMP):
			case (OPCODE_JZ):
			case (OPCODE_JNZ):
			case (OPCODE_JG):
			case (OPCODE_JGE):
			case (OPCODE_JL):
			case (OPCODE_JLE):
			case (OPCODE_JR):
			case (OPCODE_FJZ):
			case (OPCODE_FJNZ):
			case (OPCODE_FJG):
			case (OPCODE_FJGE):
			case (OPCODE_FJL):
			case (OPCODE_FJLE):
				object_file->Read(&value, sizeof(int));
				value += shift;
				file->WriteByte(opcode);
				file->Write(&value, sizeof(int));
				position += 4;
				break;
		}
	}

	// Close the object file
	delete object_file;

	outfunc;
}


void VMUFile::AddClassInfo(Class *class_ptr)
{
	infunc(VMUFile::AddClassInfo);
	class_ptr->WriteVMUInfo(file);
	outfunc;
}


void VMUFile::WriteDataSegment(Class *class_ptr, char *data, int size)
{
	infunc(VMUFile::WriteDataSegment);

	// Construct the data segment image
	class_ptr->WriteDataSegment(data);

	// Write the data segment size
	file->Write(&size, sizeof(int));

	// Write the data segment!
	file->Write(data, size);

	outfunc;
}


void VMUFile::BackpatchInt(int what, int where)
{
	infunc(VMUFile::BackpatchInt);

	int		position;

	// Remember the current position
	position = file->GetPosition();

	// Seek to the desired location and write the value
	file->SeekTo(where, FILESEEK_START);
	file->Write(&what, sizeof(int));

	// Restore the original position
	file->SeekTo(position, FILESEEK_START);

	outfunc;
}


void VMUFile::WriteInt(int value)
{
	infunc(VMUFile::WriteInt);
	file->Write(&value, sizeof(int));
	outfunc;
}