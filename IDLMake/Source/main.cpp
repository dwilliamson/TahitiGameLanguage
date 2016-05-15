
#include "..\include\main.h"

#undef GetMessage


#define FLAGS_RECURSE			1


#define CFLAGS_IN_METHOD_REG	1


// File system
CFileSystem		g_FileSystem;

// List of files to process
TArray<char *>	g_FileList;

// Search flags
int				g_Flags = 0;

// List of tokens found within the C++ file
TArray<Token *>	g_TokenList;

// List of interfaces found
TArray<Interface *>	g_InterfaceList;

// List of base interfaces
TArray<Interface *>	g_BaseInterfaces;

// Default output name
char			g_Output[256] = { "output.idl" };

// Were there any errors in the file
int				g_Errors = 0;


void GetTokenString(CFile *file, Token &token, char *string)
{
	// Seek to the location
	file->SeekTo(token.m_Location, FILESEEK_START);

	// Read the token string
	file->Read(string, token.m_Length);
	string[token.m_Length] = 0;
}


void ClearTokens(void)
{
	int		x;

	// Delete all the tokens
	for (x = 0; x < g_TokenList.GetPosition(); x++)
		delete g_TokenList(x);

	// Back to the beginning
	g_TokenList.Reset();
}


void TokeniseFile(CFile *file)
{
	int		cur_char;
	int		x = 0, pos = 0, len = 0;
	char	string[512];
	int		already_read = 0;
	int		line = 1;

	// Loop reading each byte in the file
	while (1)
	{
		if (already_read)
			already_read = 0;
		else
		{
			cur_char = file->ReadByte();
			x++;
		}

		// Break at end of file
		if (cur_char == EOF)
			break;

		// Move up a line
		if (cur_char == 10)
			line++;

		// Is it an alphabetical character
		if (IS_ALPHA(cur_char))
		{
			// Mark the beginning of the word
			pos = x - 1;

			// Loop reading the word
			while (IS_ALPHA(cur_char))
			{
				// Add the character
				string[x - pos - 1] = cur_char;

				cur_char = file->ReadByte();
				x++;

				already_read = 1;
			}
			string[x - pos - 1] = 0;

			// Calculate the length
			len = x - pos - 1;

			// Check for tokens
			if (already_read)
			{
				// First keywords
				DO_KEYWORD(vm_START_METHOD_REGISTER);
				DO_KEYWORD(vm_END_METHOD_REGISTER);
				DO_KEYWORD(vm_REGISTER_METHOD);
				DO_KEYWORD(vm_REGISTER_CLASS);

				// Getting to this stage means it's a name
				Token *token = new Token;

				// Fill info
				token->m_Type = TOKEN_NAME;
				token->m_Location = pos;
				token->m_Length = len;
				token->m_Line = line;

				// Add it to the list
				g_TokenList.Add(token);
			}
		}

		// Beginning of a string
		else if (cur_char == '\"')
		{
			// Mark the beginning of the string
			pos = x - 1;

			// Read a byte
			cur_char = file->ReadByte();
			x++;

			// Loop reading the string
			while (cur_char != '\"')
			{
				cur_char = file->ReadByte();
				x++;
			}

			// Calculate the length
			len = x - pos;

			// Allocate the new token
			Token *token = new Token;

			// Fill info
			token->m_Type = TOKEN_STRING;
			token->m_Location = pos + 1;
			token->m_Length = len - 2;
			token->m_Line = line;

			// Add it to the list
			g_TokenList.Add(token);
		}

		// Beginning of a comment
		else if (cur_char == '/')
		{
			int		end[2];

			// Mark the beginning of the string
			pos = x - 1;

			// Read a byte
			cur_char = file->ReadByte();
			x++;

			// Determine the comment end
			if (cur_char == '*')
			{
				end[0] = '*';
				end[1] = '/';

				// Read passed
				cur_char = file->ReadByte();
				x++;
			}
			else
			{
				end[0] = 13;
				end[1] = 10;
			}

			LoopAgain:;

			// Loop reading the string
			while (cur_char != end[0])
			{
				cur_char = file->ReadByte();
				x++;
			}

			// It's a two-byte combination
			cur_char = file->ReadByte();
			x++;
			
			if (cur_char != end[1])
			{
				already_read = 1;
				x--;

				if (end[0] == '*')
					goto LoopAgain;
			}

			// Calculate the length
			len = x - pos;

			// Allocate the new token
			Token *token = new Token;

			// Fill info
			token->m_Type = TOKEN_COMMENT;
			token->m_Location = pos;
			token->m_Length = len;
			token->m_Line = line;

			// Add the CRLF
			if (end[0] == '*') token->m_Length += 2;

			// Add it to the list
			g_TokenList.Add(token);
		}
	}
}


char *CheckType(char *filename, char *string, Token &token, int parameter)
{
	int		x = 0, y = 0;

	// Skip past any whitespace
	while (string[x] && (string[x] == 32 || string[x] == 9)) x++;
	y = x;

	// Read the name
	while (string[x] && IS_ALPHA(string[x])) x++;

	// If there is no name
	if (x == y)
	{
		if (parameter == -1)
			printf("ERROR - [%s, %d]\n   Expecting a type name for return type\n", filename, token.m_Line);
		else
			printf("ERROR - [%s, %d]\n   Expecting a type name for parameter %d\n", filename, token.m_Line, parameter);
		g_Errors = 1;
		return (NULL);
	}

	// Skip past any whitespace
	while (string[x] && (string[x] == 32 || string[x] == 9)) x++;

	// Expecting a name if this isn't a return type and it isn't 'void'
	if (parameter != -1 && strncmp(string, "void", 4))
	{
		y = x;

		// Read the name
		while (string[x] && IS_ALPHA(string[x])) x++;

		// If there is no name
		if (x == y)
		{
			printf("ERROR - [%s, %d]\n   Expecting the name of parameter %d\n", filename, token.m_Line, parameter);
			g_Errors = 1;
			return (NULL);
		}

		// Skip past any whitespace
		while (string[x] && (string[x] == 32 || string[x] == 9)) x++;
	}

	// Is it declaring an array?
	if (string[x] == '[')
	{
		int		y = 0;

		x++;

		// If a void-type array is trying to be declared
		if (!strncmp(string, "void", 4))
		{
			if (parameter == -1)
				printf("ERROR - [%s, %d]\n   'void' type cannot be an array in return type\n", filename, token.m_Line);
			else
				printf("ERROR - [%s, %d]\n   'void' type cannot be an array in parameter %d\n", filename, token.m_Line, parameter);
			g_Errors = 1;
			return (NULL);
		}

		// Loop reading the array dimension
		while (1)
		{
			// Check for the closing array
			if (string[x] == ']')
			{
				// If a number wasn't specified
				if (y == 0)
				{
					if (parameter == -1)
						printf("ERROR - [%s, %d]\n   Expecting a dimension for the array of the return type\n", filename, token.m_Line);
					else
						printf("ERROR - [%s, %d]\n   Expecting a dimension for the array of parameter %d\n", filename, token.m_Line, parameter);
					g_Errors = 1;
					return (NULL);
				}

				x++;

				break;
			}

			// If the dimension is not constant
			if (!IS_NUM(string[x]))
			{
				if (parameter == -1)
					printf("ERROR - [%s, %d]\n   Array dimension in return type is not a constant number\n", filename, token.m_Line);
				else
					printf("ERROR - [%s, %d]\n   Array dimension in parameter %d is not a constant number\n", filename, token.m_Line, parameter);
				g_Errors = 1;
				return (NULL);
			}

			x++;
			y++;
		}

		// Skip past any whitespace
		while (string[x] && (string[x] == 32 || string[x] == 9)) x++;
	}

	return (&string[x]);
}


int CheckSingleType(char *filename, char *string, Token &token)
{
	char	*end;

	// Check a single type
	if ((end = CheckType(filename, string, token, -1)) == NULL)
		return (0);

	// If there's some crappy stuff after the declaration
	if (*end != 0)
	{
		printf("ERROR - [%s, %d]\n   Illegal character after return type declaration\n", filename, token.m_Line);
		g_Errors = 1;
		return (0);
	}

	return (1);
}


int CheckMultipleTypes(char *filename, char *string, Token &token)
{
	char	*cur_pos = string;
	int		x = 1;

	while (1)
	{
		// Check the current single type
		if ((cur_pos = CheckType(filename, cur_pos, token, x)) == NULL)
			return (0);

		// Check for the end of the declaration
		if (*cur_pos == 0)
			break;

		// If there is no comma
		if (*cur_pos != ',')
		{
			printf("ERROR - [%s, %d]\n   Expecting comma seperator or end of declaration after paramter %d\n", filename, token.m_Line, x);
			g_Errors = 1;
			return (0);
		}

		// Skip past the comma
		cur_pos++;

		x++;
	}

	return (1);
}


void ProcessFile(char *filename)
{
	CFile		*file;
	int			x, y;
	int			flags = 0;
	Interface	*cur_iface = NULL;
	Method		*cur_method = NULL;
	char		name[256];

	// Open the file
	file = new CFile(filename, FILEOPEN_READ);

	// Turn the text into tokens
	TokeniseFile(file);

	// For each token generated
	for (x = 0; x < g_TokenList.GetPosition(); x++)
	{
		// Grab the token
		Token token = *g_TokenList(x);

		switch (token.m_Type)
		{
			case (TOKEN_vm_START_METHOD_REGISTER):

				// If already in a method register block
				if (flags & CFLAGS_IN_METHOD_REG)
				{
					printf("ERROR - [%s, %d]\n   Already within a method register block\n", filename, token.m_Line);
					g_Errors = 1;
					continue;
				}

				flags |= CFLAGS_IN_METHOD_REG;

				// If the next token isn't a name for the interface
				if (g_TokenList(x + 1)->m_Type != TOKEN_NAME)
				{
					printf("ERROR - [%s, %d]\n   Expecting an interface name after 'START_METHOD_REGISTER'\n", filename, token.m_Line);
					g_Errors = 1;
					continue;
				}

				// Read the interface name
				GetTokenString(file, *g_TokenList(x + 1), name);

				// Search for the interface
				for (y = 0; y < g_InterfaceList.GetPosition(); y++)
				{
					if (!strcmp(g_InterfaceList(y)->m_Name, name))
					{
						cur_iface = g_InterfaceList(y);
						break;
					}
				}

				// Nothing found
				if (y == g_InterfaceList.GetPosition())
				{
					// Allocate a new interface
					cur_iface = new Interface;

					// Default stuff
					strcpy(cur_iface->m_Name, name);
					cur_iface->m_Inherits = 0;

					// Add the current interface to the list
					g_InterfaceList.Add(cur_iface);
				}

				break;

			case (TOKEN_vm_END_METHOD_REGISTER):

				// If not in a method register block
				if (!(flags & CFLAGS_IN_METHOD_REG))
				{
					printf("ERROR - [%s, %d]\n   There is no method register block to end\n", filename, token.m_Line);
					g_Errors = 1;
					continue;
				}

				flags &= ~CFLAGS_IN_METHOD_REG;

				break;

			case (TOKEN_vm_REGISTER_CLASS):

				// If no interface name was specified
				if (g_TokenList(x + 1)->m_Type != TOKEN_NAME)
				{
					printf("ERROR - [%s, %d]\n   Expecting an interface name\n", filename, token.m_Line);
					g_Errors = 1;
					continue;
				}

				// If no interface to inherit from was specified
				if (g_TokenList(x + 2)->m_Type != TOKEN_NAME)
				{
					printf("ERROR - [%s, %d]\n   Expecting an interface to inherit from\n", filename, token.m_Length);
					g_Errors = 1;
					continue;
				}

				// Get the interface name
				GetTokenString(file, *g_TokenList(x + 1), name);

				// Search the interface list looking for it
				for (y = 0; y < g_InterfaceList.GetPosition(); y++)
				{
					if (!strcmp(name, g_InterfaceList(y)->m_Name))
					{
						// Get the super-interface
						GetTokenString(file, *g_TokenList(x + 2), name);

						if (strcmp(name, "vm_InheritsNone"))
						{
							// Copy inheritance info
							g_InterfaceList(y)->m_Inherits = 1;
							strcpy(g_InterfaceList(y)->m_Super, name);
						}

						break;
					}
				}

				// If no matching interface was found
				if (y == g_InterfaceList.GetPosition())
				{
					// Allocate a new interface
					cur_iface = new Interface;

					// Read its name
					GetTokenString(file, *g_TokenList(x + 1), cur_iface->m_Name);

					// Get the super-interface
					GetTokenString(file, *g_TokenList(x + 2), name);

					if (strcmp(name, "vm_InheritsNone"))
					{
						// Copy inheritance info
						cur_iface->m_Inherits = 1;
						strcpy(cur_iface->m_Super, name);
					}
					else
						cur_iface->m_Inherits = 0;

					// Add the current interface to the list
					g_InterfaceList.Add(cur_iface);
					cur_iface = NULL;
				}

				break;

			case (TOKEN_vm_REGISTER_METHOD):

				// If not within a method register block
				if (!(flags & CFLAGS_IN_METHOD_REG))
				{
					printf("ERROR - [%s, %d]\n   'REGISTER_METHOD' cannot appear outside of a method register block\n", filename, token.m_Line);
					g_Errors = 1;
					continue;
				}

				// If there is no method name
				if (g_TokenList(x + 1)->m_Type != TOKEN_NAME)
				{
					printf("ERROR - [%s, %d]\n   Expecting a name for the method that is being registered\n", filename, token.m_Line);
					g_Errors = 1;
					continue;
				}

				// If there is no return type string
				if (g_TokenList(x + 2)->m_Type != TOKEN_STRING)
				{
					printf("ERROR - [%s, %d]\n   Expecting a return type string for the method being registered\n", filename, token.m_Line);
					g_Errors = 1;
					continue;
				}

				// If there is no parameter type string
				if (g_TokenList(x + 3)->m_Type != TOKEN_STRING)
				{
					printf("ERROR - [%s, %d]\n   Expecting a parameter list string for the method being registered\n", filename, token.m_Line);
					g_Errors = 1;
					continue;
				}

				// Allocate a new method
				cur_method = new Method;

				// Get the method info
				GetTokenString(file, *g_TokenList(x + 1), cur_method->m_Name);
				GetTokenString(file, *g_TokenList(x + 2), cur_method->m_Returns);
				GetTokenString(file, *g_TokenList(x + 3), cur_method->m_Parameters);

				if (!CheckSingleType(filename, cur_method->m_Returns, *g_TokenList(x + 2)))
					continue;

				if (!CheckMultipleTypes(filename, cur_method->m_Parameters, *g_TokenList(x + 3)))
					continue;

				// Attach a comment if there is one
				if (g_TokenList(x - 1)->m_Type == TOKEN_COMMENT)
				{
					char	comment[1024];

					GetTokenString(file, *g_TokenList(x - 1), comment);
					cur_method->m_Comment = strnew(comment);
				}

				// Add the method to the interface
				cur_iface->m_MethodList.Add(cur_method);
				cur_method = NULL;

				break;
		}
	}

	// Close the file
	delete file;
}


void SearchDirectory(char *mask)
{
	int				type;
	char			name[512];
	int				x = 0;
	TArray<char *>	dirlist;

	// Enumerate all the files in this directory
	while (g_FileSystem.EnumerateFiles(NULL, mask, name, &type))
	{
		if (type == FILETYPE_FILE)
		{
			// Create the name
			char	*filename = strnew(name);

			// Add it to the list of files
			g_FileList.Add(filename);
		}
	}

	// Enumerate everything
	while (g_FileSystem.EnumerateFiles(NULL, "*", name, &type))
	{
		if (type == FILETYPE_FOLDER)
		{
			// First two directories are "." and ".."
			if (x++ < 2)
				continue;

			// Create the name
			char	*filename = strnew(name);

			// Add it to the list of directories
			dirlist.Add(filename);
		}
	}

	// For ever directory to recurse
	for (x = 0; x < dirlist.GetPosition(); x++)
	{
		// Set the path
		g_FileSystem.SetCurrentDirectory(dirlist(x));

		// Recurse
		SearchDirectory(mask);

		// Move back up a level
		g_FileSystem.SetCurrentDirectory("..");

		// Release the memory
		delete [] dirlist(x);
	}
}


int ProcessArguments(int argc, char *argv[])
{
	int		x;

	// For each argument
	for (x = 1; x < argc; x++)
	{
		// Check for a switch
		if (argv[x][0] == '-')
		{
			if (argv[x][1] == 'r') g_Flags |= FLAGS_RECURSE;

			if (argv[x][1] == 'o') strcpy(g_Output, argv[x] + 2);
		}

		// It's a filename
		else
		{
			// Is it a wildcard search?
			if (strchr(argv[x], '*'))
				SearchDirectory(argv[x]);

			else
			{
				// Create a new filename
				char	*filename = strnew(argv[x]);

				// Add it to the list
				g_FileList.Add(filename);
			}
		}
	}

	return (1);
}


Interface *GetInterface(char *name)
{
	int		x;

	// Search for an interface match
	for (x = 0; x < g_InterfaceList.GetPosition(); x++)
		if (!strcmp(name, g_InterfaceList(x)->m_Name))
			return (g_InterfaceList(x));

	return (NULL);
}


void BuildInterfaceHierarchy(void)
{
	int		x;

	// For each interface
	for (x = 0; x < g_InterfaceList.GetPosition(); x++)
	{
		Interface	*interface_ptr;

		// Get the current interface
		interface_ptr = g_InterfaceList(x);

		// Does this interface inherit?
		if (interface_ptr->m_Inherits)
		{
			Interface	*super;

			// Get the super-interface
			super = GetInterface(interface_ptr->m_Super);

			// Add the child
			super->m_Children.Add(interface_ptr);
		}

		// No
		else
		{
			// Add it to the list of base interfaces
			g_BaseInterfaces.Add(interface_ptr);
		}
	}
}


void WriteInterface(CFile *file, Interface *interface_ptr)
{
	int		x;

	// Header
	if (interface_ptr->m_Inherits)
		file->Printf("interface %s extends %s\r\n", interface_ptr->m_Name, interface_ptr->m_Super);
	else
		file->Printf("interface %s\r\n", interface_ptr->m_Name);
	file->Printf("{\r\n");

	// Print each method
	for (x = 0; x < interface_ptr->m_MethodList.GetPosition(); x++)
	{
		// Shortcut
		Method	*method_ptr = interface_ptr->m_MethodList(x);

		if (x && interface_ptr->m_MethodList(x - 1)->m_Comment)
			file->Printf("\r\n");

		// Comment
		if (method_ptr->m_Comment)
		{
			file->Printf("\t%s", method_ptr->m_Comment);
			delete method_ptr->m_Comment;
		}

		file->Printf("\tmethod %s %s(%s);\r\n", method_ptr->m_Returns, method_ptr->m_Name,
			method_ptr->m_Parameters);
	}

	// Footer
	file->Printf("}\r\n\r\n\r\n");

	// Write the children
	for (x = 0; x < interface_ptr->m_Children.GetPosition(); x++)
		WriteInterface(file, interface_ptr->m_Children(x));
}


void WriteIDLFile(void)
{
	CFile	*file;
	CTime	time;
	int		x;

	BuildInterfaceHierarchy();

	// Open the output file
	file = new CFile(g_Output, FILEOPEN_WRITE);

	// Get just this instant!
	file->GetTime(time, FILETIME_LASTWRITE);

	// Write the file header
	file->Printf("\r\n//\r\n");
	file->Printf("// File: %s\r\n", g_Output);
	file->Printf("// Generated by IDLMake.exe at %02d/%02d/%d - %02d:%02d:%02d\r\n", time.Day, time.Month, time.Year, time.Hour, time.Minute, time.Second);
	file->Printf("//\r\n");
	file->Printf("// DO NOT EDIT!\r\n");
	file->Printf("//\r\n\r\n\r\n");

	// Write each interface from the base interface
	for (x = 0; x < g_BaseInterfaces.GetPosition(); x++)
		WriteInterface(file, g_BaseInterfaces(x));

	// Print each interface
/*	for (x = 0; x < g_InterfaceList.GetPosition(); x++)
	{
		// Header
		if (g_InterfaceList(x)->m_Inherits)
			file->Printf("interface %s extends %s\r\n", g_InterfaceList(x)->m_Name, g_InterfaceList(x)->m_Super);
		else
			file->Printf("interface %s\r\n", g_InterfaceList(x)->m_Name);
		file->Printf("{\r\n");

		// Print each method
		for (y = 0; y < g_InterfaceList(x)->m_MethodList.GetPosition(); y++)
		{
			// Shortcut
			Method	*method_ptr = g_InterfaceList(x)->m_MethodList(y);

			if (y && g_InterfaceList(y)->m_MethodList(y - 1)->m_Comment)
				file->Printf("\r\n");

			// Comment
			if (method_ptr->m_Comment)
			{
				file->Printf("\t%s", method_ptr->m_Comment);
				delete method_ptr->m_Comment;
			}

			// Header
//			file->Printf("\tmethod %s\r\n", method_ptr->m_Name);
//			file->Printf("\t{\r\n");

			// Information
//			file->Printf("\t\treturns %s;\r\n", method_ptr->m_Returns);
//			file->Printf("\t\tparameters %s;\r\n", method_ptr->m_Parameters);

			// Footer
//			file->Printf("\t}\r\n\r\n");

			file->Printf("\tmethod %s %s(%s);\r\n", method_ptr->m_Returns, method_ptr->m_Name,
				method_ptr->m_Parameters);
		}

		// Footer
		file->Printf("}\r\n\r\n\r\n");
	}*/

	// Close the file
	delete file;
}


void ClearInterfaces(void)
{
	int		x, y;

	// For every interface
	for (y = 0; y < g_InterfaceList.GetPosition(); y++)
	{
		// Delete every method in the interface
		for (x = 0; x < g_InterfaceList(y)->m_MethodList.GetPosition(); x++)
			delete g_InterfaceList(y)->m_MethodList(x);

		// Delete the interface
		delete g_InterfaceList(y);
	}

	// Back to the beginning
	g_InterfaceList.Reset();
}


void PrintHelp(void)
{
	char	string[64];
	int		x;

	// Header
	strcpy(string, "\nIDLMake - Build [ " __DATE__ " , " __TIME__ " ]\n");
	printf(string);
	for (x = 0; x < (int)strlen(string) - 2; x++) printf("-");
	printf("\n");

	// Information
	printf("This program will inspect one or more C/C++ files, looking for class and method\n");
	printf("registers with the virtual machine. Any that it finds will be compiled to an\n");
	printf("interface definition language ready for use by the virtual machine compiler.\n\n");

	// Syntax
	printf("Syntax...\n\n");
	printf("   idlmake <file_list> [options]\n\n");
	printf("You can either specify multiple filenames or a typical wildcard string.\n\n");

	// Options
	printf("Options...\n\n");
	printf("   -o<filename>     Specify the output filename (default: output.idl)\n");
	printf("   -r               Recurse sub-directories with wildcard search\n\n");
}


int main(int argc, char *argv[])
{
	int		x;

	try
	{
		// Build the file list
		if (!ProcessArguments(argc, argv))
			return (0);

		// Read through all the files
		for (x = 0; x < g_FileList.GetPosition(); x++)
		{
			ClearTokens();
			ProcessFile(g_FileList(x));
		}

		if (!g_Errors && g_FileList.GetPosition())
			WriteIDLFile();

		if (!g_FileList.GetPosition())
			PrintHelp();

		ClearInterfaces();

		// Release the name memory
		for (x = 0; x < g_FileList.GetPosition(); x++)
			delete [] g_FileList(x);
	}
	catch (CError &error)
	{
		printf("ERROR: %s\n", error.GetMessage());
		return (1);
	}

	return (0);
}