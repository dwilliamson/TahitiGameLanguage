
#ifndef	_INCLUDED_CFILESYSTEM_H
#define	_INCLUDED_CFILESYSTEM_H


// NICE!
#undef SetCurrentDirectory


// Types of file time
#define	FILETIME_CREATION		1
#define	FILETIME_LASTACCESS		2
#define	FILETIME_LASTWRITE		3


// Type of access when opening files
#define FILEOPEN_READ			1
#define	FILEOPEN_WRITE			2
#define FILEOPEN_APPEND			4


// Types of file seek
#define	FILESEEK_START			1
#define	FILESEEK_CURRENT		2
#define	FILESEEK_END			3


#define FILETYPE_FILE			1
#define FILETYPE_FOLDER			2


#define	FILE_EOF				-1


#define PATH_SIZE				300


class CFileSystem
{
public:
	// Constructor which is just plain old HD
	CFileSystem(void);

	// Constructor which allows you to specify the root directory (or file)
	CFileSystem(const char *root);

	// Get a time associated with the file
	int		GetFileTime(const char *filename, CTime &time, int which);


	// Set a time associated with the file
	int		SetFileTime(const char *filename, CTime &time, int which);

	// Get the size of a file
	int		GetFileSize(const char *filename);

	// Open a file under direction of the file system
	CFile	*OpenFile(const char *filename, int type);

	// Set the current directory
	void	SetCurrentDirectory(const char *filename);

	// Get the current directory
	void	GetCurrentDirectory(char *dest, int buflen);

	// Get the number of files in a particular location, of a certain type
	int		GetFileCount(const char *root, const char *mask);

	// Enumerate all the files of a certain type in a directory
	// "filename" should be a buffer of PATH_SIZE bytes
	int		EnumerateFiles(const char *root, const char *mask, char *filename, int *type = NULL);

	// Check to see if a certain file exists
	int		FileExists(const char *filename);

private:
	// Current disk directory
	char	current_directory[PATH_SIZE];

	// Whether or not an enumeration search is open
	int		search_open;

	// Where the enumeration search is currently looking
	char	search_path[PATH_SIZE];

	// Platform specific data
	void	*user_data;

	// Format the path so it has the ending slash
	void	FormatPath(char *path);

	// Removing everything at the end of a path to the ending slash
	void	TruncatePath(char *path);
};


#endif	/* _INCLUDED_CFILESYSTEM_H */