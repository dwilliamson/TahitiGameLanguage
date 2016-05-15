
#ifndef	_INCLUDED_INTERFACE_H
#define	_INCLUDED_INTERFACE_H


class Interface
{
public:
	// Interface name
	char	m_Name[256];

	// Does this interface inherit?
	int		m_Inherits;

	// Name of the super-interface
	char	m_Super[256];

	// List of methods in the interface
	TArray<Method *>	m_MethodList;

	// List of interfaces that inherit from this one
	TArray<Interface *>	m_Children;
};


#endif	/* _INCLUDED_INTERFACE_H */