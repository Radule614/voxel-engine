#include "GLCore.hpp"
#include "VoxelEngine.hpp"

int main()
{
    std::unique_ptr<VoxelEngine> app = std::make_unique<VoxelEngine>();
    app->Run();
}
