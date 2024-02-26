#include "GLCore.h"
#include "VoxelEngine.h"

int main()
{
    std::unique_ptr<VoxelEngine> app = std::make_unique<VoxelEngine>();
    app->Run();
}
