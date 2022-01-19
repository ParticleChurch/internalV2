#pragma once
struct ModelRenderInfo {
	Vector origin;
	Vector angles;
	char pad[4];
	void* renderable;
	const Model_t* model;
	const matrix3x4_t* modelToWorld;
	const matrix3x4_t* lightingOffset;
	const Vector* lightingOrigin;
	int flags;
	int entityIndex;
};
