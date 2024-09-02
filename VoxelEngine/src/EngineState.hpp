#pragma once
#include "Assets/AssetManager.hpp"
#include "GLCore/Core/Application.hpp"

struct EngineState
{
	bool MenuActive = false;
	GLCore::Utils::PerspectiveCameraController CameraController;


	//TEMP
	VoxelEngine::AssetManager AssetManager;
	GLCore::Application* Application;
};