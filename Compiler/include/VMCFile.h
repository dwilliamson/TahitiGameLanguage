
#ifndef	_INCLUDED_VMCFILE_H
#define	_INCLUDED_VMCFILE_H


// Utility macros
#define CUR_TOKEN		(tokeniser->token_list(tokeniser->token_pos))			// Current token
#define NEXT_TOKEN		(tokeniser->token_list(tokeniser->token_pos + 1))		// Next token
#define PREV_TOKEN		(tokeniser->token_list(tokeniser->token_pos - 1))		// Previous token
#define INC_TOKEN		(tokeniser->IncToken())


// Flags used during parsing
#define FLAGS_EXPECTING_REFERENCE		0x00001
#define FLAGS_EXPLICIT_ASSIGNMENT		0x00002		// Like v = x ('x' is in explicit assignment)
#define FLAGS_IMPLICIT_ASSIGNMENT		0x00004		// Like v[x] ('x' is in implicit assignment)
#define FLAGS_ASSIGNMENT				0x00006		// Combination of the two
#define FLAGS_IN_CLASS					0x00008
#define	FLAGS_IN_ARRAY					0x00010
#define FLAGS_IN_STATE					0x00020
#define FLAGS_IN_FUNCTION				0x00040
#define FLAGS_PUSH_ADDRESS				0x00080		// Push variable addresses instead of values
#define FLAGS_ASSIGN_DEST				0x00100		// The left side of an assignment (=)
#define FLAGS_ASSIGNOP_DEST				0x00200		// The left side of an operator assignment (+=)
#define FLAGS_INTO_BINARYANDOP			0x00400
#define FLAGS_INTO_BINARYOROP			0x00800
#define FLAGS_INTO_BINARYOP				0x00C00		// The result of this node is heading into a binary operator (&&, ||)
#define FLAGS_FUNCTION_FINAL			0x01000
#define FLAGS_FUNCTION_IMPORT			0x02000
#define	FLAGS_IN_INTERFACE				0x04000
#define FLAGS_ALLOW_LINE_BLOCKS			0x08000
#define FLAGS_IGNORE_END_TOKEN			0x10000
#define FLAGS_ALLOW_BREAK				0x20000
#define FLAGS_ALLOW_CONTINUE			0x40000
#define FLAGS_ALLOW_BREAK_CONTINUE		0x60000


class VMCFile
{
public:
	// Constructor
	VMCFile(const char *_filename, const char *object_file);

	// Destructor
	~VMCFile(void);

	// Compile the current VMC file
	int				Compile(void);

	// Name of the file
	char			filename[PATH_SIZE];

	// Size of the file
	int				filesize;

	// Pointer to the data as read from file
	char			*data;

	// The output object file
	VMOFile			*object;

private:
	Tokeniser		*tokeniser;
	CodeGenerator	*code_gen;
};


#endif	/* _INCLUDED_VMCFILE_H */