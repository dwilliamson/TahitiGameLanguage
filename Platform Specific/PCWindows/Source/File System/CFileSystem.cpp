
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <string.h>
#include "..\..\..\..\Common\include\Common.h"
#include "..\..\..\include\Platform.h"


CFileSystem::CFileSystem(void)
{
	// Setup the root to be the current directory
	GetCurrentDirectory(current_directory, PATH_SIZE);

	search_open = 0;
}


CFileSystem::CFileSystem(const char *root)
{
	// First set the directory
	SetCurrentDirectory(root);

	// Now retrieve the full current path
	GetCurrentDirectory(current_directory, PATH_SIZE);

	search_open = 0;
}


int CFileSystem::GetFileTime(const char *filename, CTime &time, int which)
{
	// Open the file for reading
	CFile	*file = new CFile(filename, FILEOPEN_READ);

	// Get the file time
	if (!file->GetTime(time, which))
	{
		delete file;
		return (0);
	}

	// Close the file
	delete file;

	return (0);
}


int CFileSystem::SetFileTime(const char *filename, CTime &time, int which)
{
	// Open the file for reading
	CFile	*file = new CFile(filename, FILEOPEN_WRITE | FILEOPEN_APPEND);

	// Get the file time
	if (!file->SetTime(time, which))
	{
		delete file;
		return (0);
	}

	// Close the file
	delete file;

	return (0);
}


int CFileSystem::GetFileSize(const char *filename)
{
	int		ret;

	// Open the file for reading
	CFile	*file = new CFile(filename, FILEOPEN_READ);

	// Get the file size
	ret = file->GetSize();

	// Close the file
	delete file;

	return (ret);
}


CFile *CFileSystem::OpenFile(const char *filename, int type)
{
	// Simply open the file (to be changed later for an actual filing system)
	return (new CFile(filename, type));
}


void CFileSystem::SetCurrentDirectory(const char *filename)
{
	// Call the windows version
	::SetCurrentDirectoryA(filename);

	// Setup the root to be the current directory
	GetCurrentDirectory(current_directory, PATH_SIZE);
}


void CFileSystem::GetCurrentDirectory(char *dest, int buflen)
{
	// Call the windows version
	::GetCurrentDirectory((DWORD)buflen, dest);

	// Make it nice
	FormatPath(dest);
}


int CFileSystem::GetFileCount(const char *root, const char *mask)
{
	char			search[PATH_SIZE];
	HANDLE			hFind;
	WIN32_FIND_DATA	w32_find;
	int				count = 1;

	if (root == NULL)
	{
		// Take the search path from the current directory
		wsprintf(search, "%s%s", current_directory, mask);
	}
	else
	{
		// Construct a new search path
		strcpy(search, root);
		FormatPath(search);
		strcat(search, mask);
	}

	// Try and see the first file
	if ((hFind = FindFirstFile(search, &w32_find)) == NULL)
		return (0);

	// Loop counting each next file
	while (FindNextFile(hFind, &w32_find) == TRUE)
		count++;

	// End the search
	FindClose(hFind);

	return (count);
}


int CFileSystem::EnumerateFiles(const char *root, const char *mask, char *filename, int *type)
{
	WIN32_FIND_DATA	w32_find;
	HANDLE			hFind;

	// Search initiated yet?
	if (!search_open)
	{
		if (root == NULL)
		{
			// Take the search path from the current directory
			wsprintf(search_path, "%s%s", current_directory, mask);
		}
		else
		{
			// Construct a new search path
			strcpy(search_path, root);
			FormatPath(search_path);
			strcat(search_path, mask);
		}

		// Try and see the first file
		if ((hFind = FindFirstFile(search_path, &w32_find)) == (void *)-1)
			return (0);

		// Set some data
		user_data = (void *)hFind;
		search_open = 1;
	}
	else
	{
		hFind = (HANDLE)user_data;

		// See if the next file exists
		if (FindNextFile(hFind, &w32_find) == FALSE)
		{
			search_open = 0;
			return (0);
		}
	}

	// Construct the Win32 filename
	strcpy(filename, search_path);
	TruncatePath(filename);
	strcat(filename, w32_find.cFileName);

	// Set the type
	if (type)
	{
		if (w32_find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) *type = FILETYPE_FOLDER;
		else *type = FILETYPE_FILE;
	}

	return (1);
}


int CFileSystem::FileExists(const char *filename)
{
	HANDLE	hFile;

	// Attempt to open the file
	if ((hFile = CreateFile(filename, GENERIC_READ, 0, NULL, 0, 0, NULL)) == INVALID_HANDLE_VALUE)
		return (0);

	// Shut stuff down
	CloseHandle(hFile);

	return (1);
}


void CFileSystem::FormatPath(char *path_name)
{
	int		s;

	// Get the length of the path
	s = strlen(path_name);

	// Append the character if need be
	if (path_name[s - 1] != '\\')
	{
		path_name[s] = '\\';
		path_name[s + 1] = 0;
	}
}


void CFileSystem::TruncatePath(char *path)
{
	char	*ptr;

	// Find the last occurence
	ptr = strrchr(path, '\\');

	// Truncate
	ptr[1] = 0;
}