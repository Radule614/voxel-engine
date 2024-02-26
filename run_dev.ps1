Remove-Item .\build\Debug -Recurse

cmake -S . -B .\build
cmake --build .\build

.\build\Debug\VoxelEngine.exe
