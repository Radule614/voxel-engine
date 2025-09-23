//
// Created by RadU on 9/21/2025.
//

#include "TreeGenerator.hpp"
#include "../Structures/Tree.hpp"

namespace VoxelEngine
{

void TreeGenerator::Generate(const GenerationContext& context, std::vector<Structure>& output) const
{
    auto position = context.ChunkPosition;

    int32_t i = 0;
    double_t treeChance = context.PerlinEngine.octave2D_01(static_cast<double_t>(position.x) + i,
                                                           static_cast<double_t>(position.y) + i,
                                                           2);
    while (treeChance > 0.55 && output.size() < 2)
    {
        const int32_t random = context.PerlinEngine.octave2D_01(static_cast<double_t>(position.x) + i,
                                                                static_cast<double_t>(position.y) + i,
                                                                2) * CHUNK_WIDTH * CHUNK_WIDTH;
        size_t x = random / CHUNK_WIDTH;
        size_t z = random % CHUNK_WIDTH;
        ++i;
        if (!InRange(x, 0, CHUNK_WIDTH - 1) || !InRange(z, 0, CHUNK_WIDTH - 1))
            continue;
        bool isValid = true;
        for (auto& s: output)
        {
            Position2D p(s.GetRoot().GetPosition().GetX() - x, s.GetRoot().GetPosition().GetZ() - z);
            if (p.GetLength() < s.GetRadius())
            {
                isValid = false;
                break;
            }
        }
        if (isValid)
        {
            if (treeChance > 0.7)
                output.push_back(LargeTree(Position3D(x, context.HeightMap[x][z], z)));
            else
                output.push_back(Tree(Position3D(x, context.HeightMap[x][z], z)));
        }
    }
}

}
