#pragma once

using BYTE		= unsigned char;
using int8		= __int8;
using int16		= __int16;
using int32		= __int32;
using int64		= __int64;
using uint8		= unsigned __int8;
using uint16	= unsigned __int16;
using uint32	= unsigned __int32;
using uint64	= unsigned __int64;
using float32	= float;
using float64	= double;


struct Point
{

	Point() = default;

	Point(int32 y, int32 x)
		: y(y)
		, x(x)
	{
		
	}

	Point(const Point& other)
		: y(other.y),
		  x(other.x)
	{
	}

	Point& operator=(const Point& other)
	{
		if (this == &other)
			return *this;
		y = other.y;
		x = other.x;
		return *this;
	}

	friend bool operator==(const Point& lhs, const Point& rhs)
	{
		return lhs.y == rhs.y
			&& lhs.x == rhs.x;
	}

	friend bool operator!=(const Point& lhs, const Point& rhs)
	{
		return !(lhs == rhs);
	}

	int y;
	int x;
};