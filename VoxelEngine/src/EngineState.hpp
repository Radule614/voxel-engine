#pragma once
#include "Assets/AssetManager.hpp"

struct EngineState
{
	bool MenuActive = false;
	GLCore::Utils::PerspectiveCameraController CameraController;

	//TEMP
	VoxelEngine::AssetManager AssetManager;
};