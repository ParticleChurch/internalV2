using ArrayLengthProxyFn = void(__cdecl*)(void*, int, int);
using RecvVarProxyFn = void(__cdecl*)(const CRecvProxyData*, void*, void*);
using DataTableProxyFn = void(__cdecl*)(const RecvProp_t*, void**, void*, int);

// receive prop comes from the receive data table
struct RecvProp_t
{
	char* szVarName;
	ESendPropType		iRecvType;
	int					iFlags;
	int					nStringBufferSize;
	bool				bInsideArray;
	const void* pExtraData;
	RecvProp_t* pArrayProp;
	ArrayLengthProxyFn* oArrayLengthProxyFn;
	RecvVarProxyFn		oProxyFn;
	DataTableProxyFn* oDataTableProxyFn;
	RecvTable_t* pDataTable;
	int					iOffset;
	int					iElementStride;
	int					iElements;
	const char* szParentArrayPropName;
};
