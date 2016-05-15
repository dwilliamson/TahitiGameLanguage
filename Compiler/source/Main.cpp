
#include "..\include\main.h"


//#define TRAP_EXCEPTIONS


CFileSystem		g_file_system;
CCallStack		g_call_stack;
unsigned int	g_compile_all = 0;
unsigned int	g_has_IDL = 0;
char			g_IDL_file[256];


int ProcessArguments(int argc, char *argv[])
{
	int		x = 1;

	infunc(ProcessArguments);

	// Compile and link command (no arguments)
	if (argc == 1)
		return (1);

	// Now process the options
	for (x = 1; x < argc; x++)
	{
		if (argv[x][0] == '-')
		{
			// Compile all files instead of comparing their times with that of their objects
			if (argv[x][1] == 'a')
				g_compile_all = 1;

			// Specifies an interface definition file
			if (argv[x][1] == 'i')
			{
				g_has_IDL = 1;
				strcpy(g_IDL_file, &argv[x][2]);
			}
		}
	}

	return (1);

	outfunc;
}


int CompileFiles(void)
{
	char		current_file[PATH_SIZE], object_file[PATH_SIZE];
	int			x = 0;
	Environment	*env;

	infunc(CompileFiles);

	// Init the environment
	if ((env = new Environment) == NULL)
		throw CError("Couldn't allocate the environment!");

	// Loop through all the .VMC files in the current directory
	while (g_file_system.EnumerateFiles(NULL, "*.vmc", current_file))
	{
		CTime	vmc_time, vmo_time;

		// If there's nothing there, ignore it
		if (g_file_system.GetFileSize(current_file) == 0)
			continue;

		// Change the filename to that of an object file
		strcpy(object_file, current_file);
		object_file[strlen(object_file) - 1] = 'o';

		// If the object file doesn't exist, continue
		if (g_compile_all || !g_file_system.FileExists(object_file))
		{
			env->ConstructClass(current_file);
			continue;
		}

		// Get the file time of both the object and source
		g_file_system.GetFileTime(current_file, vmc_time, FILETIME_LASTWRITE);
		g_file_system.GetFileTime(object_file, vmo_time, FILETIME_LASTWRITE);

		/* ---

		If the times match, the VMC file has been compiler so add its object file
		contents to the environment. Otherwise, the VMC file has been modified and
		the new structure needs to be read from the VMC file.

		--- */

		if (vmc_time == vmo_time)
			env->AddObject(object_file);
		else
			env->ConstructClass(current_file);
	}

	if (g_has_IDL)
		env->AddIDL(g_IDL_file);

	// Link all classes ready for the second pass
	env->LinkClasses();

	// Compile the classes hierarchically
	env->CompileClasses();

	// Build all tables necessary for writing
	env->PrepareForOutput();

	// Write the unit
	env->GenerateVMUnit("output.vmu");

	delete env;

	return (1);

	outfunc;
}


int MainProgram(int argc, char *argv[])
{
	try
	{
		infunc(MainProgram);

		if (!ProcessArguments(argc, argv))
			return (1);

		if (!CompileFiles())
			return (1);

		outfunc;
	}
	catch (CompileError &error)
	{
		printf("%s", error.GetMessage());

		return (1);
	}
	catch (CError &error)
	{
		const char	*string;

		printf("Internal Compiler Error: %s\n\nCall Stack:\n\n", error.GetMessage());

#ifdef	CALLSTACK_ENABLED
		// Print the call stack
		while (string = g_call_stack.EnumerateFunctions())
			printf("%s ->\n", string);
#endif

		return (1);
	}

	return (0);
}


int main(int argc, char *argv[])
{
	// In a seperate function because maybe this part needs to be platform specific

#ifdef	TRAP_EXCEPTIONS

	try
	{
		if (!MainProgram(argc, argv))
			return (1);
	}
	catch (...)
	{
		const char	*string;

		printf("Exception!\n\nCall Stack:\n\n");

#ifdef	CALLSTACK_ENABLED
		// Print the call stack
		while (string = g_call_stack.EnumerateFunctions())
			printf("%s ->\n", string);
#endif

		return (1);
	}

#else

	return (MainProgram(argc, argv));

#endif

	return (0);
}