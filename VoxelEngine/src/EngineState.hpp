#pragma once

struct EngineState
{
	bool MenuActive = false;
	GLCore::Utils::PerspectiveCameraController CameraController;

	//TEMP
	GLCore::Utils::TextureManager TextureManager;
};