#pragma once

struct Vector2
{
    float x;
    float y;

    Vector2() = default;
    Vector2(float x, float y) : x(x), y(y) {}

    Vector2 operator+(Vector2 addend) const {
        return Vector2(x + addend.x, y + addend.y);
    }

    Vector2 operator-(Vector2 subtrahend) const {
        return Vector2(x - subtrahend.x, y - subtrahend.y);
    }

    Vector2 operator*(float multiplier) const {
        return Vector2(x * multiplier, y * multiplier);
    }

    Vector2 operator/(float divisor) const {
        return Vector2(x / divisor, y / divisor);
    }
};

struct Vector4
{
    float x;
    float y;
    float z;
    float w;

    Vector4() = default;
    Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    Vector4 operator+(Vector4 addend) const {
        return Vector4(x + addend.x, y + addend.y, z + addend.z, w + addend.w);
    }

    Vector4 operator-(Vector4 subtrahend) const {
        return Vector4(x - subtrahend.x, y - subtrahend.y, z - subtrahend.z, w - subtrahend.w);
    }

    Vector4 operator*(float multiplier) const {
        return Vector4(x * multiplier, y * multiplier, z * multiplier, w * multiplier);
    }

    Vector4 operator/(float divisor) const {
        return Vector4(x / divisor, y / divisor, z / divisor, w / divisor);
    }
};
