#pragma once

#include "GLCore/Core/Window.hpp"
#include <glfw/glfw3.h>

namespace GLCore
{
class WindowsWindow : public Window
{
public:
	WindowsWindow(const WindowProps& props);
	virtual ~WindowsWindow();
	void OnUpdate() override;


	inline uint32_t GetWidth() const override
	{
		return m_Data.Width;
	}
	inline uint32_t GetHeight() const override
	{
		return m_Data.Height;
	}
	inline virtual void* GetNativeWindow() const
	{
		return m_Window;
	}

	inline void SetEventCallback(const EventCallbackFn& callback) override
	{
		m_Data.EventCallback = callback;
	}

	inline void CaptureMouse(bool enabled) override
	{
		glfwSetCursorPos(m_Window, m_Data.Width / 2.0, m_Data.Height / 2.0);
		if (enabled)
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	inline void RaiseEvent(Event& event) override
	{
		m_Data.EventCallback(event);
	}

	void SetVSync(bool enabled) override;
	bool IsVSync() const override;

private:
	virtual void Init(const WindowProps& props);
	virtual void Shutdown();

private:
	GLFWwindow* m_Window;
	struct WindowData
	{
		std::string Title;
		uint32_t Width, Height;
		bool VSync;
		EventCallbackFn EventCallback;
	};
	WindowData m_Data;
};
} 
