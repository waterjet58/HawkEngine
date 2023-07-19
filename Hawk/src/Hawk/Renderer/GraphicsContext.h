#pragma once

namespace Hawk {
	class GraphicsContext
	{
	private:


	public:
		virtual void init() = 0;
		virtual void cleanup() = 0; 
		//virtual void SwapBuffers() = 0;
	};
}