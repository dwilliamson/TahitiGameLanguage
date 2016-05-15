
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "..\..\..\..\Common\include\Common.h"
#include "..\..\..\include\Platform.h"
#include "CFile_internal.h"


CFile::CFile(const char *filename, int type)
{
	FileInfo	*info_ptr;
	DWORD		access = 0, creation = 0;

	user_data = NULL;

	// Allocate the file info structure
	if ((info_ptr = new FileInfo) == NULL)
		throw CError("Failed to allocate file information structure");

	// Set the access mode
	access |= (type & FILEOPEN_READ ? GENERIC_READ : 0);
	access |= (type & FILEOPEN_WRITE ? GENERIC_WRITE : 0);
	creation = (type & FILEOPEN_READ ? OPEN_EXISTING : type & FILEOPEN_APPEND ? OPEN_ALWAYS : CREATE_ALWAYS);

	// Attempt to open the file
	if ((info_ptr->handle = CreateFile(filename, access, 0, NULL, creation, 0, NULL)) == INVALID_HANDLE_VALUE)
	{
		unsigned int	error;

		error = GetLastError();

		throw CError("Failed to open the file '%s'", filename);
	}

	// Save the information structure
	user_data = (void *)info_ptr;
}


CFile::~CFile(void)
{
	// Close the file
	Close();
}


void CFile::Close(void)
{
	if (user_data)
	{
		// Ask windows to close the file
		CloseHandle(((FileInfo *)user_data)->handle);

		// Release the allocated structure
		delete user_data;

		user_data = NULL;
	}
}


int CFile::Write(void *data, int nb_bytes)
{
	int		out_bytes;

	// Write the number of bytes
	if (WriteFile(((FileInfo *)user_data)->handle, data, nb_bytes, (DWORD *)&out_bytes, NULL) == FALSE)
		return (0);

	return (out_bytes);
}


int CFile::WriteAt(int location, void *data, int nb_bytes)
{
	int		out_bytes, backup;

	// Seek to the new position
	backup = GetPosition();
	SeekTo(location, FILESEEK_START);

	// Write the data
	out_bytes = Write(data, nb_bytes);

	// Restore position
	SeekTo(backup, FILESEEK_START);

	return (out_bytes);
}


int CFile::Read(void *data, int nb_bytes)
{
	int		out_bytes;

	// Read the nuber of bytes
	if (ReadFile(((FileInfo *)user_data)->handle, data, nb_bytes, (DWORD *)&out_bytes, NULL) == FALSE)
	{
		// Check for EOF
		if (GetLastError() == ERROR_HANDLE_EOF)
			return (-1);

		return (0);
	}

	return (out_bytes);
}


int CFile::Printf(const char *format, ...)
{
	va_list		arglist;
	char		*buffer;
	int			ret;

	// Allocate the buffer
	if ((buffer = new char[strlen(format) + 512]) == NULL)
		throw CError("Couldn't allocate string");

	// Make the string
	va_start(arglist, format);
	vsprintf(buffer, format, arglist);
	va_end(arglist);

	// Write it to file
	ret = Write(buffer, strlen(buffer));

	// Release the buffer
	delete buffer;

	return (ret);
}


int CFile::PrintfZ(const char *format, ...)
{
	va_list		arglist;
	char		*buffer;

	// Allocate the buffer
	if ((buffer = new char[strlen(format) + 512]) == NULL)
		throw CError("Couldn't allocate string");

	// Make the string
	va_start(arglist, format);
	vsprintf(buffer, format, arglist);
	va_end(arglist);

	// Write stuff
	if (!Write(buffer, strlen(buffer)))
		return (0);

	// Release the buffer
	delete buffer;

	return (WriteByte(0));
}


int CFile::ReadStringZ(char *dest)
{
	int		x = 0;
	int		inchar;

	// Read all the bytes until EOF
	while ((inchar = ReadByte()) != EOF)
	{
		// Store this one
		dest[x++] = inchar;

		// If it was NULL terminator, break
		if (inchar == 0)			
			break;
	}

	return (x);
}


int CFile::WriteByte(int byte)
{
	int		out_bytes;

	// Write the byte
	if (WriteFile(((FileInfo *)user_data)->handle, (void *)&byte, 1, (DWORD *)&out_bytes, NULL) == FALSE)
		return (0);

	return (out_bytes);
}


int CFile::ReadByte(void)
{
	int		out_bytes;
	int		byte = 0;

	// Read the byte
	if (ReadFile(((FileInfo *)user_data)->handle, (void *)&byte, 1, (DWORD *)&out_bytes, NULL) == FALSE)
	{
		// Check for EOF
		if (GetLastError() == ERROR_HANDLE_EOF)
			return (-1);

		return (0);
	}

	if (out_bytes == 0)
		return (-1);

	return (byte);
}


int CFile::WriteByteLoop(int byte_val, int nb_times)
{
	unsigned char	*data;
	int				ret;

	// Allocate some space for the data
	if ((data = new unsigned char[nb_times]) == NULL)
		return (0);

	// Loop write the byte to the buffer
	__asm
	{
		mov		ecx, nb_times
		mov		eax, byte_val
		mov		edi, data

		rep		stosb
	}

	// Write the data
	ret = Write(data, nb_times);

	// Free up the memory
	delete data;

	return (ret);
}


int CFile::WriteWordLoop(int word_val, int nb_times)
{
	unsigned short	*data;
	int				ret;

	// Allocate some space for the data
	if ((data = new unsigned short[nb_times]) == NULL)
		return (0);

	// Loop write the word to the buffer
	__asm
	{
		mov		ecx, nb_times
		mov		eax, word_val
		mov		edi, data

		rep		stosw
	}

	// Write the data
	ret = Write(data, nb_times << 1);

	// Free up the memory
	delete data;

	return (ret);
}


int CFile::WriteDWordLoop(int dword_val, int nb_times)
{
	unsigned int	*data;
	int				ret;

	// Allocate some space for the data
	if ((data = new unsigned int[nb_times]) == NULL)
		return (0);

	// Loop write the dword to the buffer
	__asm
	{
		mov		ecx, nb_times
		mov		eax, dword_val
		mov		edi, data

		rep		stosd
	}

	// Write the data
	ret = Write(data, nb_times << 2);

	// Free up the memory
	delete data;

	return (ret);
}


int CFile::SeekTo(int distance, int where)
{
	DWORD	method;

	// Determine the seek method
	switch (where)
	{
		case (FILESEEK_START): method = FILE_BEGIN; break;
		case (FILESEEK_CURRENT): method = FILE_CURRENT; break;
		case (FILESEEK_END): method = FILE_END; break;
	}

	// Seek to the position
	if (SetFilePointer(((FileInfo *)user_data)->handle, distance, NULL, method) == -1)
		return (-1);

	return (1);
}


int CFile::GetPosition(void)
{
	LONG	pointer = 0;

	// Seek to the current position, location is placed in the variable
	if ((pointer = SetFilePointer(((FileInfo *)user_data)->handle, 0, NULL, FILE_CURRENT)) == -1)
		return (-1);

	return ((int)pointer);
}


int CFile::GetTime(CTime &time, int which)
{
	FILETIME	utc_time, local_time;
	SYSTEMTIME	system_time;
	BOOL		retval;

	// Get the current time in UTC format
	switch (which)
	{
		case (FILETIME_CREATION):
			retval = GetFileTime(((FileInfo *)user_data)->handle, &utc_time, NULL, NULL);
			break;

		case (FILETIME_LASTACCESS):
			retval = GetFileTime(((FileInfo *)user_data)->handle, NULL, &utc_time, NULL);
			break;

		case (FILETIME_LASTWRITE):
			retval = GetFileTime(((FileInfo *)user_data)->handle, NULL, NULL, &utc_time);
			break;
	}

	// Failed?
	if (retval == FALSE)
		return (0);

	// Convert from UTC to local time (daylight savings and things)
	if (!FileTimeToLocalFileTime(&utc_time, &local_time))
		return (0);

	// Now convert into the needed system time
	FileTimeToSystemTime(&local_time, &system_time);

	// Transfer these into the time class
	time.Year = system_time.wYear;
	time.Month = system_time.wMonth;
	time.Day = system_time.wDay;
	time.DayOfWeek = system_time.wDayOfWeek;
	time.Hour = system_time.wHour;
	time.Minute = system_time.wMinute;
	time.Second = system_time.wSecond;
	time.Millisecond = system_time.wMilliseconds;

	return (1);
}


int CFile::SetTime(CTime &time, int which)
{
	FILETIME	local_time;
	SYSTEMTIME	system_time;
	BOOL		retval;

	// Transfer the time to system time
	system_time.wDay = time.Day;
	system_time.wDayOfWeek = time.DayOfWeek;
	system_time.wHour = time.Hour;
	system_time.wMilliseconds = time.Millisecond;
	system_time.wMinute = time.Minute;
	system_time.wMonth = time.Month;
	system_time.wSecond = time.Second;
	system_time.wYear = time.Year;

	// Convert from system to local time
	if (!SystemTimeToFileTime(&system_time, &local_time))
		return (0);

	// Set the current time in UTC format
	switch (which)
	{
		case (FILETIME_CREATION):
			retval = SetFileTime(((FileInfo *)user_data)->handle, &local_time, NULL, NULL);
			break;

		case (FILETIME_LASTACCESS):
			retval = SetFileTime(((FileInfo *)user_data)->handle, NULL, &local_time, NULL);
			break;

		case (FILETIME_LASTWRITE):
			retval = SetFileTime(((FileInfo *)user_data)->handle, NULL, NULL, &local_time);
			break;
	}

	// Failed?
	if (retval == FALSE)
	{
		unsigned int	error;

		error = GetLastError();

		return (0);
	}

	return (1);
}


int CFile::GetSize(void)
{
	DWORD	size;

	if ((size = GetFileSize(((FileInfo *)user_data)->handle, NULL)) == 0xFFFFFFFF)
	{
		DWORD	error;

		error = GetLastError();

		return (-1);
	}

	return (size);
}