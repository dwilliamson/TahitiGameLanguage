
#ifndef	_INCLUDED_CFILE_H
#define	_INCLUDED_CFILE_H


class CFile
{
public:
	// Constructor
	CFile(const char *filename, int type);

	// Destructor
	~CFile(void);

	// Write a block of data to file
	int			Write(void *data, int nb_bytes);

	// Read a block of data from file
	int			Read(void *data, int nb_bytes);

	// Write a byte to file
	int			WriteByte(int byte);

	// Read a byte from file
	int			ReadByte(void);

	// Write a byte a certain number of times
	int			WriteByteLoop(int byte, int nb_times);

	// Write a word a certain number of times
	int			WriteWordLoop(int word, int nb_times);

	// Write a dword a certain number of times
	int			WriteDWordLoop(int dword, int nb_times);

	// Write data at a certain position in the file, saving the current
	int			WriteAt(int location, void *data, int nb_bytes);

	// Write a formatted string to file
	int			Printf(const char *format, ...);

	// Write a formatted string to file as well as the NULL terminator
	int			PrintfZ(const char *format, ...);

	// Read a NULL terminated string
	int			ReadStringZ(char *dest);

	// Seek to a certain position in the file
	int			SeekTo(int distance, int where);

	// Get the position of the file pointer
	int			GetPosition(void);

	// Get the size of the file
	int			GetSize(void);

	// Close the file
	void		Close(void);

	// Get one of the file's associated times
	int			GetTime(CTime &time, int which);

	// Set one of the file's associated times
	int			SetTime(CTime &time, int which);

private:
	// Private data that's needed by the platform
	void		*user_data;
};


#endif	/* _INCLUDED_CFILE_H */