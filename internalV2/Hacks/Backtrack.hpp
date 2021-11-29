#pragma once
class Backtrack
{
public:
	int TargEnt;
	LagComp::Record TargTick;
	bool ValidEntFound = false;
	bool ValidTickFound = false;
	void GetTargEnt();
	void GetTargTick();
public:
	void Run();
	Backtrack();
};

extern Backtrack* backtrack;

