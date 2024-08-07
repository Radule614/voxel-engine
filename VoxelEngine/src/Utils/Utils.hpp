#pragma once

namespace VoxelEngine
{

inline int Cantor(int32_t a, int32_t b)
{
	return (a + b + 1) * (a + b) / 2 + b;
}

inline bool InRange(int32_t v, int32_t l, int32_t r)
{
	return l <= v && v <= r;
}

}

