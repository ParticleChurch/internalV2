
struct cplane_t
{
	Vector		vecNormal;
	float		flDistance;
	std::byte	dType;
	std::byte	dSignBits;
	std::byte	pad[0x2];
};