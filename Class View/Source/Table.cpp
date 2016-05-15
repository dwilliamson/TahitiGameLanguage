
#include "..\include\main.h"


Table::Table(CFile *file, int nb_entries, int type)
{
	int		x;

	m_NbEntries = nb_entries;
	m_Type = type;

	// Add each virtual address to the table
	for (x = 0; x < nb_entries; x++)
	{
		int			address;
		TableEntry	*entry;

		file->Read(&address, sizeof(int));
		entry = new TableEntry(address);
		m_Entries.Add(entry);
	}
}


Table::~Table(void)
{
	int		x;

	// Delete the table entries

	for (x = 0; x < m_NbEntries; x++)
		delete m_Entries(x);
}


void Table::AddToTreeView(int level)
{
	int		x;

	if (!(g_Display & DISPLAY_TABLE))
		return;

	// Add the base icon
	if (m_Type == TABLE_TYPE_FUNCTIONS)
		g_Tree->AddTable("VTable", level, this);
	else
		g_Tree->AddTable("STable", level, this);

	// For every entry
	for (x = 0; x < m_NbEntries; x++)
	{
		char	string[64];

		sprintf(string, "%d - %06d", x, m_Entries(x)->m_Address);
		g_Tree->AddTableEntry(string, level + 1, m_Entries(x));
	}
}


void Table::PrintInfo(void)
{
	// Type
	if (m_Type == TABLE_TYPE_FUNCTIONS)
	{
		g_Edit->Printf("VIRTUAL FUNCTION TABLE\n");
		g_Edit->Printf("\n");
	}
	else
	{
		g_Edit->Printf("VIRTUAL STATE TABLE\n");
		g_Edit->Printf("\n");
	}

	g_Edit->Printf("Number of entries: %d\n", m_NbEntries);
}


void TableEntry::PrintInfo(void)
{
	g_Edit->Printf("TABLE ENTRY\n");
	g_Edit->Printf("\n");
}