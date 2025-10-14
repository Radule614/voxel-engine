//
// Created by RadU on 10/14/2025.
//

#pragma once

#include "GLCore/Core/Application.hpp"
#include "EngineState.hpp"

using namespace GLCore;

namespace VoxelEngine
{

class VoxelEngineApp : public Application
{
public:
    VoxelEngineApp();

    ~VoxelEngineApp() override;

private:
    void Setup();
    void Init();
    static void Shutdown();

private:
    EngineState m_State;
};

}
