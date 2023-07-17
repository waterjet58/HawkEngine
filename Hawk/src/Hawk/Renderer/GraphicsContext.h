#pragma once

namespace Hawk {
	class GraphicsContext
	{
	private:


	public:
		virtual void Init() = 0;
		virtual void Cleanup() = 0; 
		//virtual void SwapBuffers() = 0;
	};
}