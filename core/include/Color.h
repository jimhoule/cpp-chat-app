#pragma once

#include "Vector.h"

struct Rgba
{
    int r;
    int g;
    int b;
    int a;

    Rgba() = default;
    Rgba(int r, int g, int b, int a): r(r), g(g), b(b), a(a) {}

    Vector4 ToVector4()
    {
        const float RGBA_MAX_VALUE = 255.0f;
        return Vector4(r / RGBA_MAX_VALUE, g / RGBA_MAX_VALUE, b / RGBA_MAX_VALUE, a / RGBA_MAX_VALUE);
    }
};
