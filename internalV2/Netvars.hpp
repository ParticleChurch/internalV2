#pragma once

namespace N {
	extern std::vector<RecvTable_t*> Tables;

	extern void Init();
	extern std::vector<RecvTable_t*> GetTables();
	extern RecvTable_t* GetTable(std::vector<RecvTable_t*> Tables, const char* TableName);
	extern int GetOffset(std::vector<RecvTable_t*> Tables, const char* TableName, const char* PropName);
	extern int GetOffset(const char* TableName, const char* PropName);
	extern int GetRecvProp(std::vector<RecvTable_t*> Tables, const char* TableName, const char* PropName, RecvProp_t** Prop = 0);
	extern int GetRecvProp(std::vector<RecvTable_t*> Tables, RecvTable_t* RecvTable, const char* PropName, RecvProp_t** Prop = 0);
	extern std::string DumpTable();
	extern std::string DumpTable(RecvTable_t* Table, int Depth);
}