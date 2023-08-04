#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <cstring>
#include <cstdlib>
#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <limits>
#include <set>
#include <stdexcept>

#include "Hawk/ECS/Components/Transform.h"
#include "Hawk/ECS/Components/Sprite.h"

#include <glm/gtc/constants.hpp>

#ifdef HWK_PLATFORM_WINDOWS
	#include <Windows.h>
#endif