#pragma once
class Chams
{
private:
	void OverideMat(bool ignorez, int material, float transparent, Color rgba, void* thisptr, void* ctx, void* state, const ModelRenderInfo& info, matrix3x4_t* customBoneToWorld);
	Material* flat;
	Material* normal;
	Material* animated;
	Material* glass;
	Material* crystal;
	Material* chrome;
	Material* pearlescent;
	Material* glow;

	Material* GetCurMaterial(int index)
	{
		switch (index) {
		case 0:
			return flat;
			break;
		case 1:
			return normal;
			break;
		case 2:
			return glow;
			break;
		case 3:
			return animated;
			break;
		case 4:
			return glass;
			break;
		case 5:
			return crystal;
			break;
		case 6:
			return chrome;
			break;
		default:
			return flat;
			break;
		}
	}
public:
	void Init();
	void Run(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo& info, matrix3x4_t* customBoneToWorld);
};

extern Chams* chams;