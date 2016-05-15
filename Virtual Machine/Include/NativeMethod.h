
#ifndef	_INCLUDED_NATIVEMETHOD_H
#define	_INCLUDED_NATIVEMETHOD_H


class NativeMethod : public CHashCell
{
public:
	// Method index in the virtual function table
	int		m_Index;

	// Method address in the C++ code
	int		m_Address;

	// Pointer to the import details
	VMImport	*m_Import;
};


#endif	/* _INCLUDED_NATIVEMETHOD_H */