#pragma once


namespace Hawk {
	class GraphicsContext
	{
	private:


	public:
		virtual void init(uint32_t width, uint32_t height) = 0;
		virtual void cleanup() = 0; 
		//virtual void SwapBuffers() = 0;
	};
}