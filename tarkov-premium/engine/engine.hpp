#pragma once

#include <mutex>

#include "types/types.hpp"
#include "offsets/offsets.hpp"

namespace engine::values
{
	extern std::mutex global_locker;
	extern std::mutex player_vector_locker;

	extern std::uintptr_t unity_player;

	extern types::game_object_manager_t game_object_manager;
	extern types::camera_t camera;
	extern types::local_game_world_t local_game_world;

	extern std::vector< types::player_t > players;
}

namespace engine::functions
{
	void initiate( );

	void update_players_vector( );
}