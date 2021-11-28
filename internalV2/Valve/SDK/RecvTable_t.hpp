// receive data table
struct RecvTable_t
{
	RecvProp_t* pProps;
	int				nProps;
	void* pDecoder;
	char* szNetTableName;
	bool			bInitialized;
	bool			bInMainList;
};