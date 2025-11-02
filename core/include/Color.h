#pragma once

#include "Vector.h"

struct Rgba
{
	int r;
	int g;
	int b;
	int a;

	Rgba() = default;
	Rgba(int r, int g, int b, int a) : r(r), g(g), b(b), a(a)
	{
	}

	Vector4 ToVector4()
	{
		const float RGBA_MAX_VALUE = 255.0f;
		return Vector4(static_cast<float>(r) / RGBA_MAX_VALUE, static_cast<float>(g) / RGBA_MAX_VALUE,
		               static_cast<float>(b) / RGBA_MAX_VALUE, static_cast<float>(a) / RGBA_MAX_VALUE);
	}
};
