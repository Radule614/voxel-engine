#pragma once

#include <glm/glm.hpp>
#include "../Utils/Utils.hpp"
#include <string>

namespace VoxelEngine
{
struct Position3D
{
	int32_t x;
	int32_t y;
	int32_t z;

	Position3D() : x(0), y(0), z(0)
	{
	}

	Position3D(int32_t x, int32_t y, int32_t z) : x(x), y(y), z(z)
	{
	}

	bool operator<(const Position3D& pos) const noexcept
	{
		if (y != pos.y)
			return y < pos.y;
		if (z != pos.z)
			return z < pos.z;
		return x < pos.x;
	}

	bool operator==(const Position3D& pos) const noexcept
	{
		return x == pos.x && y == pos.y;
	}

	Position3D operator+(Position3D const& other)
	{
		Position3D res;
		res.x = x + other.x;
		res.y = y + other.y;
		res.z = z + other.z;
		return res;
	}

	inline double_t GetLength() const
	{
		return glm::sqrt(glm::pow(x, 2) + glm::pow(y, 2) + glm::pow(z, 2));
	}

	operator glm::vec3() const
	{
		return glm::vec3(x, y, z);
	}

	std::string ToString() const
	{
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
	}
};

};

namespace std
{
using namespace VoxelEngine;
template<>
struct std::hash<Position3D>
{
	std::size_t operator()(const Position3D& p) const
	{
		return Cantor(p.x, Cantor(p.y, p.z));
	}
};
};