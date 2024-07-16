#include "GLCore.hpp"
#include "Terrain/VoxelLayer.hpp"

using namespace GLCore;

namespace VoxelEngine
{
    class VoxelEngine : public Application
    {
    public:
        VoxelEngine() : Application("Voxel Engine")
        {
            PushLayer(new VoxelLayer());
        }
    };
}

int main()
{
    VoxelEngine::VoxelEngine app;
    app.Run();
}