//
// Created by RadU on 10/14/2025.
//

#pragma once

#include "GLCore/Core/Application.hpp"
#include "EngineState.hpp"

namespace VoxelEngine
{

class VoxelEngineApp : public GLCore::Application
{
public:
    VoxelEngineApp();
    ~VoxelEngineApp() override;

    void Run() override;

protected:
    virtual void Setup() = 0;
    void Init();
    static void Shutdown();

protected:
    EngineState m_State;
};

}
