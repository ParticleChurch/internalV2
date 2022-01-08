class ESP
{
private:
	Vector TL;
	Vector BR;
	void CalculateDormant(int userid, bool dormant);
	bool GetBounds(Entity* ent);

	void DrawName(char name[128], int userid);
	void DrawSkeleton(Entity*, int userid);
	void DrawExtrapolatedSkeletons();
	void DrawHealth(int health, int userid);
	void DrawWeapon(Entity*, int userid);
	void DrawProjectiles(Entity* ent);

	
public:
	std::map<int, float> Dormant;
	void RunPaintTraverse();
	void RunEndScene();
};

extern ESP* esp;