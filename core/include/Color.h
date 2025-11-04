#pragma once

#include "Vector.h"

struct Rgba
{
	int R;
	int G;
	int B;
	int A;

	Rgba() = default;
	Rgba(int R, int G, int B, int A) : R(R), G(G), B(B), A(A)
	{
	}

	Vector4 ToVector4()
	{
		const float RGB_MAX_VALUE = 255.0f;
        const float ALPHA_MAX_VALUE = 1.0f;
		return Vector4(static_cast<float>(R) / RGB_MAX_VALUE, static_cast<float>(G) / RGB_MAX_VALUE,
		               static_cast<float>(B) / RGB_MAX_VALUE, static_cast<float>(A) / ALPHA_MAX_VALUE);
	}
};
