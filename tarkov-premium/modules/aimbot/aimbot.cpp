#include "aimbot.hpp"

#include "../../engine/engine.hpp"

#include "../../renderer/renderer.hpp"

#include "../../renderer/menu/config/config.hpp"

#include "../../dependencies/xorstring/xorstring.hpp"
#include "../../settings/settings.hpp"

void modules::aimbot::callback()
{
	while (true)
	{
		if (!settings::values::premium || !std::get< bool >(menu::config::config_map["assist_enable"]))
			continue;

		if (!GetAsyncKeyState(std::get< int >(menu::config::config_map["assist_hotkey"])))
			continue;


		std::unique_lock<std::mutex> initiate_thread_locker(engine::values::global_locker);

		const auto camera_copy = engine::values::camera;

		const auto local_game_world_copy = engine::values::local_game_world;

		initiate_thread_locker.unlock();

		if (!camera_copy.instance || !local_game_world_copy.instance)
			continue;

		std::unique_lock<std::mutex> update_players_locker(engine::values::player_vector_locker);

		const auto players_copy = engine::values::players;

		const auto local_player_copy = local_game_world_copy.get_local_player();

		update_players_locker.unlock();

		engine::types::player_t best_player = { 0 }; float distance = 9999.f;

		for (const auto player : players_copy)
		{
			if (!player.instance)
				continue;

			const auto head_world_position = player.get_bone_transform(engine::structures::bones::head).get_position();

			const auto head_position = camera_copy.world_to_screen(head_world_position);

			if (!head_position.has_value())
				continue;

			const auto head_screen_position = head_position.value();

			static const auto w = lf(GetSystemMetrics)(SM_CXSCREEN), h = lf(GetSystemMetrics)(SM_CYSCREEN);

			const auto screen_distance = std::sqrtf(std::powf(head_screen_position.x - w / 2, 2) + std::powf(head_screen_position.y - h / 2, 2));

			if (screen_distance < distance)
			{
				distance = screen_distance;
				best_player = player;
			}
		}

		if (best_player.instance)
		{
			const auto movement_context = driver.read(local_player_copy.instance + 0x40);

			auto from = driver.read< engine::types::transform_t >(local_player_copy.instance, {0x500, 0xe8, 0x10 }).get_position();

			auto head_world_position = best_player.get_bone_transform(engine::structures::bones::head).get_position();

			engine::structures::vector3_t delta = from - head_world_position; engine::structures::rotator_t ret;

			float length = sqrt(delta.squared());

			ret.y = std::asinf(delta.y / length); ret.x = -std::atan2f(delta.x, -delta.z);

			auto res = ret * 57.29578f;

			driver.write< engine::structures::vector2_t >(movement_context + 0x22C, { res.x, res.y });
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void modules::aimbot::draw_fov()
{
    if (std::get< bool >(menu::config::config_map["assist_show_fov"]) && std::get< int >(menu::config::config_map["assist_fov"]) > 0)
    {
        static const auto w = lf(GetSystemMetrics)(SM_CXSCREEN), h = lf(GetSystemMetrics)(SM_CYSCREEN);

        const auto draw_list = ImGui::GetBackgroundDrawList();

        draw_list->AddCircle({ w / 2.f, h / 2.f }, std::get< int >(menu::config::config_map["assist_fov"]) * 10, ImColor{ std::get< engine::structures::vector4_t >(menu::config::config_map["fov_color"]).color[0], std::get< engine::structures::vector4_t >(menu::config::config_map["fov_color"]).color[1], std::get< engine::structures::vector4_t >(menu::config::config_map["fov_color"]).color[2], std::get< engine::structures::vector4_t >(menu::config::config_map["fov_color"]).color[3] }, std::get< int >(menu::config::config_map["assist_fov_segments"]), 2.f);
    }
}
