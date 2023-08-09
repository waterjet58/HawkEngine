#pragma once

namespace Hawk {

	struct Timestep
	{
	public:
		Timestep(float time = 0.f) : _time(time) {}


		operator float() const { return _time; }
		float getSeconds() const { return _time; }
		float getMilliseconds() const { return _time * 1000.f; }

	private:
		float _time;
	};


}