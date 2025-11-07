#pragma once

struct Vector2
{
public:
	float X;
	float Y;

	Vector2() = default;
	Vector2(float X, float Y) : X(X), Y(Y)
	{
	}

	Vector2 operator+(Vector2 Addend) const
	{
		return Vector2(X + Addend.X, Y + Addend.Y);
	}

	Vector2 operator-(Vector2 Subtrahend) const
	{
		return Vector2(X - Subtrahend.X, Y - Subtrahend.Y);
	}

	Vector2 operator*(float Multiplier) const
	{
		return Vector2(X * Multiplier, Y * Multiplier);
	}

	Vector2 operator*(Vector2 Multiplier) const
	{
		return Vector2(X * Multiplier.X, Y * Multiplier.Y);
	}

	Vector2 operator/(float Divisor) const
	{
		return Vector2(X / Divisor, Y / Divisor);
	}

	Vector2 operator/(Vector2 Divisor) const
	{
		return Vector2(X / Divisor.X, Y / Divisor.Y);
	}
};

struct Vector4
{
public:
	float X;
	float Y;
	float Z;
	float W;

	Vector4() = default;
	Vector4(float X, float Y, float Z, float W) : X(X), Y(Y), Z(Z), W(W)
	{
	}

	Vector4 operator+(Vector4 Addend) const
	{
		return Vector4(X + Addend.X, Y + Addend.Y, Z + Addend.Z, W + Addend.W);
	}

	Vector4 operator-(Vector4 Subtrahend) const
	{
		return Vector4(X - Subtrahend.X, Y - Subtrahend.Y, Z - Subtrahend.Z, W - Subtrahend.W);
	}

	Vector4 operator*(float Multiplier) const
	{
		return Vector4(X * Multiplier, Y * Multiplier, Z * Multiplier, W * Multiplier);
	}

	Vector4 operator*(Vector4 Multiplier) const
	{
		return Vector4(X * Multiplier.X, Y * Multiplier.Y, Z * Multiplier.Z, W * Multiplier.W);
	}

	Vector4 operator/(float Divisor) const
	{
		return Vector4(X / Divisor, Y / Divisor, Z / Divisor, W / Divisor);
	}

	Vector4 operator/(Vector4 Divisor) const
	{
		return Vector4(X / Divisor.X, Y / Divisor.Y, Z / Divisor.Z, W / Divisor.W);
	}
};
