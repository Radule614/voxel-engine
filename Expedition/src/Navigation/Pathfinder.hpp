#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace Expedition
{

class Pathfinder
{
public:
    // Returns world-space waypoints from start to goal. Empty = no path found.
    std::vector<glm::vec3> FindPath(glm::vec3 start, glm::vec3 goal, float maxStepHeight = 1.5f);

private:
    bool GetGroundHeight(float worldX, float worldZ, float& outY) const;
    bool IsBlocked(glm::vec3 from, glm::vec3 to) const;

    static constexpr float CellSize  = 1.0f;
    static constexpr int   MaxSearch = 500;
    static constexpr float RayStartY = 200.0f;
    static constexpr float RayLength = 250.0f;
};

}
