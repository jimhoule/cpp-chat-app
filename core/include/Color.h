#pragma once

#include "Vector.h"

#include <algorithm>

const int RGBA_EMPTY_VALUE = -1;
const int RGBA_MIN_VALUE = 0;
const int RGBA_MAX_VALUE = 255;

struct Rgba
{
  public:
	int R = RGBA_EMPTY_VALUE;
	int G = RGBA_EMPTY_VALUE;
	int B = RGBA_EMPTY_VALUE;
	int A = RGBA_EMPTY_VALUE;

	Rgba() = default;
	Rgba(int R, int G, int B, int A) : R(Clamp(R)), G(Clamp(G)), B(Clamp(B)), A(Clamp(A))
	{
	}

	bool IsEmpty()
	{
		return R == RGBA_EMPTY_VALUE && G == RGBA_EMPTY_VALUE && B == RGBA_EMPTY_VALUE && A == RGBA_EMPTY_VALUE;
	}

	Vector4 ToVector4()
	{
		const float RGBA_MAX_VALUE_FLOAT = static_cast<float>(RGBA_MAX_VALUE);
		return Vector4(static_cast<float>(R) / RGBA_MAX_VALUE_FLOAT, static_cast<float>(G) / RGBA_MAX_VALUE_FLOAT,
		               static_cast<float>(B) / RGBA_MAX_VALUE_FLOAT, static_cast<float>(A) / RGBA_MAX_VALUE_FLOAT);
	}

  private:
	int Clamp(int Value)
	{
		return std::clamp(Value, RGBA_MIN_VALUE, RGBA_MAX_VALUE);
	}
};
