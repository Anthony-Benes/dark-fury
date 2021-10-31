#pragma once

#include "Engine/Core/PlatformDetection.h"

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <ostream>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Engine/Core/Base.hpp"
#include "Engine/Core/Log.hpp"

#define ENG_PROFILE 0
#include "Engine/Debug/Instrumentor.hpp"

#ifdef ENG_PLATFORM_WINDOWS
	#include <Windows.h>
#endif