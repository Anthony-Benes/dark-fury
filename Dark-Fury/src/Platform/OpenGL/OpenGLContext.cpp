#include "engpch.h"
#include "Platform/OpenGL/OpenGLContext.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Engine {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		ENG_CORE_ASSERT(windowHandle, "Window handle is null!")
	}

	void OpenGLContext::Init()
	{
		ENG_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		ENG_CORE_ASSERT(status, "Failed to initialize Glad!");

		ENG_CORE_INFO("OpenGL Info:");
		ENG_CORE_INFO("    Vendor: %s", glGetString(GL_VENDOR));
		ENG_CORE_INFO("    GPU: %s", glGetString(GL_RENDERER));
		ENG_CORE_INFO("    Version: %s", glGetString(GL_VERSION));

		ENG_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Engine requires at least OpenGL version 4.5!");
	}

	void OpenGLContext::SwapBuffers()
	{
		ENG_PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle);
	}

}
