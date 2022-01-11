#pragma once
class DlightManager
{
private:
	int assignKey = 69;
	int numLights = 0;
	// the ones in the list
	// use this 
	std::vector<dlight_t> storedLights;
	// use this for rendering
	std::vector<dlight_t*> displayedLights;
	void CopyValuesOver(dlight_t* dst, dlight_t* src);
public:
	void Run();
	void Save();
};

extern DlightManager* dlightManager;
