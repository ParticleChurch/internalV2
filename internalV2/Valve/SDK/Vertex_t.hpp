#pragma once
struct Vertex_t
{
	Vertex_t() = default;

	Vertex_t(const Vector2D& vecPosition, const Vector2D& vecCoordinate = Vector2D(0, 0))
	{
		this->vecPosition = vecPosition;
		this->vecCoordinate = vecCoordinate;
	}

	void Init(const Vector2D& vecPosition, const Vector2D& vecCoordinate = Vector2D(0, 0))
	{
		this->vecPosition = vecPosition;
		this->vecCoordinate = vecCoordinate;
	}

	Vector2D vecPosition;
	Vector2D vecCoordinate;
};