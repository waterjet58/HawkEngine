#pragma once

#include "GLFW/glfw3.h"

namespace Hawk {
	class GraphicsContext
	{
	private:


	public:
		virtual void init(uint32_t width, uint32_t height, GLFWwindow* window) = 0;
		virtual void cleanup() = 0; 
		//virtual void SwapBuffers() = 0;
	};
}