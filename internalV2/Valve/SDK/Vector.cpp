
#include "../../Include.hpp"

Vector Vector::Transform(const matrix3x4_t& m)
{
	Vector a = Vector(m.arrData[0][0], m.arrData[0][1], m.arrData[0][2]);
	Vector b = Vector(m.arrData[1][0], m.arrData[1][1], m.arrData[1][2]);
	Vector d = Vector(m.arrData[2][0], m.arrData[2][1], m.arrData[2][2]);
	L::Debug("doing deh dots");
	return Vector(this->Dot(a) + m.arrData[0][3], this->Dot(b) + m.arrData[1][3], this->Dot(d) + m.arrData[2][3]);
}