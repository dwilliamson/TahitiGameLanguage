
#include "..\include\main.h"


CompileError::CompileError(const char *format, ...)
{
	va_list		arglist;
	char		buffer[512];

	// Make the error string
	va_start(arglist, format);
	vsprintf(buffer, format, arglist);
	va_end(arglist);

	// Prefix some stuff and copy it
	sprintf(message, "Error: %s\n", buffer);
}


char *CompileError::GetMessage(void)
{
	return (message);
}