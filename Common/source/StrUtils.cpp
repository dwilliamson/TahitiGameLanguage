
#include "..\include\Common.h"


char *strnew(const char *string)
{
	char	*retstr;

	// Allocate and copy the string
	retstr = new char[strlen(string) + 1];
	strcpy(retstr, string);

	return (retstr);
}