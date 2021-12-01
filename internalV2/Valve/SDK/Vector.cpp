
#include "../../Include.hpp"

Vector Vector::Transform(const matrix3x4_t& m)
{
	return Vector{ DotProduct({ m[0][0], m[0][1], m[0][2] }) + m[0][3],
						DotProduct({ m[1][0], m[1][1], m[1][2] }) + m[1][3],
						DotProduct({ m[2][0], m[2][1], m[2][2] }) + m[2][3] };
}