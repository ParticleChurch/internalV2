#include "Include.hpp"

namespace N {
	std::vector<RecvTable_t*> Tables;

	void Init()
	{
		Tables = GetTables();
	}

	std::vector<RecvTable_t*> GetTables()
	{
		std::vector<RecvTable_t*> Tables;

		CBaseClient* ClientClass = I::client->GetAllClasses();
		if (!ClientClass)
			return std::vector<RecvTable_t*>();

		while (ClientClass)
		{
			RecvTable_t* recvTable = ClientClass->pRecvTable;
			Tables.push_back(recvTable);

			ClientClass = ClientClass->pNext;
		}

		return Tables;
	}
	RecvTable_t* GetTable(std::vector<RecvTable_t*> Tables, const char* TableName)
	{
		if (Tables.empty())
			return NULL;

		for (unsigned long i = 0; i < Tables.size(); i++)
		{
			RecvTable_t* table = Tables[i];

			if (!table)
				continue;

			if (_strnicmp(table->szNetTableName, TableName, 128) == 0)
				return table;
		}

		return NULL;
	}
	int GetOffset(std::vector<RecvTable_t*> Tables, const char* TableName, const char* TropName)
	{
		int offset = GetRecvProp(Tables, TableName, TropName);
		if (!offset)
			return 0;

		return offset;
	}

	int GetOffset(const char* TableName, const char* TropName)
	{
		return GetOffset(Tables, TableName, TropName);
	}

	int GetRecvProp(std::vector<RecvTable_t*> Tables, const char* TableName, const char* PropName, RecvProp_t** Prop)
	{
		RecvTable_t* Table = GetTable(Tables, TableName);
		if (!Table)
			return 0;

		int offset = GetRecvProp(Tables, Table, PropName, Prop);
		if (!offset)
			return 0;

		return offset;
	}

	int GetRecvProp(std::vector<RecvTable_t*> Tables, RecvTable_t* Table, const char* PropName, RecvProp_t** Prop)
	{
		int ExtraOffset = 0;

		for (int i = 0; i < Table->nProps; ++i) {
			RecvProp_t* RProp = &Table->pProps[i];
			RecvTable_t* Child = RProp->pDataTable;

			if (Child && (Child->nProps > 0))
			{
				int tmp = GetRecvProp(Tables, Child, PropName, Prop);
				if (tmp)
					ExtraOffset += (RProp->iOffset + tmp);
			}

			if (_strnicmp(RProp->szVarName, PropName, 128))
				continue;

			if (Prop)
				*Prop = RProp;

			return (RProp->iOffset + ExtraOffset);
		}

		return ExtraOffset;
	}

	std::string DumpTable(RecvTable_t* Table, int Depth)
	{
		std::string str;

		std::string pre = "";

		for (int i = 0; i < Depth; i++)
			pre.append("\t");

		str.append(pre);
		str.append(Table->szNetTableName);
		str.append("\n");

		for (int i = 0; i < Table->nProps; i++) {
			RecvProp_t* prop = &Table->pProps[i];
			if (!prop)
				continue;

			std::string varName(prop->szVarName);

			if (varName.find("baseclass") == 0 || varName.find("0") == 0 || varName.find("1") == 0 || varName.find("2") == 0)
				continue;

			str.append("\t");
			str.append(pre);
			str.append(varName);
			str.append("\t");
			str.append(std::to_string(prop->iOffset));
			str.append("\n");

			if (prop->pDataTable)
				str.append(DumpTable(prop->pDataTable, Depth + 1));
		}

		return str;
	}

	std::string DumpTable()
	{
		std::string str;
		for (auto a : Tables) {
			str.append("\n");
			str.append(DumpTable(a, 0));
		}
		return str;
	}
}
