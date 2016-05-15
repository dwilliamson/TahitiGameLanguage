
#include "..\include\main.h"


char *OpcodeString[] =
{
	"nop",
	"push",
	"fpush",
	"pushc",
	"pushc",
	"pushl",
	"pushl",
	"pushr",
	"popr",
	"pop",
	"add",
	"sub",
	"mul",
	"div",
	"mod",
	"and",
	"or",
	"xor",
	"shl",
	"shr",
	"not",
	"neg",
	"inc",
	"dec",
	"sete",
	"setne",
	"setl",
	"setle",
	"setg",
	"setge",
	"setez",
	"jmp",
	"jz",
	"jnz",
	"jg",
	"jge",
	"jl",
	"jle",
	"jr",
	"asg",
	"asgp",
	"asgr",
	"instf",
	"outstf",
	"addst",
	"subst",
	"ret",
	"rpv",
	"decr",
	"incr",
	"decp",
	"incp",
	"decm",
	"incm",
	"call",
	"callv",
	"arrayp",
	"arrayi",
	"ftoi",
	"ftos",
	"ftoc",
	"ftous",
	"ftouc",
	"itof",
	"itos",
	"itoc",
	"itous",
	"itouc",
	"fadd",
	"fsub",
	"fmul",
	"fdiv",
	"fmod",
	"fneg",
	"fsete",
	"fsetne",
	"fsetl",
	"fsetle",
	"fsetg",
	"fsetge",
	"fsetez",
	"fjz",
	"fjnz",
	"fjg",
	"fjge",
	"fjl",
	"fjle",
	"fdecr",
	"fincr",
	"fdecp",
	"fincp",
	"fdecm",
	"fincm",
	"pushca",
	"pushla",
	"setst",
	"endst",
	"callim"
};


VMOFile::VMOFile(const char *_filename)
{
	infunc(VMOFile::VMOFile);

	// Open the file 
	if ((file = new CFile(_filename, FILEOPEN_WRITE)) == NULL)
		throw CError("Couldn't open '%s' for writing", _filename);

	// Set the name
	strcpy(filename, _filename);

	add_level = 0;

	outfunc;
}


VMOFile::~VMOFile(void)
{
	infunc(VMOFile::~VMOFile);

	// Close the opened file
	delete file;

	outfunc;
}


void VMOFile::WriteOp(OpcodeType opcode, ...)
{
	va_list	arglist;
	dynamic	value;
	double	dv;

	infunc(VMOFile::WriteOp);

	// Start the variable arguments thingy
	va_start(arglist, opcode);

	// Write the opcode
	file->WriteByte(opcode);

	// Big switch statement!!!
	switch (opcode)
	{
		case (OPCODE_PUSH_IMMEDIATE):
		case (OPCODE_PUSH_MEMBER_VALUE):
		case (OPCODE_PUSH_MEMBER_ADDRESS):
		case (OPCODE_PUSH_LOCAL_VALUE):
		case (OPCODE_PUSH_LOCAL_ADDRESS):
		case (OPCODE_JMP):
		case (OPCODE_JZ):
		case (OPCODE_JNZ):
		case (OPCODE_JG):
		case (OPCODE_JGE):
		case (OPCODE_JL):
		case (OPCODE_JLE):
		case (OPCODE_JR):
		case (OPCODE_ADDST):
		case (OPCODE_SUBST):
		case (OPCODE_CALL):
		case (OPCODE_CALLV):
		case (OPCODE_ARRAYP):
		case (OPCODE_FJZ):
		case (OPCODE_FJNZ):
		case (OPCODE_FJG):
		case (OPCODE_FJGE):
		case (OPCODE_FJL):
		case (OPCODE_FJLE):
		case (OPCODE_PUSH_MEMBER_ARRAY):
		case (OPCODE_PUSH_LOCAL_ARRAY):
		case (OPCODE_SETSTATE):
		case (OPCODE_CALLIM):
			// Write the immediate
			value.i = va_arg(arglist, int);
			file->Write(&value.i, sizeof(value.i));
			break;

		case (OPCODE_FPUSH_IMMEDIATE):
			// Write the floating point immediate
			// Arrghh, VC passes floats through the stack as doubles!
			dv = va_arg(arglist, double);
			value.f = (float)dv;
			file->Write(&value.f, sizeof(value.f));
			break;
	}

	outfunc;
}


int VMOFile::GetPosition(void)
{
	return (file->GetPosition());
}


void VMOFile::AddBackpatchItem(int label)
{
	BackpatchItem	*item;

	infunc(VMOFile::AddBackpatchItem);

	// Construct the item information
	item = new BackpatchItem;
	item->position = GetPosition() + 1;
	item->label = label;
	item->level = add_level;

	// Add it to the linked list
	bpitems.AddLast(item);

	outfunc;
}


void VMOFile::UpdateItems(int label, int address)
{
	BackpatchItem	*item, *next;
	int				jmp;

	infunc(VMOFile::UpdateItems);

	// Get the needed code position
	if (address == -1)
		jmp = GetPosition();
	else
		jmp = address;

	// Loop through the items
	for (item = bpitems.GetFirst(); item; item = next)
	{
		next = item->next;

		// Back-patch and empty
		if (item->label == label && add_level <= item->level)
		{
			BackpatchInt(jmp, item->position);
			bpitems.Remove(item);
			delete item;
		}
	}

	outfunc;
}


void VMOFile::BackpatchInt(int value, int where)
{
	infunc(VMOFile::BackpatchInt);
	file->WriteAt(where, &value, 4);
	outfunc;
}


void VMOFile::Disassemble(void)
{
	int		opcode = 0;
	dynamic	value;
	int		end, position = 0;

	infunc(VMOFile::Disassemble);

	// Close the already opened file
	delete file;

	// Open the file for reading now
	if ((file = new CFile(filename, FILEOPEN_READ)) == NULL)
		throw CError("Couldn't open '%s' for reading", filename);

	// Read the code size
	file->SeekTo(-4, FILESEEK_END);
	file->Read(&end, 4);
	file->SeekTo(0, FILESEEK_START);

	// Loop reading until the end of the file
	while (file->GetPosition() < end)
	{
		opcode = file->ReadByte();

		if (opcode == FILE_EOF)
			break;

		switch (opcode)
		{
			case (OPCODE_PUSH_IMMEDIATE):
			case (OPCODE_PUSH_MEMBER_VALUE):
			case (OPCODE_PUSH_MEMBER_ADDRESS):
			case (OPCODE_PUSH_LOCAL_VALUE):
			case (OPCODE_PUSH_LOCAL_ADDRESS):
			case (OPCODE_JMP):
			case (OPCODE_JZ):
			case (OPCODE_JNZ):
			case (OPCODE_JG):
			case (OPCODE_JGE):
			case (OPCODE_JL):
			case (OPCODE_JLE):
			case (OPCODE_JR):
			case (OPCODE_ADDST):
			case (OPCODE_SUBST):
			case (OPCODE_CALL):
			case (OPCODE_CALLV):
			case (OPCODE_ARRAYP):
			case (OPCODE_FJZ):
			case (OPCODE_FJNZ):
			case (OPCODE_FJG):
			case (OPCODE_FJGE):
			case (OPCODE_FJL):
			case (OPCODE_FJLE):
			case (OPCODE_PUSH_MEMBER_ARRAY):
			case (OPCODE_PUSH_LOCAL_ARRAY):
			case (OPCODE_SETSTATE):
			case (OPCODE_CALLIM):
				file->Read(&value.i, 4);

				printf("%04d:\t%s\t%d", position, OpcodeString[opcode], value.i);

				position += 5;
				break;

			case (OPCODE_FPUSH_IMMEDIATE):
				file->Read(&value.f, 4);

				printf("%04d:\t%s\t%f", position, OpcodeString[opcode], value.f);

				position += 5;
				break;

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
				printf("%04d:\t%s", position, OpcodeString[opcode]);
				position++;
				break;
		}

		printf("\n");
	}

	printf("\n");

	outfunc;
}


void VMOFile::WriteClassInfo(Class *class_ptr)
{
	infunc(VMOFile::WriteClassInfo);
	class_ptr->Write(file);
	outfunc;
}