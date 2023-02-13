#include "no_intertia.hpp"

#include "../../../engine/engine.hpp"

#include "../../../renderer/menu/config/config.hpp"

void modules::weapon::no_intertia::callback()
{
	std::unique_lock<std::mutex> initiate_thread_locker(engine::values::global_locker);

	const auto camera_copy = engine::values::camera;

	const auto local_game_world_copy = engine::values::local_game_world;

	if (!camera_copy.instance || !local_game_world_copy.instance)
		return;

	const auto local_player_copy = local_game_world_copy.get_local_player();

	initiate_thread_locker.unlock();

	if (std::get< bool >(menu::config::config_map["enable_no_intertia"]))
	{
		if (!local_player_copy.instance)
			return;

		if(const auto physical = driver.read(local_player_copy.instance + engine::offsets::player::physical))
		{
			std::printf("setting\n");

			driver.write<float>(physical + engine::offsets::physical::inertia, 0.0f);
		}
	}
}