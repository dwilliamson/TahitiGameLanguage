
#include "..\include\Common.h"


CError::CError(const char *text, ...)
{
	va_list		arglist;
	char		buffer[512];

	// Make the error string
	va_start(arglist, text);
	vsprintf(buffer, text, arglist);
	va_end(arglist);

	// Now copy it
	strcpy(message, buffer);
}