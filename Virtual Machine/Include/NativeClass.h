
#ifndef	_INCLUDED_NATIVECLASS_H
#define	_INCLUDED_NATIVECLASS_H


class NativeClass : public CHashCell
{
public:
	// Constructor
	NativeClass(void);

	// Add an exported method
	void	RegisterMethod(char *name, int address);

	// Set the object allocation routine
	void	SetAllocator(void *(*func)(void))	{ m_Allocate = func; }

	// Set the object release routine
	void	SetDestroyer(void (*func)(void *))	{ m_Destroy = func; }

	// Allocate a new instance
	void	*NewInstance(void);

	// Delete the instance
	void	DeleteInstance(void *instance);

	// Set the class this one inherits from
	void	SetSuperClass(NativeClass *super_class);

	// Get a method
	NativeMethod	*GetMethod(char *name);

	friend class	VMClass;

private:
	// Get the vtable index associated with a method
	int		GetMethodIndex(char *name);

	// Function to allocate the class
	void	*(*m_Allocate)(void);

	// Function to release the class
	void	(*m_Destroy)(void *);

	// List of methods this class is exporting
	TArray<NativeMethod *>	m_MethodList;

	// Pointers to the methods for quick access
	THash<>			m_MethodHash;

	// Super-class this class inherits from
	NativeClass		*m_SuperClass;
};


#endif	/* _INCLUDED_NATIVECLASS_H */