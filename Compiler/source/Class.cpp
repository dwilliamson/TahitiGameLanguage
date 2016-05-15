
#include "..\include\main.h"


Class::Class(void)
{
	cur_address = 0;
	cur_state = NULL;
	cur_function = NULL;
	cur_funcnum = 0;
	cur_statenum = 0;
	cur_impnum = 0;
	inherits = 0;
	defined = 0;
	nb_vt_entries = 0;
	sclass_ptr = NULL;
	nb_functions = 0;
	nb_impfuncs = 0;
	nb_states = 0;
	nb_it_entries = 0;
	nb_st_entries = 0;
	nb_variables = 0;
	need_compile = 0;
	flags = 0;
	entry_point = -1;
}


Class::~Class(void)
{
	infunc(Class::~Class);

	Variable		*var;
	State			*state;
	Function		*func;

	// Delete all the variables
	while (var = (Variable *)variables.Enumerate())
	{
		variables.Remove(var);
		delete var;
	}

	// Delete all the functions
	while (func = (Function *)functions.Enumerate())
	{
		functions.Remove(func);
		delete func;
	}

	// Delete all the states
	while (state = (State *)states.Enumerate())
	{
		states.Remove(state);
		delete state;
	}

	outfunc;
}


int Class::NewAddress(int size, int align)
{
	int		r;

	// Align the address on a 4 byte boundary if necessary
	if (align)
	{
		if (cur_address & 3)
			cur_address += (4 - (cur_address & 3));
	}

	r = cur_address;
	cur_address += size;

	return (r);
}


int Class::GetCurAddress(void)
{
	infunc(Class::GetCurAddress);
	return (cur_address);
	outfunc;
}


int Class::NewFunctionIndex(void)
{
	infunc(Class::NewFunctionIndex);
	return (cur_funcnum++);
	outfunc;
}


int Class::NewStateIndex(void)
{
	infunc(Class::NewStateIndex);
	return (cur_statenum++);
	outfunc;
}


int Class::NewImportIndex(void)
{
	infunc(Class::NewImportIndex);
	return (cur_impnum++);
	outfunc;
}


Variable *Class::GetVariable(char *name, int flags)
{
	infunc(Class::GetVariable);

	Variable	*var = NULL;
	Class		*class_ptr;

	// Attempt to get the variable as a local first
	if (flags & FLAGS_IN_FUNCTION)
		var = (Variable *)cur_function->local_list.GetEntry(name);

	// Try to get it as a member
	if (var == NULL)
		var = (Variable *)variables.GetEntry(name);

	// If it's not found, search the super-class
	if (var == NULL && (class_ptr = sclass_ptr))
	{
		while (class_ptr)
		{
			// Is it in this super-class?
			if (var = (Variable *)class_ptr->variables.GetEntry(name))
				break;

			class_ptr = class_ptr->sclass_ptr;
		}
	}

	return (var);

	outfunc;
}


Function *Class::GetFunction(char *name, int flags)
{
	infunc(Class::GetFunction);

	Function	*func_ptr = NULL;
	Class		*class_ptr;

	// Attempt to get the function as part of the state first
	if (flags & FLAGS_IN_STATE)
		func_ptr = (Function *)cur_state->functions.GetEntry(name);

	// Try to get it as a member
	if (func_ptr == NULL)
		func_ptr = (Function *)functions.GetEntry(name);

	// If it's not found, search the super-class
	if (func_ptr == NULL && (class_ptr = sclass_ptr))
	{
		while (class_ptr)
		{
			if (flags & FLAGS_IN_STATE)
			{
				State	*state_ptr;

				// Get the state that the current one inherits from
				state_ptr = class_ptr->GetState(cur_state->GetName());
				
				// If a super-state exists, get the function from that
				if (state_ptr)
					func_ptr = (Function *)state_ptr->functions.GetEntry(name);
			}

			// Try grabbing it as a member of the super-class
			if (func_ptr == NULL)
				func_ptr = (Function *)class_ptr->functions.GetEntry(name);

			if (func_ptr)
				break;

			class_ptr = class_ptr->sclass_ptr;
		}
	}

	return (func_ptr);

	outfunc;
}


Function *Class::GetImport(char *name)
{
	infunc(Class::GetImport);

	Function	*func_ptr = NULL;
	Class		*class_ptr;

	// Try to get it as a member
	func_ptr = (Function *)imp_functions.GetEntry(name);

	// If it's not found, search the super-class
	if (func_ptr == NULL && (class_ptr = sclass_ptr))
	{
		while (class_ptr)
		{
			// Try grabbing it as a member of the super-class
			func_ptr = (Function *)class_ptr->imp_functions.GetEntry(name);

			if (func_ptr)
				break;

			class_ptr = class_ptr->sclass_ptr;
		}
	}

	return (func_ptr);

	outfunc;
}


Variable *Class::GetStringLiteral(char *string)
{
	infunc(Class::GetStringLiteral);

	Variable	*var = NULL;
	Class		*class_ptr;

	// Try to get it as a member
	var = (Variable *)string_literals.GetEntry(string);

	// If it's not found, search the super-class
	if (var == NULL && (class_ptr = sclass_ptr))
	{
		while (class_ptr)
		{
			// Is it in this super-class?
			if (var = (Variable *)class_ptr->string_literals.GetEntry(string))
				break;

			class_ptr = class_ptr->sclass_ptr;
		}
	}

	return (var);

	outfunc;
}


void Class::SetSuperClass(char *name)
{
	inherits = 1;
	strcpy(super_class, name);
}


char *Class::GetSuperClass(void)
{
	return (super_class);
}


int Class::HasSuperClass(char *name)
{
	infunc(Class::HasSuperClass);

	if (sclass_ptr)
	{
		// Is this the correct super-class?
		if (!strcmp(super_class, name))
			return (1);

		// Recurse up
		sclass_ptr->HasSuperClass(name);
	}
	
	return (0);

	outfunc;
}


void Class::SetForCompile(char *file)
{
	infunc(Class::SetForCompile);
	strcpy(filename, file);
	need_compile = 1;
	outfunc;
}


int Class::NeedsCompile(void)
{
	infunc(Class::NeedsCompile);
	return (need_compile);
	outfunc;
}


char *Class::GetFilename(void)
{
	infunc(Class::GetFilename);
	return (filename);
	outfunc;
}


void Class::SetObjectFile(char *file)
{
	infunc(Class::SetObjectFile);
	strcpy(object_file, file);
	outfunc;
}


char *Class::GetObjectFile(void)
{
	infunc(Class::GetObjectFile);
	return (object_file);
	outfunc;
}


void Class::WriteValue(CFile *file, int value, int where)
{
	infunc(Class::WriteValue);
	file->WriteAt(where, &value, 4);
	outfunc;
}


int Class::DoesInherit(void)
{
	return (inherits);
}


void Class::SetDefined(void)
{
	defined = 1;
}


int Class::IsDefined(void)
{
	return (defined);
}


void Class::SetFlag(int flag)
{
	flags |= flag;
}


int Class::GetFlags(void)
{
	return (flags);
}


void Class::SetEntryPoint(int index)
{
	entry_point = index;
}


int Class::GetEntryPoint(void)
{
	return (entry_point);
}


void Class::ShiftFunctionIndices(int value)
{
	infunc(Class::ShiftFunctionIndices);

	int			x;
	Function	*func;
	State		*state;

	// Enumerate all the functions in this class and shift them
	while (func = (Function *)functions.Enumerate())
	{
		func->SetIndex(func->GetIndex() + value);
		nb_functions++;
	}

	// Enumerate all the states in the class
	while (state = (State *)states.Enumerate())
	{
		// Enumerate all the functions in the state and shift them
		while (func = (Function *)state->functions.Enumerate())
		{
			func->SetIndex(func->GetIndex() + value);
			nb_functions++;
		}
	}

	// Shift the entry point
	if (entry_point != -1)
		entry_point += value;

	// Now shift each child class
	for (x = 0; x < children.GetPosition(); x++)
		children(x)->ShiftFunctionIndices(value + nb_functions);

	outfunc;
}


void Class::ShiftImportIndices(int value)
{
	infunc(Class::ShiftImportIndices);

	int			x;
	Function	*func;

	// Enumerate all the imported functions in this class and shift them
	while (func = (Function *)imp_functions.Enumerate())
	{
		func->SetIndex(func->GetIndex() + value);
		nb_impfuncs++;
	}

	// Now shift each child class
	for (x = 0; x < children.GetPosition(); x++)
		children(x)->ShiftImportIndices(value + nb_impfuncs);

	outfunc;
}


void Class::ShiftStateIndices(int value)
{
	infunc(Class::ShiftStateIndices);

	int		x;
	State	*state;

	// Enumerate all the states in this class and shift them
	while (state = (State *)states.Enumerate())
	{
		state->SetIndex(state->GetIndex() + value);
		nb_states++;
	}

	// Now shift each child class
	for (x = 0; x < children.GetPosition(); x++)
		children(x)->ShiftStateIndices(value + nb_states);

	outfunc;
}


void Class::ShiftVariableAddresses(int value)
{
	infunc(Class::ShiftVariableAddresses);

	int			x, max_addr = 0, addr;
	Variable	*var;

	// Enumerate all the variables in this class and shift them
	while (var = (Variable *)variables.Enumerate())
	{
		// Set the new shifted address
		addr = var->GetAddress() + value;
		var->SetAddress(addr);

		// Modify the address by the variables size
		if (var->GetType().array)
			addr += (4 + (var->GetType().elements << 2));
		else
			addr += 4;

		// Check for the maximum size
		if (addr > max_addr)
			max_addr = addr;

		// Check for a string, their value points to a literal so they need shifting
		if (var->GetType().id == VARIABLE_TYPEID_STRING)
		{
			int		def;

			// Array
			if (var->GetType().array)
			{
				// For every element in the array
				for (x = 0; x < var->GetType().elements; x++)
				{
					var->GetElementValue(x, &def);
					def += value;
					var->SetElementValue(x, &def);
				}
			}

			// Single
			else
			{
				var->GetDefaultValue(&def);
				def += value;
				var->SetDefaultValue(&def);
			}
		}

		nb_variables++;
	}

	// Enumerate all the string literals in this class and shift them
	while (var = (Variable *)string_literals.Enumerate())
	{
		// Set the new shifted address
		addr = var->GetAddress() + value;
		var->SetAddress(addr);

		// Modify the address by the variables size
		addr += (strlen(var->GetName()) + 1);

		// Check for the maximum size
		if (addr > max_addr)
			max_addr = addr;
	}

	// Shift the current address up too so the data segment size can be determined
	cur_address += value;

	// Now shift each child class
	for (x = 0; x < children.GetPosition(); x++)
		children(x)->ShiftVariableAddresses(value + max_addr);

	outfunc;
}


void Class::ManageEntryPoint(int value)
{
	infunc(Class::ManageEntryPoint);

	int		x;

	// If this class has no entry point, inherit it from above
	if (entry_point == -1)
		entry_point = value;

	// If this is a non-abstract class with no defined entry point
	if (!(flags & (CLASS_FLAGS_ABSTRACT | CLASS_FLAGS_INTERFACE)) && entry_point == -1)
		throw CompileError("Class '%s' has no defined constructor entry point", GetName());

	// Work on the children
	for (x = 0; x < children.GetPosition(); x++)
		children(x)->ManageEntryPoint(entry_point);

	outfunc;
}


Function *Class::GetLastFuncDecl(char *state, char *name)
{
	infunc(Class::GetLastFuncDecl);

	Function	*func = NULL;
	State		*state_ptr;

	// Need a super-class for all this
	if (sclass_ptr)
	{
		// Grab the function from the super-class or a state within the super-class
		if (state == NULL)
			func = (Function *)sclass_ptr->functions.GetEntry(name);
		else
		{
			state_ptr = (State *)sclass_ptr->states.GetEntry(state);

			if (state_ptr)
				func = (Function *)state_ptr->functions.GetEntry(name);
		}

		// If the function wasn't found in the super-class, perhaps it's further up
		if (func == NULL)
			func = sclass_ptr->GetLastFuncDecl(state, name);
	}

	return (func);

	outfunc;
}


void Class::ResolveFuncOverwrites(void)
{
	infunc(Class::ResolveFuncOverwrites);

	int			x;
	Function	*func;
	State		*state;

	// Need a super-class for all this
	if (sclass_ptr)
	{
		// Enumerate all the functions in this class
		while (func = (Function *)functions.Enumerate())
		{
			Function	*tempf;

			// See if this function has a previous definition
			if (tempf = GetLastFuncDecl(NULL, func->GetName()))
			{
				// Check for an error
				if (tempf->GetFlags() & FFLAGS_FINAL)
					throw CompileError("(Class %s) Cannot over-write final function '%s'", GetName(), func->GetName());

				// Set the new index for this coolio virtual function
				func->SetIndex(tempf->GetIndex());
			}
			else
			{
				// Doesn't exist in the super-class, this is a unique entry to the vtable
				nb_vt_entries++;
			}
		}

		// Enumerate all the states in this class
		while (state = (State *)states.Enumerate())
		{
			// Enumerate all the functions in this state
			while (func = (Function *)state->functions.Enumerate())
			{
				Function	*tempf;

				// See if this function has a previous definition
				if (tempf = GetLastFuncDecl(state->GetName(), func->GetName()))
				{
					// Check for an error
					if (tempf->GetFlags() & FFLAGS_FINAL)
						throw CompileError("(Class %s) Cannot over-write final function '%s'", GetName(), func->GetName());

					// Set the new index for this coolio virtual function
					func->SetIndex(tempf->GetIndex());
				}
				else
				{
					// Doesn't exist in the super-class, this is a unique entry to the vtable
					nb_vt_entries++;
				}
			}
		}
	}
	else
	{
		// No super-class, vtable entries is normal
		nb_vt_entries = nb_functions;
	}

	// Continue through and work on the child classes
	for (x = 0; x < children.GetPosition(); x++)
		children(x)->ResolveFuncOverwrites();

	outfunc;
}


Function *Class::GetLastImpDecl(char *name)
{
	infunc(Class::GetLastImpDecl);

	Function	*func = NULL;

	// Need a super-class for all this
	if (sclass_ptr)
	{
		// Grab the function from the super-class
		func = (Function *)sclass_ptr->imp_functions.GetEntry(name);

		// If the function wasn't found in the super-class, perhaps it's further up
		if (func == NULL)
			func = sclass_ptr->GetLastImpDecl(name);
	}

	return (func);

	outfunc;
}


void Class::ResolveImpOverwrites(void)
{
	infunc(Class::ResolveImpOverwrites);

	int			x;
	Function	*func;

	// Need a super-class for all this
	if (sclass_ptr)
	{
		// Enumerate all the imported functions in this class
		while (func = (Function *)imp_functions.Enumerate())
		{
			Function	*tempf;

			// See if this imported function has a previous definition
			if (tempf = GetLastImpDecl(func->GetName()))
			{
				// Check for an error
				if (tempf->GetFlags() & FFLAGS_FINAL)
					throw CompileError("(Class %s) Cannot over-write final function '%s'", GetName(), func->GetName());

				// Set the new index for this coolio imported virtual function
				func->SetIndex(tempf->GetIndex());
			}
			else
			{
				// Doesn't exist in the super-class, this is a unique entry to the itable
				nb_it_entries++;
			}
		}
	}
	else
	{
		// No super-class, itable entries is normal
		nb_it_entries = nb_impfuncs;
	}

	// Continue through and work on the child classes
	for (x = 0; x < children.GetPosition(); x++)
		children(x)->ResolveImpOverwrites();

	outfunc;
}


State *Class::GetLastStateDecl(char *state)
{
	infunc(Class::GetLastStateDecl);

	State	*state_ptr = NULL;

	// There needs to be a super-class to explore
	if (sclass_ptr)
	{
		// Search for the state in the super-class
		state_ptr = (State *)sclass_ptr->states.GetEntry(state);

		// If the state wasn't found in the super-class, perhaps it is further up
		if (state_ptr == NULL)
			state_ptr = sclass_ptr->GetLastStateDecl(state);
	}

	return (state_ptr);

	outfunc;
}


void Class::ResolveStateOverwrites(void)
{
	infunc(Class::ResolveStateOverwrites);

	int		x;
	State	*state, *super;

	// Need a super-class
	if (sclass_ptr)
	{
		// Enumerate all the states in this class
		while (state = (State *)states.Enumerate())
		{
			// See if it exists in a super-class
			if (super = GetLastStateDecl(state->GetName()))
				state->SetIndex(super->GetIndex());
			else
				nb_st_entries++;
		}
	}
	else
	{
		// No super-class, number of states is normal
		nb_st_entries = nb_states;
	}

	// Resolve for the children
	for (x = 0; x < children.GetPosition(); x++)
		children(x)->ResolveStateOverwrites();

	outfunc;
}


void Class::BuildVTable(Class *parent)
{
	infunc(Class::BuildVTable);

	Function	*func;
	State		*state;
	int			x;

	// If this class has a parent, inherit the vtable from above
	if (parent)
	{
		for (x = 0; x < parent->nb_vt_entries; x++)
			vtable.Add(parent->vtable(x));

		nb_vt_entries += parent->nb_vt_entries;
	}

	// Enumerate each function in this class
	while (func = (Function *)functions.Enumerate())
	{
		// Grow the vtable if it's too small
		if (func->GetIndex() >= vtable.GetSize())
			vtable.Grow(func->GetIndex() + 1);

		// Fill in the vtable entry
		vtable(func->GetIndex()) = func;
	}

	// Enumerate each state in this class
	while (state = (State *)states.Enumerate())
	{
		// Enumerate each function in this state
		while (func = (Function *)state->functions.Enumerate())
		{
			// Grow the vtable if it's too small
			if (func->GetIndex() >= vtable.GetSize())
				vtable.Grow(func->GetIndex() + 1);

			// Fill in the vtable entry
			vtable(func->GetIndex()) = func;
		}
	}

	// Work on the children classes
	for (x = 0; x < children.GetPosition(); x++)
		children(x)->BuildVTable(this);

	outfunc;
}


void Class::BuildITable(Class *parent)
{
	infunc(Class::BuildITable);

	Function	*func;
	int			x;

	// If this class has a parent, inherit the itable from above
	if (parent)
	{
		for (x = 0; x < parent->nb_it_entries; x++)
			itable.Add(parent->itable(x));

		nb_it_entries += parent->nb_it_entries;
	}

	// Enumerate each imported function in this class
	while (func = (Function *)imp_functions.Enumerate())
	{
		// Grow the itable if it's too small
		if (func->GetIndex() >= itable.GetSize())
			itable.Grow(func->GetIndex() + 1);

		// Fill in the itable entry
		itable(func->GetIndex()) = func;
	}

	// Work on the children classes
	for (x = 0; x < children.GetPosition(); x++)
		children(x)->BuildITable(this);

	outfunc;
}


void Class::BuildSTable(Class *parent)
{
	infunc(Class::BuildSTable);

	State	*state;
	int		x;

	// If this class has a parent, inherit the stable from above
	if (parent)
	{
		for (x = 0; x < parent->nb_states; x++)
			stable.Add(parent->stable(x));

		nb_st_entries += parent->nb_st_entries;
	}

	// Enumerate each state in this class
	while (state = (State *)states.Enumerate())
	{
		// Grow the state table if it's too small
		if (state->GetIndex() >= stable.GetSize())
			stable.Grow(state->GetIndex());

		// Fill in the entry
		stable(state->GetIndex()) = state;
	}

	// Work on the children classes
	for (x = 0; x < children.GetPosition(); x++)
		children(x)->BuildSTable(this);

	outfunc;
}


void Class::UpdateCodeAddresses(int position)
{
	infunc(Class::UpdateCodeAddresses);

	Function	*func_ptr;
	State		*state_ptr;

	// For every function, shift the code position
	while (func_ptr = (Function *)functions.Enumerate())
		func_ptr->SetCodeLocation(func_ptr->GetCodeLocation() + position);

	// For every state in the class
	while (state_ptr = (State *)states.Enumerate())
	{
		// State position
		state_ptr->SetPosition(state_ptr->GetPosition() + position);

		// Location of the 'begin' label
		if (state_ptr->GetCodeStart() != -1)
			state_ptr->SetCodeStart(state_ptr->GetCodeStart() + position);

		// For every function in the state, shift the code position
		while (func_ptr = (Function *)state_ptr->functions.Enumerate())
			func_ptr->SetCodeLocation(func_ptr->GetCodeLocation() + position);
	}

	outfunc;
}


State *Class::GetState(char *name)
{
	infunc(Class::GetState);
	return ((State *)states.GetEntry(name));
	outfunc;
}


void Class::Write(CFile *file)
{
	infunc(Class::Write);

	Function		*func_ptr;
	State			*state;
	Variable		*var;
	int				x, position, info;

	// Get the beginning of where the class information is stored
	info = file->GetPosition();

	// Class name
	file->PrintfZ(GetName());

	// Whether the class inherits
	file->WriteByte(inherits);

	// Write the super class if it inherits
	if (inherits)
		file->PrintfZ(super_class);

	// ---------- Variable Information

	position = file->GetPosition();
	file->WriteDWordLoop(0, 1);
	x = 0;

	while ((var = (Variable *)variables.Enumerate()))
	{
		var->Write(file);
		x++;
	}

	WriteValue(file, x, position);

	// ---------- Function Information

	position = file->GetPosition();
	file->WriteDWordLoop(0, 1);
	x = 0;

	while ((func_ptr = (Function *)functions.Enumerate()))
	{
		func_ptr->Write(file);
		x++;
	}

	WriteValue(file, x, position);

	// ---------- Imported Function Information

/*	position = file->GetPosition();
	file->WriteDWordLoop(0, 1);
	x = 0;

	while ((func_ptr = (Function *)imp_functions.Enumerate()))
	{
		func_ptr->Write(file);
		x++;
	}

	WriteValue(file, x, position);*/

	// ---------- State Information

	position = file->GetPosition();
	file->WriteDWordLoop(0, 1);
	x = 0;

	while ((state = (State *)states.Enumerate()))
	{
		state->Write(file);
		x++;
	}

	WriteValue(file, x, position);

	// Write the class flags
	file->Write(&flags, sizeof(int));

	// Write the class entry point
	file->Write(&entry_point, sizeof(int));

	// ---------- String Literal Information

	position = file->GetPosition();
	file->WriteDWordLoop(0, 1);
	x = 0;

	while ((var = (Variable *)string_literals.Enumerate()))
	{
		int		value;

		// Address of string literal within the data segment
		value = var->GetAddress();
		file->Write(&value, sizeof(int));

		// Length of string literal (plus NULL)
		value = strlen(var->GetName()) + 1;
		file->Write(&value, sizeof(int));

		// String literal itself
		file->PrintfZ(var->GetName());

		x++;
	}

	WriteValue(file, x, position);

	// Write where the class info is located
	file->Write(&info, 4);

	outfunc;
}


void Class::Read(CFile *file)
{
	infunc(Class::Read);

	int		num, x;
	char	string[MAX_LABEL_SIZE];

	// Class name
	file->ReadStringZ(string);
	SetName(string);

	// Whether the class inherits
	inherits = file->ReadByte();

	// Read the super-class if it inherits
	if (inherits)
		file->ReadStringZ(super_class);

	// Number of variables
	file->Read(&num, 4);

	// For every variable
	for (x = 0; x < num; x++)
	{
		Variable	*var;

		// Allocate some space
		if ((var = new Variable) == NULL)
			throw CError("Couldn't allocate variable");

		// Read the variable from file
		var->Read(file);

		// Add to the list
		variables.Add(var->GetName(), var);
	}

	// Number of functions
	file->Read(&num, 4);

	// For every function
	for (x = 0; x < num; x++)
	{
		Function	*func;

		// Allocate some space
		if ((func = new Function) == NULL)
			throw CError("Couldn't allocate function");

		// Read the function from file
		func->Read(file);

		// Add to the list
		functions.Add(func->GetName(), func);
	}

	// Number of imported functions
	file->Read(&num, 4);

	// For every imported function
/*	for (x = 0; x < num; x++)
	{
		Function	*func;

		// Allocate some space
		if ((func = new Function) == NULL)
			throw CError("Couldn't allocate function");

		// Read the function from file
		func->Read(file);

		// Add to the list
		imp_functions.Add(func->GetName(), func);
	}*/

	// Number of states
	file->Read(&num, 4);

	// For every state
	for (x = 0; x < num; x++)
	{
		State	*state;

		// Allocate some space
		if ((state = new State) == NULL)
			throw CError("Couldn't allocate state");

		// Read the state from file
		state->Read(file);

		// Add to the list
		states.Add(state->GetName(), state);
	}

	// Class entry point
	file->Read(&entry_point, sizeof(int));

	// Class flags
	file->Read(&flags, sizeof(int));

	// Number of string literals
	file->Read(&num, 4);

	// For every string literal
	for (x = 0; x < num; x++)
	{
		Variable	*var;
		int			address, value;
		char		*string;

		// Allocate some space
		if ((var = new Variable) == NULL)
			throw CError("Couldn't allocate variable");

		// Read the variable address and set some info
		file->Read(&address, sizeof(int));
		var->SetAddress(address);
		var->MakeType(VARIABLE_TYPEID_STRINGLIT);

		// Read the size of the string
		file->Read(&value, sizeof(int));

		// Allocate and read the string
		if ((string = new char[value]) == NULL)
			throw CError("Couldn't allocate string");
		
		file->ReadStringZ(string);

		// Add it to the string literal list
		string_literals.Add(string, var);

		// Release the string
		delete string;
	}

	outfunc;
}


void Class::WriteVMUInfo(CFile *file)
{
	infunc(Class::WriteVMUInfo);

	Variable	*var;
	Function	*func;
	State		*state;
	int			position, num, x;

	// Class name
	file->PrintfZ(GetName());

	// Whether the class inherits
	file->WriteByte(inherits);

	// Write the super class if it inherits
	if (inherits)
		file->PrintfZ(super_class);

	// Remember where the number of variables is stored
	position = file->GetPosition();
	file->WriteDWordLoop(0, 1);
	num = 0;

	// Enumerate all the variables in the class, writing them
	while (var = (Variable *)variables.Enumerate())
	{
		var->WriteVMUInfo(file);
		num++;
	}

	// Write the number of variables found
	g_Unit->BackpatchInt(num, position);

	// Remember where the number of functions is stored
	position = file->GetPosition();
	file->WriteDWordLoop(0, 1);
	num = 0;

	// Enumerate all the functions in the class, writing them
	while (func = (Function *)functions.Enumerate())
	{
		func->WriteVMUInfo(file);
		num++;
	}

	// Write the number of functions found
	g_Unit->BackpatchInt(num, position);

	// Write the number of imported functions
	file->WriteDWordLoop(nb_it_entries, 1);

	// Write the import table to file
	for (x = 0; x < nb_it_entries; x++)
		itable(x)->WriteVMUInfo(file);

	// Remember where the number of states is stored
	position = file->GetPosition();
	file->WriteDWordLoop(0, 1);
	num = 0;

	// Enumerate all the states in the class, writing them
	while (state = (State *)states.Enumerate())
	{
		state->WriteVMUInfo(file);
		num++;
	}

	// Write the number of states found
	g_Unit->BackpatchInt(num, position);

	// Write the class flags
	file->Write(&flags, sizeof(int));

	// Write the class entry point
	file->Write(&entry_point, sizeof(int));

	// Write the virtual function table
	file->Write(&nb_vt_entries, sizeof(int));
	for (x = 0; x < nb_vt_entries; x++)
	{
		num = vtable(x)->GetCodeLocation();
		file->Write(&num, sizeof(int));
	}

	// Write the virtual state table
	file->Write(&nb_st_entries, sizeof(int));
	for (x = 0; x < nb_st_entries; x++)
	{
		num = stable(x)->GetCodeStart();
		file->Write(&num, sizeof(int));
	}

	outfunc;
}


void Class::WriteDataSegment(char *data)
{
	infunc(Class::WriteDataSegment);

	Variable	*var;

	// If there is a super-class, write that data first
	if (sclass_ptr)
		sclass_ptr->WriteDataSegment(data);

	// Enumerate all the variables in this class
	while (var = (Variable *)variables.Enumerate())
	{
		VarType	type = var->GetType();

		// Array
		if (type.array)
		{
			int		x;

			// Set the array pointer
			*(int *)(&data[var->GetAddress()]) = var->GetAddress() + 4;

			// Write the default array values
			for (x = 0; x < type.elements; x++)
				var->GetElementValue(x, &data[var->GetAddress() + 4 + (x * 4)]);
		}

		// Single
		else
		{
			var->GetDefaultValue(&data[var->GetAddress()]);
		}
	}

	// Enumerate all string literals
	while (var = (Variable *)string_literals.Enumerate())
	{
		// String print
		strcpy(&data[var->GetAddress()], var->GetName());
	}

	outfunc;
}