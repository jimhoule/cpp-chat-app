#pragma once

#include "Vector.h"

#include <algorithm>

const int RGBA_EMPTY_VALUE = -1;
const int RGBA_MIN_VALUE = 0;
const int RGBA_MAX_VALUE = 255;

struct Rgba
{
  public:
	int r = RGBA_EMPTY_VALUE;
	int g = RGBA_EMPTY_VALUE;
	int b = RGBA_EMPTY_VALUE;
	int a = RGBA_EMPTY_VALUE;

	Rgba() = default;
	Rgba(int r, int g, int b, int a) : r(Clamp(r)), g(Clamp(g)), b(Clamp(b)), a(Clamp(a))
	{
	}

	bool IsEmpty()
	{
		return r <= RGBA_EMPTY_VALUE && g <= RGBA_EMPTY_VALUE && b <= RGBA_EMPTY_VALUE && a <= RGBA_EMPTY_VALUE;
	}

	Vector4 ToVector4()
	{
		const float RGBA_MAX_VALUE_FLOAT = static_cast<float>(RGBA_MAX_VALUE);
		return Vector4(static_cast<float>(r) / RGBA_MAX_VALUE_FLOAT, static_cast<float>(g) / RGBA_MAX_VALUE_FLOAT,
		               static_cast<float>(b) / RGBA_MAX_VALUE_FLOAT, static_cast<float>(a) / RGBA_MAX_VALUE_FLOAT);
	}

  private:
	int Clamp(int value)
	{
		return std::clamp(value, RGBA_MIN_VALUE, RGBA_MAX_VALUE);
	}
};
