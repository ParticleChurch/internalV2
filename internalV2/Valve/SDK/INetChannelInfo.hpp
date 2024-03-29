// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/inetchannelinfo.h
class INetChannelInfo
{
public:
	/*
	enum
	{
		GENERIC = 0,	// must be first and is default group
		LOCALPLAYER,	// bytes for local player entity update
		OTHERPLAYERS,	// bytes for other players update
		ENTITIES,		// all other entity bytes
		SOUNDS,			// game sounds
		EVENTS,			// event messages
		TEMPENTS,		// temp entities
		USERMESSAGES,	// user messages
		ENTMESSAGES,	// entity messages
		VOICE,			// voice data
		STRINGTABLE,	// a stringtable update
		MOVE,			// client move cmds
		STRINGCMD,		// string command
		SIGNON,			// various signondata
		PAINTMAP,		// paintmap data
		ENCRYPTED,		// encrypted data
		TOTAL,			// must be last and is not a real group
	};

	virtual const char* GetName() const = 0;			// get channel name
	virtual const char* GetAddress() const = 0;			// get channel IP address as string
	virtual float		GetTime() const = 0;			// current net time
	virtual float		GetTimeConnected() const = 0;	// get connection time in seconds
	virtual int			GetBufferSize() const = 0;		// netchannel packet history size
	virtual int			GetDataRate() const = 0;		// send data rate in byte/sec

	virtual bool		IsLoopback() const = 0;			// true if loopback channel
	virtual bool		IsTimingOut() const = 0;		// true if timing out
	virtual bool		IsPlayback() const = 0;			// true if demo playback
	virtual float		GetLatency(int iFlow) const = 0; // current latency (RTT), more accurate but jittering
	virtual float		GetAvgLatency(int iFlow) const = 0; // average packet latency in seconds
	virtual float		GetAvgLoss(int iFlow) const = 0; // avg packet loss[0..1]
	virtual float		GetAvgChoke(int iFlow) const = 0; // avg packet choke[0..1]
	virtual float		GetAvgData(int iFlow) const = 0; // data flow in bytes/sec
	virtual float		GetAvgPackets(int iFlow) const = 0; // avg packets/sec
	virtual int			GetTotalData(int iFlow) const = 0; // total flow in/out in bytes
	virtual int			GetTotalPackets(int iFlow) const = 0;
	virtual int			GetSequenceNr(int iFlow) const = 0; // last send seq number
	virtual bool		IsValidPacket(int iFlow, int nFrame) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float		GetPacketTime(int iFlow, int nFrame) const = 0; // time when packet was send
	virtual int			GetPacketBytes(int iFlow, int nFrame, int group) const = 0; // group size of this packet
	virtual bool		GetStreamProgress(int iFlow, int* piReceived, int* piTotal) const = 0; // TCP progress if transmitting
	virtual float		GetTimeSinceLastReceived() const = 0; // get time since last received packet in seconds
	virtual	float		GetCommandInterpolationAmount(int iFlow, int nFrame) const = 0;
	virtual void		GetPacketResponseLatency(int iFlow, int frame_number, int* pnLatencyMsecs, int* pnChoke) const = 0;
	virtual void		GetRemoteFramerate(float* pflFrameTime, float* pflFrameTimeStdDeviation, float* pflFrameStartTimeStdDeviation) const = 0;
	virtual float		GetTimeoutSeconds() const = 0;
	*/
	//std::byte	pad0[0x14];				//0x0000
	//bool		bProcessingMessages;	//0x0014
	//bool		bShouldDelete;			//0x0015
	//std::byte pad1[0x2];
	//int			iOutSequenceNr;			//0x0018 last send outgoing sequence number
	//int			iInSequenceNr;			//0x001C last received incoming sequence number
	//int			iOutSequenceNrAck;		//0x0020 last received acknowledge outgoing sequence number
	//int			iOutReliableState;		//0x0024 state of outgoing reliable data (0/1) flip flop used for loss detection
	//int			iInReliableState;		//0x0028 state of incoming reliable data
	//int			iChokedPackets;			//0x002C number of choked packets
	//std::byte	pad2[0x414];			//0x0030
	float GetLatency(int flow)
	{
		typedef float(__thiscall* ogetLatency)(void*, int);
		return GetVFunc<ogetLatency>(this, 9)(this, flow);
	}

	float GetAvgLatency(int flow)
	{
		typedef float(__thiscall* oGetAvgLatency)(void*, int);
		return GetVFunc<oGetAvgLatency>(this, 10)(this, flow);
	}

	std::byte pad[44];
	int chokedPackets;

	
};