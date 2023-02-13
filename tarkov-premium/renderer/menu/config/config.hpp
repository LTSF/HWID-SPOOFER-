#pragma once

#include <windows.h>
#include <string_view>
#include <unordered_map>

#include <variant>
#include <fstream>

#include "../../../engine/structures/structures.hpp"

namespace menu::config
{
	extern std::unordered_map< const char*, std::variant< bool, int, float, engine::structures::vector4_t > > config_map;
}