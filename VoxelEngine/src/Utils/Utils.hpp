#pragma once

namespace VoxelEngine
{

inline int Cantor(int a, int b)
{
	return (a + b + 1) * (a + b) / 2 + b;
}

}

