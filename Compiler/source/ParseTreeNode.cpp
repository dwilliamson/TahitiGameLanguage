
#include "..\include\main.h"


ParseTreeNode::~ParseTreeNode(void)
{
	// Delete the left child
	if (children[0])
		delete children[0];

	// Delete the right child
	if (children[1])
		delete children[1];
}


void ParseTreeNode::AssignLeftChild(ParseTreeNode *child)
{
	// Assign
	children[0] = child;
	if (child) child->parent = this;
}


void ParseTreeNode::AssignRightChild(ParseTreeNode *child)
{
	// Assign
	children[1] = child;
	if (child) child->parent = this;
}


void ParseTreeNode::GenerateCode(int flags)
{
	infunc(ParseTreeNode::GenerateCode);

	int		lflags = flags, rflags = flags;
	VarType	ctype[2];

	g_Object->add_level++;

	/* -----------

	So that the child nodes know the situation they are in (e.g. in an assignment), modify
	the flags here before generating any code.

	A single assignment needs to notify the left child that it is being assigned to and
	the right child that it is in an assignment. This allows the code further on to push
	either an address or value as required.

	Coupled operators and assignments need to know the same thing with one extra detail.
	The child being assigned to has to duplicate its value on the stack (pushing its
	address one down) so that it can be used with the operator.

	The post and pre-effect operators always need the address pushed so temporarily
	fake that it is a value being assigned to. Do this to both nodes since only one node
	will be valid, depending upon the type of effect.

	----------- */

	// This should only be one level deep so it represents the immediate parent
	lflags &= ~FLAGS_INTO_BINARYOP;
	rflags &= ~FLAGS_INTO_BINARYOP;

	switch (token.type)
	{
		// Single assignment
		case (TOKEN_ASSIGN):
			lflags |= FLAGS_ASSIGN_DEST;
			rflags |= FLAGS_EXPLICIT_ASSIGNMENT;
			break;

		// Coupled operator and assignment
		case (TOKEN_PLUS_ASSIGN):
		case (TOKEN_MINUS_ASSIGN):
		case (TOKEN_MULTIPLY_ASSIGN):
		case (TOKEN_DIVIDE_ASSIGN):
		case (TOKEN_MODULUS_ASSIGN):
		case (TOKEN_BITWISE_AND_ASSIGN):
		case (TOKEN_BITWISE_OR_ASSIGN):
		case (TOKEN_BITWISE_XOR_ASSIGN):
		case (TOKEN_SHL_ASSIGN):
		case (TOKEN_SHR_ASSIGN):
			lflags |= FLAGS_ASSIGNOP_DEST;
			rflags |= FLAGS_EXPLICIT_ASSIGNMENT;
			break;

		// Post and pre-effect operators
		case (TOKEN_PLUS_PLUS):
		case (TOKEN_MINUS_MINUS):
			lflags |= FLAGS_ASSIGN_DEST; lflags &= ~FLAGS_ASSIGNMENT;
			rflags |= FLAGS_ASSIGN_DEST; rflags &= ~FLAGS_ASSIGNMENT;
			break;

		// Assignments within the array block
		case (TOKEN_ARRAY_OPEN):
			rflags |= FLAGS_IMPLICIT_ASSIGNMENT;
			break;

		// Binary branching operators
		case (TOKEN_LOGICAL_AND):
			lflags |= FLAGS_INTO_BINARYANDOP;
			rflags |= FLAGS_INTO_BINARYANDOP;
			break;

		case (TOKEN_LOGICAL_OR):
			lflags |= FLAGS_INTO_BINARYOROP;
			rflags |= FLAGS_INTO_BINARYOROP;
			break;
	}

	// Generate left node
	if (children[0])
	{
		children[0]->GenerateCode(lflags);
		ctype[0] = children[0]->out_type;

		if (token.Is(TOKEN_TYPE_OPERATOR | TTNOT(TOKEN_TYPE_BRACKETOPS)))
			WriteConverter(ctype[0].id, in_type.id);

		/* ---

		If the left child is not a binary operator then it must be an expression of some
		kind. This means that the jumps pertaining to the current token type need to
		be emitted.

		--- */

		if (!children[0]->token.Is(TOKEN_TYPE_BRANCHOPS))
		{
			/* -----

			With AND, emit a jump to the false label if the left expression fails because
			both sides of the AND need to succeed.

			With OR, emit a jump to true if the left expression succeeds since either
			side is allowed to succeed. Upon failure of the left side, the code falls
			throuh to process the right side.

			----- */

			switch (token.type)
			{
				case (TOKEN_LOGICAL_AND):
					// Add a false item
					g_Object->AddBackpatchItem(0);

					if (in_type.id == VARIABLE_TYPEID_FLOAT)
						g_Object->WriteOp(OPCODE_FJZ, 0);
					else
						g_Object->WriteOp(OPCODE_JZ, 0);
					break;

				case (TOKEN_LOGICAL_OR):
					// Add a true item
					g_Object->AddBackpatchItem(1);

					if (in_type.id == VARIABLE_TYPEID_FLOAT)
						g_Object->WriteOp(OPCODE_FJNZ, 1);
					else
						g_Object->WriteOp(OPCODE_JNZ, 1);
					break;
			}
		}

		/* -------

		When the current node is a branchop along with its child, the true/false labels
		may need to be updated.

		With AND, set the true label to the current position and back-patch all "jnz true"
		jumps. This is so that both sides of an AND are processed to ensure success.

		With OR, set the false label to the current position and back-patch all "jz false"
		jumps. This is so that the right side of the OR is processed if the left side
		fails.

		------- */

		else
		{
			switch (token.type)
			{
				case (TOKEN_LOGICAL_AND):
					// ... Set true label to here and back-patch "jnz true" jumps
					g_Object->UpdateItems(1);
					break;

				case (TOKEN_LOGICAL_OR):
					g_Object->UpdateItems(0);
					// ... Set false label to here and back-patch "jz false" jumps
					break;
			}
		}
	}

	// Generate right node
	if (children[1])
	{
		children[1]->GenerateCode(rflags);
		ctype[1] = children[1]->out_type;

		if (token.Is(TOKEN_TYPE_OPERATOR | TTNOT(TOKEN_TYPE_BRACKETOPS)))
			WriteConverter(ctype[1].id, in_type.id);

		/* --------

		With the right side, jump to true if it succeeds, else just jump to false. If
		the current node's parent is not a binary operator then the true/false code
		needs to be written her so just let the code fall through to false instead of
		jumping to it.

		If, with binary operators, the current node is the same type as it's parent node
		then this right child can be treated as a left child. This means a sequence of
		the same binary operators has been encountered so the above code need not be
		written until the pattern is broken.

		-------- */

		if (!children[1]->token.Is(TOKEN_TYPE_BRANCHOPS))
		{
			if (token.type == TOKEN_LOGICAL_AND || token.type == TOKEN_LOGICAL_OR)
			{
				if (token.type == TOKEN_LOGICAL_AND && (flags & FLAGS_INTO_BINARYANDOP))
				{
					// Add a false item
					g_Object->AddBackpatchItem(0);

					if (in_type.id == VARIABLE_TYPEID_FLOAT)
						g_Object->WriteOp(OPCODE_FJZ, 0);
					else
						g_Object->WriteOp(OPCODE_JZ, 0);
				}

				else if (token.type == TOKEN_LOGICAL_OR && (flags & FLAGS_INTO_BINARYOROP))
				{
					// Add a true item
					g_Object->AddBackpatchItem(1);

					if (in_type.id == VARIABLE_TYPEID_FLOAT)
						g_Object->WriteOp(OPCODE_FJNZ, 1);
					else
						g_Object->WriteOp(OPCODE_JNZ, 1);
				}
				
				else
				{
					// Add a true item
					g_Object->AddBackpatchItem(1);

					if (in_type.id == VARIABLE_TYPEID_FLOAT)
						g_Object->WriteOp(OPCODE_FJNZ, 1);
					else
						g_Object->WriteOp(OPCODE_JNZ, 1);

					// Let it fall through to false if the false code is to be written here
					if (flags & FLAGS_INTO_BINARYOP)
					{
						g_Object->AddBackpatchItem(0);
						g_Object->WriteOp(OPCODE_JMP, 0);
					}
				}
			}
		}

		/* -

		When the current node is a branchop, do the same as at the left child.

		- */

		else
		{
			switch (token.type)
			{
				case (TOKEN_LOGICAL_AND):
					g_Object->UpdateItems(1);
					// ... Set true label to here and back-patch "jnz true" jumps
					break;

				case (TOKEN_LOGICAL_OR):
					g_Object->UpdateItems(0);
					// ... Set false label to here and back-patch "jz false" jumps
					break;
			}
		}
	}

	/* ------------------------

	Normal two-sided operations simply need the appropriate opcode emitted at this stage,
	depending on if it's an integer or floating point operand.

	With assignments, the top of the stack should contain a value resulting from the
	expression at the right child. If the code is within the body of an assignment
	already (of the type x = y or [x]) then after placing the value at the top of the
	stack into the address below it (popping them both off), the value should be pushed
	back onto the stack so that it can be used again. Otherwise just perform the
	assignment.

	With coupled operator and assignments, a value is on the top of the stack ready to
	be operated upon, while the destination of the result is below it. Perform the intended
	operation and do a normal assignment.

	Unary pre-effect operators increment/decrement the value at an address, remove the
	address from the stack and pushes the value back on. This is one instruction. If
	this isn't within an assignment, the value does not need to be pushed back onto the
	stack.

	Post effect operators replace the address on the stack with the value stored there
	and increment/decrement the value at the address. This is also one instruction. Like
	above, if this isn't within an assignment, the address merely needs to be popped off
	the stack. This leads to the same instruction (when not in an assignment) as the
	pre-effect operators (when not in an assignment).

	With array indexing, the left child holds the address of the array and the right child
	holds the index into that array. Each array element is always 32-bits in size so this
	index needs to be multiplied by four and added to the address to produce a final address
	on the stack. The "arrayi" instruction will do this. If, after processing the entire
	array lookup, the code is within an assignment, use the "asgr" instruction to replace
	the address with the value at that address.

	If the current node is a binary operator (&&, ||) then the final true/false labels
	need to be emitted so that the result can be converted into arithmetic. This should
	only be done if the current node has no parent binary operator since the true/false
	labels in that case would be taken up another level.

	------------------------ */

	#define WRITE_OP(x, y)	{ if (in_type.id == VARIABLE_TYPEID_FLOAT) g_Object->WriteOp(y); else g_Object->WriteOp(x); }

	switch (token.type)
	{
		// Normal two-sided operators ----------------------------------------------------

		case (TOKEN_PLUS):				WRITE_OP(OPCODE_ADD, OPCODE_FADD); break;
		case (TOKEN_MINUS):				WRITE_OP(OPCODE_SUB, OPCODE_FSUB); break;
		case (TOKEN_MULTIPLY):			WRITE_OP(OPCODE_MUL, OPCODE_FMUL); break;
		case (TOKEN_DIVIDE):			WRITE_OP(OPCODE_DIV, OPCODE_FDIV); break;
		case (TOKEN_MODULUS):			WRITE_OP(OPCODE_MOD, OPCODE_FMOD); break;
		case (TOKEN_BITWISE_AND):		WRITE_OP(OPCODE_AND, 0); break;				// Need to generate errors for these (float)
		case (TOKEN_BITWISE_OR):		WRITE_OP(OPCODE_OR, 0); break;
		case (TOKEN_BITWISE_XOR):		WRITE_OP(OPCODE_XOR, 0); break;
		case (TOKEN_SHIFT_LEFT):		WRITE_OP(OPCODE_SHL, 0); break;
		case (TOKEN_SHIFT_RIGHT):		WRITE_OP(OPCODE_SHR, 0); break;
		case (TOKEN_LESS):				WRITE_OP(OPCODE_SETL, OPCODE_FSETL); break;
		case (TOKEN_LESS_EQUAL):		WRITE_OP(OPCODE_SETLE, OPCODE_FSETLE); break;
		case (TOKEN_GREATER):			WRITE_OP(OPCODE_SETG, OPCODE_FSETG); break;
		case (TOKEN_GREATER_EQUAL):		WRITE_OP(OPCODE_SETGE, OPCODE_FSETGE); break;
		case (TOKEN_EQUAL):				WRITE_OP(OPCODE_SETE, OPCODE_FSETE); break;
		case (TOKEN_LOGICAL_NOT_EQUAL):	WRITE_OP(OPCODE_SETNE, OPCODE_FSETNE); break;

		// Normal assignment -------------------------------------------------------------

		case (TOKEN_ASSIGN):
			if (flags & FLAGS_ASSIGNMENT)
				g_Object->WriteOp(OPCODE_ASSIGN_PUSH);
			else
				g_Object->WriteOp(OPCODE_ASSIGN);
			break;

		// Coupled operator and assignment -----------------------------------------------

		case (TOKEN_PLUS_ASSIGN):
		case (TOKEN_MINUS_ASSIGN):
		case (TOKEN_MULTIPLY_ASSIGN):
		case (TOKEN_DIVIDE_ASSIGN):
		case (TOKEN_MODULUS_ASSIGN):
		case (TOKEN_BITWISE_AND_ASSIGN):
		case (TOKEN_BITWISE_OR_ASSIGN):
		case (TOKEN_BITWISE_XOR_ASSIGN):
		case (TOKEN_SHL_ASSIGN):
		case (TOKEN_SHR_ASSIGN):
			switch (token.type)
			{
				case (TOKEN_PLUS_ASSIGN):			WRITE_OP(OPCODE_ADD, OPCODE_FADD); break;
				case (TOKEN_MINUS_ASSIGN):			WRITE_OP(OPCODE_SUB, OPCODE_FSUB); break;
				case (TOKEN_MULTIPLY_ASSIGN):		WRITE_OP(OPCODE_MUL, OPCODE_FMUL); break;
				case (TOKEN_DIVIDE_ASSIGN):			WRITE_OP(OPCODE_DIV, OPCODE_FDIV); break;
				case (TOKEN_MODULUS_ASSIGN):		WRITE_OP(OPCODE_MOD, OPCODE_FMOD); break;
				case (TOKEN_BITWISE_AND_ASSIGN):	WRITE_OP(OPCODE_AND, 0); break;
				case (TOKEN_BITWISE_OR_ASSIGN):		WRITE_OP(OPCODE_OR, 0); break;
				case (TOKEN_BITWISE_XOR_ASSIGN):	WRITE_OP(OPCODE_XOR, 0); break;
				case (TOKEN_SHL_ASSIGN):			WRITE_OP(OPCODE_SHL, 0); break;
				case (TOKEN_SHR_ASSIGN):			WRITE_OP(OPCODE_SHR, 0); break;
			}

			if (flags & FLAGS_ASSIGNMENT)
				g_Object->WriteOp(OPCODE_ASSIGN_PUSH);
			else
				g_Object->WriteOp(OPCODE_ASSIGN);
			break;

		// Increment/Decrement operators -------------------------------------------------

		case (TOKEN_PLUS_PLUS):
			if (children[0])
			{
				if (flags & FLAGS_ASSIGNMENT)
					WRITE_OP(OPCODE_INCM, OPCODE_FINCM)
				else
					WRITE_OP(OPCODE_INCP, OPCODE_FINCP)
			}

			// Unary pre-increment
			else
			{
				if (flags & FLAGS_ASSIGNMENT)
					WRITE_OP(OPCODE_INCR, OPCODE_FINCR)
				else
					WRITE_OP(OPCODE_INCP, OPCODE_FINCP)
			}
			break;

		case (TOKEN_MINUS_MINUS):
			if (children[0])
			{
				if (flags & FLAGS_ASSIGNMENT)
					WRITE_OP(OPCODE_DECM, OPCODE_FDECM)
				else
					WRITE_OP(OPCODE_DECP, OPCODE_FDECP)
			}

			// Unary pre-decrement
			else
			{
				if (flags & FLAGS_ASSIGNMENT)
					WRITE_OP(OPCODE_DECR, OPCODE_FDECR)
				else
					WRITE_OP(OPCODE_DECP, OPCODE_FDECP)
			}
			break;

		// Miscellaneous unary operators -------------------------------------------------

		case (TOKEN_BITWISE_NOT): g_Object->WriteOp(OPCODE_NOT); break;
		case (TOKEN_LOGICAL_NOT): WRITE_OP(OPCODE_SETEZ, OPCODE_FSETEZ); break;

		// Array indexing ----------------------------------------------------------------

		case (TOKEN_ARRAY_OPEN):
			g_Object->WriteOp(OPCODE_ARRAYI);
			
			if ((flags & FLAGS_ASSIGNMENT) && !(flags & FLAGS_ASSIGN_DEST))
				g_Object->WriteOp(OPCODE_ASSIGN_REPLACE);
			break;

		// Binary branching operators ----------------------------------------------------

		case (TOKEN_LOGICAL_AND):
		case (TOKEN_LOGICAL_OR):
			if (!(flags & FLAGS_INTO_BINARYOP))
			{
				// False label
				g_Object->UpdateItems(0);
				g_Object->WriteOp(OPCODE_PUSH_IMMEDIATE, 0);
				g_Object->WriteOp(OPCODE_JR, 5);

				// True label
				g_Object->UpdateItems(1);
				g_Object->WriteOp(OPCODE_PUSH_IMMEDIATE, 1);
			}
			break;
	}

	#undef	WRITE_OP

	g_Object->add_level--;

	outfunc;
}


void ParseTreeNode::Optimise(ParseTreeNode **holder)
{
	infunc(ParseTreeNode::Optimise);

	// If there are no children, nothing to do here
	if (children[0] == NULL && children[1] == NULL)
		return;

	// First optimise the children
	if (children[0])
		children[0]->Optimise(&children[0]);

	if (children[1])
		children[1]->Optimise(&children[1]);

	// If there is only one right child, this is a unary operation
	if (children[1] && children[0] == NULL)
	{
		// Can only optimise constants out
		if (children[1]->GetNodeType() == PTNODE_TYPE_CONSTANT)
		{
			// Operation performed on constants? Get out of it!
			if (token.Group() == TOKEN_GROUP_OPERATOR)
			{
				Constant	*node;

				// Allocate some space for the new node with a fake constant
				if ((node = new Constant("1")) == NULL)
					throw CompileError("Couldn't allocate constant structure");

				// Link in the node but don't isolate this one
				node->parent = parent;
				node->AssignLeftChild(NULL);
				node->AssignRightChild(NULL);

				// Set default values
				node->out_type = children[1]->out_type;
				node->in_type.Make(VARIABLE_TYPEID_VOID);

				// Perform the operation
				switch (token.type)
				{
					// --------------------------------------------------------> Unary minus

					case (TOKEN_MINUS):
						switch (children[1]->out_type.id)
						{
							case (VARIABLE_TYPEID_CHAR):
								node->value.c = -((Constant *)children[1])->value.c;
								break;

							case (VARIABLE_TYPEID_SHORT):
								node->value.s = -((Constant *)children[1])->value.s;
								break;

							case (VARIABLE_TYPEID_INT):
								node->value.i = -((Constant *)children[1])->value.i;
								break;

							case (VARIABLE_TYPEID_UCHAR):
								node->value.c = -(char)((Constant *)children[1])->value.uc;
								node->out_type.id = VARIABLE_TYPEID_CHAR;
								break;

							case (VARIABLE_TYPEID_USHORT):
								node->value.s = -(short)((Constant *)children[1])->value.us;
								node->out_type.id = VARIABLE_TYPEID_SHORT;
								break;

							case (VARIABLE_TYPEID_UINT):
								node->value.i = -(int)((Constant *)children[1])->value.ui;
								node->out_type.id = VARIABLE_TYPEID_INT;
								break;

							case (VARIABLE_TYPEID_FLOAT):
								node->value.f = -((Constant *)children[1])->value.f;
								break;
						}
						break;

					// ---------------------------------------------------------> Unary plus

					case (TOKEN_PLUS):
						switch (children[1]->out_type.id)
						{
							case (VARIABLE_TYPEID_CHAR):
								node->value.c = ((Constant *)children[1])->value.c;
								break;

							case (VARIABLE_TYPEID_SHORT):
								node->value.s = ((Constant *)children[1])->value.s;
								break;

							case (VARIABLE_TYPEID_INT):
								node->value.i = ((Constant *)children[1])->value.i;
								break;

							case (VARIABLE_TYPEID_UCHAR):
								node->value.uc = ((Constant *)children[1])->value.uc;
								break;

							case (VARIABLE_TYPEID_USHORT):
								node->value.us = ((Constant *)children[1])->value.us;
								break;

							case (VARIABLE_TYPEID_UINT):
								node->value.ui = ((Constant *)children[1])->value.ui;
								break;

							case (VARIABLE_TYPEID_FLOAT):
								node->value.f = ((Constant *)children[1])->value.f;
								break;
						}
						break;

					// --------------------------------------------------> Unary bitwise NOT

					case (TOKEN_BITWISE_NOT):
						switch (children[1]->out_type.id)
						{
							case (VARIABLE_TYPEID_CHAR):
								node->value.c = ~((Constant *)children[1])->value.c;
								break;

							case (VARIABLE_TYPEID_SHORT):
								node->value.s = ~((Constant *)children[1])->value.s;
								break;

							case (VARIABLE_TYPEID_INT):
								node->value.i = ~((Constant *)children[1])->value.i;
								break;

							case (VARIABLE_TYPEID_UCHAR):
								node->value.uc = ~(char)((Constant *)children[1])->value.uc;
								break;

							case (VARIABLE_TYPEID_USHORT):
								node->value.us = ~(short)((Constant *)children[1])->value.us;
								break;

							case (VARIABLE_TYPEID_UINT):
								node->value.ui = ~(int)((Constant *)children[1])->value.ui;
								break;

							case (VARIABLE_TYPEID_FLOAT):
								throw CompileError("(Line %d) Illegal usage of '~' on floating point constant", token.line);
								break;
						}
						break;

					// --------------------------------------------------> Unary logical NOT

					case (TOKEN_LOGICAL_NOT):
						switch (children[1]->out_type.id)
						{
							case (VARIABLE_TYPEID_CHAR):
								node->value.c = !((Constant *)children[1])->value.c;
								break;

							case (VARIABLE_TYPEID_SHORT):
								node->value.s = !((Constant *)children[1])->value.s;
								break;

							case (VARIABLE_TYPEID_INT):
								node->value.i = !((Constant *)children[1])->value.i;
								break;

							case (VARIABLE_TYPEID_UCHAR):
								node->value.uc = !(char)((Constant *)children[1])->value.uc;
								break;

							case (VARIABLE_TYPEID_USHORT):
								node->value.us = !(short)((Constant *)children[1])->value.us;
								break;

							case (VARIABLE_TYPEID_UINT):
								node->value.ui = !(int)((Constant *)children[1])->value.ui;
								break;

							case (VARIABLE_TYPEID_FLOAT):
								node->value.f = !((Constant *)children[1])->value.f;
								break;
						}
						break;

					// Errors for these two

					case (TOKEN_PLUS_PLUS):
						throw CompileError("(Line %d) Cannot perform '++' on constants", token.line);
						break;

					case (TOKEN_MINUS_MINUS):
						throw CompileError("(Line %d) Cannot perform '--' on constants", token.line);
						break;
				}

				// Need to update the parent's child pointer to point to the newly allocated
				// node, then delete this, making sure not to access any member variables.

				// Argh! Delete THIS!
				delete this;

				// Update where the pointer to this newly created node
				*holder = node;
			}
		}
	}

	// Two children means a normal operation
	else if (children[0] && children[1])
	{
		// Both need to be constants
		if (children[0]->GetNodeType() == PTNODE_TYPE_CONSTANT &&
			children[1]->GetNodeType() == PTNODE_TYPE_CONSTANT)
		{
			// Operation performed on constants? Get out of it!
			if (token.Group() == TOKEN_GROUP_OPERATOR)
			{
				Constant	*node;
				dynamic		a, b;

				// Allocate some space for the new node with a fake constant
				if ((node = new Constant("1")) == NULL)
					throw CompileError("Couldn't allocate constant structure");

				// Link in the node but don't isolate this one
				node->parent = parent;
				node->AssignLeftChild(NULL);
				node->AssignRightChild(NULL);

				// Set default values
				node->out_type.array = 0;
				node->in_type.Make(VARIABLE_TYPEID_VOID);

				// Case where types are the same
				if (children[0]->out_type.id == children[1]->out_type.id)
				{
					node->out_type = children[0]->out_type;

					// Just copy the variables into a and b
					switch (node->out_type.id)
					{
						case (VARIABLE_TYPEID_CHAR):
							a.c = ((Constant *)children[0])->value.c;
							b.c = ((Constant *)children[1])->value.c;
							break;

						case (VARIABLE_TYPEID_SHORT):
							a.s = ((Constant *)children[0])->value.s;
							b.s = ((Constant *)children[1])->value.s;
							break;

						case (VARIABLE_TYPEID_INT):
							a.i = ((Constant *)children[0])->value.i;
							b.i = ((Constant *)children[1])->value.i;
							break;

						case (VARIABLE_TYPEID_UCHAR):
							a.uc = ((Constant *)children[0])->value.uc;
							b.uc = ((Constant *)children[1])->value.uc;
							break;

						case (VARIABLE_TYPEID_USHORT):
							a.us = ((Constant *)children[0])->value.us;
							b.us = ((Constant *)children[1])->value.us;
							break;

						case (VARIABLE_TYPEID_UINT):
							a.ui = ((Constant *)children[0])->value.ui;
							b.ui = ((Constant *)children[1])->value.ui;
							break;

						case (VARIABLE_TYPEID_FLOAT):
							a.f = ((Constant *)children[0])->value.f;
							b.f = ((Constant *)children[1])->value.f;
							break;
					}
				}

				// Case where either one of the children is a floating point value
				else if (children[0]->out_type.id == VARIABLE_TYPEID_FLOAT ||
						 children[1]->out_type.id == VARIABLE_TYPEID_FLOAT)
				{
					node->out_type.Make(VARIABLE_TYPEID_FLOAT);

					// Copy the first
					switch (children[0]->out_type.id)
					{
						case (VARIABLE_TYPEID_CHAR):
							a.f = (float)((Constant *)children[0])->value.c;
							break;

						case (VARIABLE_TYPEID_SHORT):
							a.f = (float)((Constant *)children[0])->value.s;
							break;

						case (VARIABLE_TYPEID_INT):
							a.f = (float)((Constant *)children[0])->value.i;
							break;

						case (VARIABLE_TYPEID_UCHAR):
							a.f = (float)((Constant *)children[0])->value.uc;
							break;

						case (VARIABLE_TYPEID_USHORT):
							a.f = (float)((Constant *)children[0])->value.us;
							break;

						case (VARIABLE_TYPEID_UINT):
							a.f = (float)((Constant *)children[0])->value.ui;
							break;

						case (VARIABLE_TYPEID_FLOAT):
							a.f = (float)((Constant *)children[0])->value.f;
							break;
					}

					// Copy the second
					switch (children[1]->out_type.id)
					{
						case (VARIABLE_TYPEID_CHAR):
							b.f = (float)((Constant *)children[1])->value.c;
							break;

						case (VARIABLE_TYPEID_SHORT):
							b.f = (float)((Constant *)children[1])->value.s;
							break;

						case (VARIABLE_TYPEID_INT):
							b.f = (float)((Constant *)children[1])->value.i;
							break;

						case (VARIABLE_TYPEID_UCHAR):
							b.f = (float)((Constant *)children[1])->value.uc;
							break;

						case (VARIABLE_TYPEID_USHORT):
							b.f = (float)((Constant *)children[1])->value.us;
							break;

						case (VARIABLE_TYPEID_UINT):
							b.f = (float)((Constant *)children[1])->value.ui;
							break;

						case (VARIABLE_TYPEID_FLOAT):
							b.f = ((Constant *)children[1])->value.f;
							break;
					}
				}

				// Typecast everything to an integer
				else
				{
					node->out_type.Make(VARIABLE_TYPEID_INT);

					// Copy the first
					switch (children[0]->out_type.id)
					{
						case (VARIABLE_TYPEID_CHAR):
							a.i = (int)((Constant *)children[0])->value.c;
							break;

						case (VARIABLE_TYPEID_SHORT):
							a.i = (int)((Constant *)children[0])->value.s;
							break;

						case (VARIABLE_TYPEID_INT):
							a.i = (int)((Constant *)children[0])->value.i;
							break;

						case (VARIABLE_TYPEID_UCHAR):
							a.i = (int)((Constant *)children[0])->value.uc;
							break;

						case (VARIABLE_TYPEID_USHORT):
							a.i = (int)((Constant *)children[0])->value.us;
							break;

						case (VARIABLE_TYPEID_UINT):
							a.i = (int)((Constant *)children[0])->value.ui;
							break;
					}

					// Copy the last
					switch (children[1]->out_type.id)
					{
						case (VARIABLE_TYPEID_CHAR):
							b.i = (int)((Constant *)children[1])->value.c;
							break;

						case (VARIABLE_TYPEID_SHORT):
							b.i = (int)((Constant *)children[1])->value.s;
							break;

						case (VARIABLE_TYPEID_INT):
							b.i = (int)((Constant *)children[1])->value.i;
							break;

						case (VARIABLE_TYPEID_UCHAR):
							b.i = (int)((Constant *)children[1])->value.uc;
							break;

						case (VARIABLE_TYPEID_USHORT):
							b.i = (int)((Constant *)children[1])->value.us;
							break;

						case (VARIABLE_TYPEID_UINT):
							b.i = (int)((Constant *)children[1])->value.ui;
							break;
					}
				}

				switch (token.type)
				{
					// -----------------------------------------------------------> Addition

					case (TOKEN_PLUS):
						switch (node->out_type.id)
						{
							case (VARIABLE_TYPEID_CHAR):
								((Constant *)node)->value.c = a.c + b.c;
								break;

							case (VARIABLE_TYPEID_SHORT):
								((Constant *)node)->value.s = a.s + b.s;
								break;

							case (VARIABLE_TYPEID_INT):
								((Constant *)node)->value.i = a.i + b.i;
								break;

							case (VARIABLE_TYPEID_UCHAR):
								((Constant *)node)->value.uc = a.uc + b.uc;
								break;

							case (VARIABLE_TYPEID_USHORT):
								((Constant *)node)->value.us = a.us + b.us;
								break;

							case (VARIABLE_TYPEID_UINT):
								((Constant *)node)->value.ui = a.ui + b.ui;
								break;

							case (VARIABLE_TYPEID_FLOAT):
								((Constant *)node)->value.f = a.f + b.f;
								break;
						}
						break;

					// --------------------------------------------------------> Subtraction

					case (TOKEN_MINUS):
						switch (node->out_type.id)
						{
							case (VARIABLE_TYPEID_CHAR):
								((Constant *)node)->value.c = a.c - b.c;
								break;

							case (VARIABLE_TYPEID_SHORT):
								((Constant *)node)->value.s = a.s - b.s;
								break;

							case (VARIABLE_TYPEID_INT):
								((Constant *)node)->value.i = a.i - b.i;
								break;

							case (VARIABLE_TYPEID_UCHAR):
								((Constant *)node)->value.uc = a.uc - b.uc;
								break;

							case (VARIABLE_TYPEID_USHORT):
								((Constant *)node)->value.us = a.us - b.us;
								break;

							case (VARIABLE_TYPEID_UINT):
								((Constant *)node)->value.ui = a.ui - b.ui;
								break;

							case (VARIABLE_TYPEID_FLOAT):
								((Constant *)node)->value.f = a.f - b.f;
								break;
						}
						break;

					// -----------------------------------------------------------> Division

					case (TOKEN_DIVIDE):
						switch (node->out_type.id)
						{
							case (VARIABLE_TYPEID_CHAR):
								((Constant *)node)->value.c = a.c / b.c;
								break;

							case (VARIABLE_TYPEID_SHORT):
								((Constant *)node)->value.s = a.s / b.s;
								break;

							case (VARIABLE_TYPEID_INT):
								((Constant *)node)->value.i = a.i / b.i;
								break;

							case (VARIABLE_TYPEID_UCHAR):
								((Constant *)node)->value.uc = a.uc / b.uc;
								break;

							case (VARIABLE_TYPEID_USHORT):
								((Constant *)node)->value.us = a.us / b.us;
								break;

							case (VARIABLE_TYPEID_UINT):
								((Constant *)node)->value.ui = a.ui / b.ui;
								break;

							case (VARIABLE_TYPEID_FLOAT):
								((Constant *)node)->value.f = a.f / b.f;
								break;
						}
						break;

					// -----------------------------------------------------> Multiplication

					case (TOKEN_MULTIPLY):
						switch (node->out_type.id)
						{
							case (VARIABLE_TYPEID_CHAR):
								((Constant *)node)->value.c = a.c * b.c;
								break;

							case (VARIABLE_TYPEID_SHORT):
								((Constant *)node)->value.s = a.s * b.s;
								break;

							case (VARIABLE_TYPEID_INT):
								((Constant *)node)->value.i = a.i * b.i;
								break;

							case (VARIABLE_TYPEID_UCHAR):
								((Constant *)node)->value.uc = a.uc * b.uc;
								break;

							case (VARIABLE_TYPEID_USHORT):
								((Constant *)node)->value.us = a.us * b.us;
								break;

							case (VARIABLE_TYPEID_UINT):
								((Constant *)node)->value.ui = a.ui * b.ui;
								break;

							case (VARIABLE_TYPEID_FLOAT):
								((Constant *)node)->value.f = a.f * b.f;
								break;
						}
						break;

					// ------------------------------------------------------------> Modulus

					case (TOKEN_MODULUS):
						switch (node->out_type.id)
						{
							case (VARIABLE_TYPEID_CHAR):
								((Constant *)node)->value.c = a.c % b.c;
								break;

							case (VARIABLE_TYPEID_SHORT):
								((Constant *)node)->value.s = a.s % b.s;
								break;

							case (VARIABLE_TYPEID_INT):
								((Constant *)node)->value.i = a.i % b.i;
								break;

							case (VARIABLE_TYPEID_UCHAR):
								((Constant *)node)->value.uc = a.uc % b.uc;
								break;

							case (VARIABLE_TYPEID_USHORT):
								((Constant *)node)->value.us = a.us % b.us;
								break;

							case (VARIABLE_TYPEID_UINT):
								((Constant *)node)->value.ui = a.ui % b.ui;
								break;

							case (VARIABLE_TYPEID_FLOAT):
								throw CompileError("(Line %d) Illegal usage of '%' with floating point constant", token.line);
								break;
						}
						break;

					// --------------------------------------------------------> Bitwise AND

					case (TOKEN_BITWISE_AND):
						switch (node->out_type.id)
						{
							case (VARIABLE_TYPEID_CHAR):
								((Constant *)node)->value.c = a.c & b.c;
								break;

							case (VARIABLE_TYPEID_SHORT):
								((Constant *)node)->value.s = a.s & b.s;
								break;

							case (VARIABLE_TYPEID_INT):
								((Constant *)node)->value.i = a.i & b.i;
								break;

							case (VARIABLE_TYPEID_UCHAR):
								((Constant *)node)->value.uc = a.uc & b.uc;
								break;

							case (VARIABLE_TYPEID_USHORT):
								((Constant *)node)->value.us = a.us & b.us;
								break;

							case (VARIABLE_TYPEID_UINT):
								((Constant *)node)->value.ui = a.ui & b.ui;
								break;

							case (VARIABLE_TYPEID_FLOAT):
								throw CompileError("(Line %d) Illegal usage of '&' with floating point constant", token.line);
								break;
						}
						break;

					// ---------------------------------------------------------> Bitwise OR

					case (TOKEN_BITWISE_OR):
						switch (node->out_type.id)
						{
							case (VARIABLE_TYPEID_CHAR):
								((Constant *)node)->value.c = a.c | b.c;
								break;

							case (VARIABLE_TYPEID_SHORT):
								((Constant *)node)->value.s = a.s | b.s;
								break;

							case (VARIABLE_TYPEID_INT):
								((Constant *)node)->value.i = a.i | b.i;
								break;

							case (VARIABLE_TYPEID_UCHAR):
								((Constant *)node)->value.uc = a.uc | b.uc;
								break;

							case (VARIABLE_TYPEID_USHORT):
								((Constant *)node)->value.us = a.us | b.us;
								break;

							case (VARIABLE_TYPEID_UINT):
								((Constant *)node)->value.ui = a.ui | b.ui;
								break;

							case (VARIABLE_TYPEID_FLOAT):
								throw CompileError("(Line %d) Illegal usage of '|' with floating point constant", token.line);
								break;
						}
						break;

					// --------------------------------------------------------> Bitwise XOR

					case (TOKEN_BITWISE_XOR):
						switch (node->out_type.id)
						{
							case (VARIABLE_TYPEID_CHAR):
								((Constant *)node)->value.c = a.c ^ b.c;
								break;

							case (VARIABLE_TYPEID_SHORT):
								((Constant *)node)->value.s = a.s ^ b.s;
								break;

							case (VARIABLE_TYPEID_INT):
								((Constant *)node)->value.i = a.i ^ b.i;
								break;

							case (VARIABLE_TYPEID_UCHAR):
								((Constant *)node)->value.uc = a.uc ^ b.uc;
								break;

							case (VARIABLE_TYPEID_USHORT):
								((Constant *)node)->value.us = a.us ^ b.us;
								break;

							case (VARIABLE_TYPEID_UINT):
								((Constant *)node)->value.ui = a.ui ^ b.ui;
								break;

							case (VARIABLE_TYPEID_FLOAT):
								throw CompileError("(Line %d) Illegal usage of '^' with floating point constant", token.line);
								break;
						}
						break;

					// ---------------------------------------------------------> Shift left

					case (TOKEN_SHIFT_LEFT):
						switch (node->out_type.id)
						{
							case (VARIABLE_TYPEID_CHAR):
								((Constant *)node)->value.c = a.c << b.c;
								break;

							case (VARIABLE_TYPEID_SHORT):
								((Constant *)node)->value.s = a.s << b.s;
								break;

							case (VARIABLE_TYPEID_INT):
								((Constant *)node)->value.i = a.i << b.i;
								break;

							case (VARIABLE_TYPEID_UCHAR):
								((Constant *)node)->value.uc = a.uc << b.uc;
								break;

							case (VARIABLE_TYPEID_USHORT):
								((Constant *)node)->value.us = a.us << b.us;
								break;

							case (VARIABLE_TYPEID_UINT):
								((Constant *)node)->value.ui = a.ui << b.ui;
								break;

							case (VARIABLE_TYPEID_FLOAT):
								throw CompileError("(Line %d) Illegal usage of '<<' with floating point constant", token.line);
								break;
						}
						break;

					// --------------------------------------------------------> Shift right

					case (TOKEN_SHIFT_RIGHT):
						switch (node->out_type.id)
						{
							case (VARIABLE_TYPEID_CHAR):
								((Constant *)node)->value.c = a.c >> b.c;
								break;

							case (VARIABLE_TYPEID_SHORT):
								((Constant *)node)->value.s = a.s >> b.s;
								break;

							case (VARIABLE_TYPEID_INT):
								((Constant *)node)->value.i = a.i >> b.i;
								break;

							case (VARIABLE_TYPEID_UCHAR):
								((Constant *)node)->value.uc = a.uc >> b.uc;
								break;

							case (VARIABLE_TYPEID_USHORT):
								((Constant *)node)->value.us = a.us >> b.us;
								break;

							case (VARIABLE_TYPEID_UINT):
								((Constant *)node)->value.ui = a.ui >> b.ui;
								break;

							case (VARIABLE_TYPEID_FLOAT):
								throw CompileError("(Line %d) Illegal usage of '>>' with floating point constant", token.line);
								break;
						}
						break;

					// -----------------------------------------------> Less than comparison

					case (TOKEN_LESS):
						switch (node->out_type.id)
						{
							case (VARIABLE_TYPEID_CHAR):
								((Constant *)node)->value.i = a.c < b.c;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_SHORT):
								((Constant *)node)->value.i = a.s < b.s;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_INT):
								((Constant *)node)->value.i = a.i < b.i;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_UCHAR):
								((Constant *)node)->value.i = a.uc < b.uc;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_USHORT):
								((Constant *)node)->value.i = a.us < b.us;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_UINT):
								((Constant *)node)->value.i = a.ui < b.ui;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_FLOAT):
								((Constant *)node)->value.i = a.f < b.f;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;
						}
						break;

					// -----------------------------------> Less than or equal to comparison

					case (TOKEN_LESS_EQUAL):
						switch (node->out_type.id)
						{
							case (VARIABLE_TYPEID_CHAR):
								((Constant *)node)->value.i = a.c <= b.c;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_SHORT):
								((Constant *)node)->value.i = a.s <= b.s;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_INT):
								((Constant *)node)->value.i = a.i <= b.i;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_UCHAR):
								((Constant *)node)->value.i = a.uc <= b.uc;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_USHORT):
								((Constant *)node)->value.i = a.us <= b.us;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_UINT):
								((Constant *)node)->value.i = a.ui <= b.ui;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_FLOAT):
								((Constant *)node)->value.i = a.f <= b.f;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;
						}
						break;

					// --------------------------------------------> Greater than comparison

					case (TOKEN_GREATER):
						switch (node->out_type.id)
						{
							case (VARIABLE_TYPEID_CHAR):
								((Constant *)node)->value.i = a.c > b.c;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_SHORT):
								((Constant *)node)->value.i = a.s > b.s;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_INT):
								((Constant *)node)->value.i = a.i > b.i;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_UCHAR):
								((Constant *)node)->value.i = a.uc > b.uc;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_USHORT):
								((Constant *)node)->value.i = a.us > b.us;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_UINT):
								((Constant *)node)->value.i = a.ui > b.ui;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_FLOAT):
								((Constant *)node)->value.i = a.f > b.f;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;
						}
						break;

					// --------------------------------> Greater than or equal to comparison

					case (TOKEN_GREATER_EQUAL):
						switch (node->out_type.id)
						{
							case (VARIABLE_TYPEID_CHAR):
								((Constant *)node)->value.i = a.c >= b.c;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_SHORT):
								((Constant *)node)->value.i = a.s >= b.s;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_INT):
								((Constant *)node)->value.i = a.i >= b.i;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_UCHAR):
								((Constant *)node)->value.i = a.uc >= b.uc;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_USHORT):
								((Constant *)node)->value.i = a.us >= b.us;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_UINT):
								((Constant *)node)->value.i = a.ui >= b.ui;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_FLOAT):
								((Constant *)node)->value.i = a.f >= b.f;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;
						}
						break;

					// ------------------------------------------------> Equal to comparison

					case (TOKEN_EQUAL):
						switch (node->out_type.id)
						{
							case (VARIABLE_TYPEID_CHAR):
								((Constant *)node)->value.i = a.c == b.c;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_SHORT):
								((Constant *)node)->value.i = a.s == b.s;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_INT):
								((Constant *)node)->value.i = a.i == b.i;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_UCHAR):
								((Constant *)node)->value.i = a.uc == b.uc;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_USHORT):
								((Constant *)node)->value.i = a.us == b.us;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_UINT):
								((Constant *)node)->value.i = a.ui == b.ui;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_FLOAT):
								((Constant *)node)->value.i = a.f == b.f;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;
						}
						break;

					// --------------------------------------------> Not equal to comparison

					case (TOKEN_LOGICAL_NOT_EQUAL):
						switch (node->out_type.id)
						{
							case (VARIABLE_TYPEID_CHAR):
								((Constant *)node)->value.i = a.c != b.c;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_SHORT):
								((Constant *)node)->value.i = a.s != b.s;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_INT):
								((Constant *)node)->value.i = a.i != b.i;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_UCHAR):
								((Constant *)node)->value.i = a.uc != b.uc;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_USHORT):
								((Constant *)node)->value.i = a.us != b.us;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_UINT):
								((Constant *)node)->value.i = a.ui != b.ui;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_FLOAT):
								((Constant *)node)->value.i = a.f != b.f;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;
						}
						break;

					// --------------------------------------------------------> Logical AND

					case (TOKEN_LOGICAL_AND):
						switch (node->out_type.id)
						{
							case (VARIABLE_TYPEID_CHAR):
								((Constant *)node)->value.i = a.c && b.c;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_SHORT):
								((Constant *)node)->value.i = a.s && b.s;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_INT):
								((Constant *)node)->value.i = a.i && b.i;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_UCHAR):
								((Constant *)node)->value.i = a.uc && b.uc;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_USHORT):
								((Constant *)node)->value.i = a.us && b.us;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_UINT):
								((Constant *)node)->value.i = a.ui && b.ui;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_FLOAT):
								((Constant *)node)->value.i = a.f && b.f;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;
						}
						break;

					// ---------------------------------------------------------> Logical OR

					case (TOKEN_LOGICAL_OR):
						switch (node->out_type.id)
						{
							case (VARIABLE_TYPEID_CHAR):
								((Constant *)node)->value.i = a.c || b.c;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_SHORT):
								((Constant *)node)->value.i = a.s || b.s;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_INT):
								((Constant *)node)->value.i = a.i || b.i;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_UCHAR):
								((Constant *)node)->value.i = a.uc || b.uc;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_USHORT):
								((Constant *)node)->value.i = a.us || b.us;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_UINT):
								((Constant *)node)->value.i = a.ui || b.ui;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;

							case (VARIABLE_TYPEID_FLOAT):
								((Constant *)node)->value.i = a.f || b.f;
								node->out_type.Make(VARIABLE_TYPEID_INT);
								break;
						}
						break;
				}

				// Argh! Delete THIS!
				delete this;

				// Update where the pointer to this newly created node
				*holder = node;
			}
		}
	}

	outfunc;
}


void ParseTreeNode::ResolveType(int flags)
{
	infunc(ParseTreeNode::ResolveType);

	VarType	ctype[2];

	// Nothing to do here
	if (children[0] == NULL && children[1] == NULL)
		return;

	// Resolve the child types

	if (children[0])
	{
		children[0]->ResolveType(flags);
		ctype[0] = children[0]->out_type;
	}

	if (children[1])
	{
		children[1]->ResolveType(flags);
		ctype[1] = children[1]->out_type;
	}

	// Check for operators modifying void types anywhere
	if ((children[0] && ctype[0].id == VARIABLE_TYPEID_VOID) ||
		(children[1] && ctype[1].id == VARIABLE_TYPEID_VOID))
		throw CompileError("(Line %d) Illegal use of 'void' type", token.line);

	/* -----------------------

	This case is an operation node which has two children with which to derive it's result.
	If the operation is an assignment then the resultant type should be inherited from the
	left child (the variable being assigned to).

	Standard types can be mixed with each other. Standard types cannot be mixed with
	custom types unless operator overloads are defined. When custom types are mixed,
	they have to be the same types unless operator overloads are defined. Arrays and
	non-arrays cannot be mixed.

	One special case is with assignment where a custom type can be assigned a type which
	derives from that type.

	When standard types are mixed, the rules for deriving the resultant type are...

		float + anything				= float
		a (not float) + b (not float)	= int
	
	Relational operators will always produce an integer result.

	When custom types are used in expressions (like a + b), overloaded operators can be
	allowed. If the types are the same then everything should be fine. If the types are
	different, however, things get a little different. For the expression to be valid one
	type has to inherit from the other. If both types have overloaded operators defined
	then the one which takes effect is the most recent one (the child). If a parent has
	a defined operator and one of the arguments is a child, this is legal. If a child
	has a defined operator and one of the arugments is a parent, this is illegal.

	String variables are present now. These are just custom objects that the compiler
	knows about so that string literals can be handled nicely.

	----------------------- */

	if (children[0] && children[1])
	{
		if (token.Is(TOKEN_TYPE_ASSIGNMENT))
		{
			// Check for illegal array operations
			if (ctype[0].array != ctype[1].array)
				throw CompileError("(Line %d) Cannot operate on mixed array/non-array variables", token.line);

			// Arrays of the same size must be operated on
			if (ctype[0].array && ctype[1].array)
			{
				if (ctype[0].elements != ctype[1].elements)
					throw CompileError("(Line %d) Elements in arrays do not match", token.line);
			}

			// Check for constant assignment
			if (children[0]->GetNodeType() == PTNODE_TYPE_CONSTANT)
				throw CompileError("(Line %d) Cannot assign constants values", token.line);

			// Are both types custom?
			if (ctype[0].id == VARIABLE_TYPEID_CUSTOM &&
				ctype[1].id == VARIABLE_TYPEID_CUSTOM)
			{
				// If the types are not the same
				if (strcmp(ctype[0].name, ctype[1].name))
				{
					Class	*class_ptr;

					// This should never (for now) be NULL
					class_ptr = g_Env->GetClass(ctype[1].name);

					// Can only assign super-class = class, not class = super-class
					if (!class_ptr->HasSuperClass(ctype[0].name))
						throw CompileError("(Line %d) Incompatible types", token.line);
				}
			}

			// Is one of the types a string?
			else if (ctype[0].id == VARIABLE_TYPEID_STRING ||
					 ctype[1].id == VARIABLE_TYPEID_STRING)
			{
				// For now, both have to be strings
				if (ctype[0].id != ctype[1].id)
					throw CompileError("(Line %d) Incompatible types", token.line);
			}

			else
			{
				// Mixed custom/non-custom types
				if (ctype[0].id == VARIABLE_TYPEID_CUSTOM ||
					ctype[1].id == VARIABLE_TYPEID_CUSTOM)
					throw CompileError("(Line %d) Incompatible types", token.line);
			}

			in_type = ctype[0];
		}

		else if (token.Is(TOKEN_TYPE_OPERATOR | TTNOT(TOKEN_TYPE_BRACKETOPS)))
		{
			// Check for illegal array operations
			if (ctype[0].array != ctype[1].array)
				throw CompileError("(Line %d) Cannot operate on mixed array/non-array variables", token.line);

			// For now, operations on arrays are not possible
			if (ctype[0].array || ctype[1].array)
				throw CompileError("(Line %d) Illegal operation on array type", token.line);

			// Are both types custom?
			if (ctype[0].id == VARIABLE_TYPEID_CUSTOM &&
				ctype[1].id == VARIABLE_TYPEID_CUSTOM)
			{
				// Undefined for now
				throw CompileError("(Line %d) Illegal operation on custom types", token.line);
			}

			else if (ctype[0].id == VARIABLE_TYPEID_STRING ||
					 ctype[1].id == VARIABLE_TYPEID_STRING)
			{
				// Undefined now
				throw CompileError("(Line %d) Illegal operation on string types", token.line);
			}

			else
			{
				// Mixed custom/non-custom types
				if (ctype[0].id == VARIABLE_TYPEID_CUSTOM ||
					ctype[1].id == VARIABLE_TYPEID_CUSTOM)
					throw CompileError("(Line %d) Incompatible types", token.line);

				// Normal types get this far

				// One is floating point
				if (ctype[0].id == VARIABLE_TYPEID_FLOAT ||
					ctype[1].id == VARIABLE_TYPEID_FLOAT)
					in_type.Make(VARIABLE_TYPEID_FLOAT);

				// Different but not float, type integer
				else
					in_type.Make(VARIABLE_TYPEID_INT);
			}

			// Check for mis-use of integer operations
			switch (token.type)
			{
				case (TOKEN_BITWISE_AND):
				case (TOKEN_BITWISE_OR):
				case (TOKEN_BITWISE_XOR):
				case (TOKEN_SHIFT_LEFT):
				case (TOKEN_SHIFT_RIGHT):
				case (TOKEN_BITWISE_AND_ASSIGN):
				case (TOKEN_BITWISE_OR_ASSIGN):
				case (TOKEN_BITWISE_XOR_ASSIGN):
				case (TOKEN_SHL_ASSIGN):
				case (TOKEN_SHR_ASSIGN):
					if (ctype[0].id == VARIABLE_TYPEID_FLOAT || ctype[1].id == VARIABLE_TYPEID_FLOAT)
						throw CompileError("(Line %d) Illegal use of 'float' type with integer operator", token.line);
					break;
			}
		}

		else
		{
			// Resolve types depending upon the type of token here
			switch (token.type)
			{
				case (TOKEN_ARRAY_OPEN):
					// Can't have floating point indices
					if (ctype[1].id == VARIABLE_TYPEID_FLOAT)
						throw CompileError("(Line %d) Array cannot have a floating point index", token.line);

					// Cannot look-up that which is not an array!
					if (ctype[0].array == 0)
						throw CompileError("(Line %d) Type not an array", children[0]->token.line);

					// Copy the left child attributes, it's no longer an array
					in_type = ctype[0];
					in_type.array = 0;
					in_type.elements = 0;
					break;

				default:
					throw CompileError("(Line %d) YOU WERE LOOKING FOR THIS!", token.line);
					break;
			}
		}
	}

	/* ---------

	The unary operations are +, -, ~, !. The + operator should have no effect on anything.
	The - operator shouldn't have an effect on the type unless the type is unsigned. In
	this case, the type needs changing to signed. The ~ operator should have no effect
	on anything. The ! operator should result simply in an integer.

	If unary operations are performed on custom types, unless the type has that operator
	overloaded, the case is an error. The same applies for strings.

	The final case to check for is unary operations on arrays (not members in an array).
	This should not be allowed (unless some form of array-specific operator overloading
	is performed in the future).

	--------- */

	else if (children[0] == NULL && children[1])
	{
		// Unary negation
		if (token.type == TOKEN_MINUS)
		{
			in_type = ctype[1];

			if (ctype[1].id & VARIABLE_TYPEID_UNSIGNED)
				in_type.id = in_type.id & (~VARIABLE_TYPEID_UNSIGNED);
		}

		// Unary logical not
		else if (token.type == TOKEN_LOGICAL_NOT)
		{
			// Results in an integer
			in_type.Make(VARIABLE_TYPEID_INT);
		}
		
		// Plus, bitwise not and pre-effect operators
		else
		{
			// Just copy it
			in_type = ctype[1];
		}

		// No unary on custom types
		if (ctype[1].id == VARIABLE_TYPEID_CUSTOM)
			throw CompileError("(Line %d) Illegal operation on custom type", token.line);

		// No unary on string types
		if (ctype[1].id == VARIABLE_TYPEID_STRING)
			throw CompileError("(Line %d) Illegal operation on string type", token.line);

		// No unary on un-indexed arrays
		if (ctype[1].array)
			throw CompileError("(Line %d) Illegal operation on array type", token.line);

		if (token.type == TOKEN_PLUS_PLUS || token.type == TOKEN_MINUS_MINUS)
		{
			// ++, -- Working on more than one node
			if ((children[1]->children[0] || children[1]->children[1]) &&
				(children[1]->token.type != TOKEN_ARRAY_OPEN))
				throw CompileError("(Line %d) Operator only works on single variables", token.line);
		}
	}

	/* ----

	The remaining possible operations are post increment and decrement. These do not
	modify types so the current node should inherit the type of the left child. Any custom
	types should generate an error if there is no overloaded operator defined for them.
	This is also the case for strings.

	---- */

	else
	{
		// Inherit the left child's type
		in_type = ctype[0];

		// ++, -- On consntants
		if (children[0]->GetNodeType() == PTNODE_TYPE_CONSTANT)
			throw CompileError("(Line %d) Illegal operation on constant", token.line);

		// ++, -- Working on more than one node
		if ((children[0]->children[0] || children[0]->children[1]) &&
			(children[0]->token.type != TOKEN_ARRAY_OPEN))
			throw CompileError("(Line %d) Operator only works on single variables", token.line);

		// Custom type operation
		if (in_type.id == VARIABLE_TYPEID_CUSTOM)
			throw CompileError("(Line %d) Illegal operation on custom type", token.line);

		// String type operation
		if (in_type.id == VARIABLE_TYPEID_STRING)
			throw CompileError("(Line %d) Illegal operation on string type", token.line);

		// Un-indexed array
		if (ctype[0].array)
			throw CompileError("(Line %d) Illegal operation on array type", token.line);
	}

	// Relational operators always output an integer
	if (token.Is(TOKEN_TYPE_RELOPS))
		out_type.Make(VARIABLE_TYPEID_INT);

	// Branching operators need a true/false label to jump to
	else if (token.Is(TOKEN_TYPE_BRANCHOPS))
		out_type.Make(VARIABLE_TYPEID_BINARY);

	// By default, operators churn out what they take in
	else
		out_type = in_type;

	outfunc;
}


int ParseTreeNode::GetNodeType(void)
{
	infunc(ParseTreeNode::GetNodeType);

	return (PTNODE_TYPE_NORMAL);

	outfunc;
}


void ParseTreeNode::WriteConverter(int from, int to)
{
	infunc(ParseTreeNode::WriteConverter);

	// From anything to floating point
	if (from != VARIABLE_TYPEID_FLOAT && to == VARIABLE_TYPEID_FLOAT)
		g_Object->WriteOp(OPCODE_ITOF);

	// From floating point to anything (saves doing "ftoi ; itoc" etc)
	else if (from == VARIABLE_TYPEID_FLOAT && to != VARIABLE_TYPEID_FLOAT)
	{
		switch (to)
		{
			case (VARIABLE_TYPEID_CHAR): g_Object->WriteOp(OPCODE_FTOC); break;
			case (VARIABLE_TYPEID_UCHAR): g_Object->WriteOp(OPCODE_FTOUC); break;
			case (VARIABLE_TYPEID_SHORT): g_Object->WriteOp(OPCODE_FTOS); break;
			case (VARIABLE_TYPEID_USHORT): g_Object->WriteOp(OPCODE_FTOUS); break;
			case (VARIABLE_TYPEID_INT):
			case (VARIABLE_TYPEID_UINT): g_Object->WriteOp(OPCODE_FTOI); break;
		}
	}

	// From integer to another type of integer
	else if (from & VARIABLE_TYPEID_INT && to != VARIABLE_TYPEID_FLOAT)
	{
		switch (to)
		{
			case (VARIABLE_TYPEID_CHAR): g_Object->WriteOp(OPCODE_ITOC); break;
			case (VARIABLE_TYPEID_UCHAR): g_Object->WriteOp(OPCODE_ITOUC); break;
			case (VARIABLE_TYPEID_SHORT): g_Object->WriteOp(OPCODE_ITOS); break;
			case (VARIABLE_TYPEID_USHORT): g_Object->WriteOp(OPCODE_ITOUS); break;
		}
	}

	outfunc;
}


void ParseTreeNode::DebugPrint(int indent, char *data)
{
	infunc(ParseTreeNode::DebugPrint);

	char	token_string[MAX_LABEL_SIZE];
	int		x;

	// Process the children first

	if (children[0])
		children[0]->DebugPrint(indent, data);

	if (children[1])
		children[1]->DebugPrint(indent, data);

	// Copy the string
	tokeniser->IsolateString(token, token_string);

	// Print the indent
	for (x = 0; x < indent; x++)
		printf(" ");

	// Now the token
	printf("%s\n", token_string);

	outfunc;
}