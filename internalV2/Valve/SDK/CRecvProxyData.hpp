class CRecvProxyData
{
public:
	const RecvProp_t* pRecvProp;	// the property it's receiving
	DataVariant_t		Value;		// the value given to you to store
	int					iElement;	// which array element you're getting
	int					nObjectID;	// the object being referred to
};