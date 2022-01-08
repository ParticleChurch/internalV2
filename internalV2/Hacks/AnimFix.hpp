#pragma once
class AnimFix
{
private:
	void UpdateEntAnim(Entity* ent);
public:
	void RunAnimFix(int stage);
};

extern AnimFix* animfix;

