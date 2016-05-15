
#ifndef	_INCLUDED_VMOFILE_H
#define	_INCLUDED_VMOFILE_H


typedef struct bpi_s
{
	// Position in the code where the item is located
	int		position;

	// Label given to item type
	int		label;

	// Item level
	int		level;

	// Links to others
	bpi_s	*next;
	bpi_s	*prev;
} BackpatchItem;


class VMOFile
{
public:
	// Constructor
	VMOFile(const char *_filename);

	// Destructor
	~VMOFile(void);

	// Write an opcode to the file
	void	WriteOp(OpcodeType opcode, ...);

	// Get the position of the code writer
	int		GetPosition(void);

	// Add a backpatch item to the list
	void	AddBackpatchItem(int label);

	// Back-patch a list of items with the current position
	void	UpdateItems(int label, int address = -1);

	// Trace back and modify a section of code
	void	BackpatchInt(int value, int where);

	// Write the program to stdout
	void	Disassemble(void);

	// After compiling, write all the class information out to file
	void	WriteClassInfo(Class *class_ptr);

	int		add_level;

private:
	// Handle to the open file for writing
	CFile	*file;

	// Name of the object file
	char	filename[PATH_SIZE];

	// List of back-patches to be made
	TLinkedList<BackpatchItem>	bpitems;
};


#endif	/* _INCLUDED_VMOFILE_H */