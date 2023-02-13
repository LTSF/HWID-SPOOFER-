#include "visuals.hpp"

#include <mutex>

#include "../../renderer/renderer.hpp"
#include <cmath>
#include "../../renderer/menu/config/config.hpp"

void modules::visuals::callback()
{
	std::unique_lock<std::mutex> initiate_thread_locker(engine::values::global_locker);

	const auto camera_copy =  engine::values::camera;

	const auto local_game_world_copy = engine::values::local_game_world;

	initiate_thread_locker.unlock();

	if ( !camera_copy.instance || !local_game_world_copy.instance )
		return;

	std::unique_lock<std::mutex> update_players_locker( engine::values::player_vector_locker );

	const auto players_copy = engine::values::players;

	const auto local_player_copy = local_game_world_copy.get_local_player();

	update_players_locker.unlock();

	for ( const auto player : players_copy )
	{
		if ( !player.instance )
			continue;

		const auto root_world_position = player.get_bone_transform( engine::structures::bones::root ).get_position( );

		const auto head_world_position = player.get_bone_transform( engine::structures::bones::head ).get_position( );

		const auto root_postion = camera_copy.world_to_screen( root_world_position );

		const auto head_position = camera_copy.world_to_screen( head_world_position );

		const auto draw_list = ImGui::GetBackgroundDrawList( );

		if ( !root_postion.has_value() || !head_position.has_value())
			continue;

		const auto root_screen_position = root_postion.value();
		const auto head_screen_position = head_position.value();

		const float width = std::fabs( head_screen_position.y - root_screen_position.y ) / 4.0f;
		const float height = std::fabs( head_screen_position.y - root_screen_position.y ) * 1.2f;

		const auto line_len_width = width / 2.5f;

		auto [current_health, max_health] = player.get_health_controller().get_health();

		const auto new_width_health = (width) / max_health * current_health;

		const auto root_world_position_lp = local_player_copy.get_bone_transform( engine::structures::bones::root ).get_position( ); //todo change to origin

		const auto distance = static_cast<std::uint32_t>( root_world_position.distance( root_world_position_lp ) );

		if (distance > std::get< int >(menu::config::config_map["display_max_distance"]))
			continue;

		if (std::get< bool >(menu::config::config_map["display_health"]))
		{
			std::string to_draw; to_draw.append(std::to_string(static_cast<int>(current_health))); to_draw.append("HP");

			{
				draw_list->AddRectFilled({ root_screen_position.x + new_width_health, (root_screen_position.y + (height / 10.f) * 1.2f) - .5f }, { root_screen_position.x - new_width_health, (root_screen_position.y + (height / 10.f)) - .5f }, ImColor{ std::get< engine::structures::vector4_t >(menu::config::config_map["health_color"]).color[0], std::get< engine::structures::vector4_t >(menu::config::config_map["health_color"]).color[1], std::get< engine::structures::vector4_t >(menu::config::config_map["health_color"]).color[2], std::get< engine::structures::vector4_t >(menu::config::config_map["health_color"]).color[3] });
			}

			{
				draw_list->AddRect({ root_screen_position.x + new_width_health, (root_screen_position.y + (height / 10.f) * 1.2f) - .5f }, { root_screen_position.x - new_width_health, (root_screen_position.y + (height / 10.f)) - .5f }, ImColor{ 1, 1, 1 });
			}

			draw_list->AddText(ImGui::GetDefaultFont(), 9.5f, { (root_screen_position.x + width / 2) - (new_width_health / 2) - 12.f, (root_screen_position.y + (height / 10.f) * 1.2f) }, ImColor{ std::get< engine::structures::vector4_t >(menu::config::config_map["health_color"]).color[0], std::get< engine::structures::vector4_t >(menu::config::config_map["health_color"]).color[1], std::get< engine::structures::vector4_t >(menu::config::config_map["health_color"]).color[2], std::get< engine::structures::vector4_t >(menu::config::config_map["health_color"]).color[3] }, to_draw.c_str());
		}

		if (std::get< bool >(menu::config::config_map["display_bounding_box"]))
		{
			{
				draw_list->AddRect({ root_screen_position.x - width, root_screen_position.y }, { root_screen_position.x + width, root_screen_position.y - height }, ImColor{ std::get< engine::structures::vector4_t >(menu::config::config_map["bounding_box_color"]).color[0], std::get< engine::structures::vector4_t >(menu::config::config_map["bounding_box_color"]).color[1], std::get< engine::structures::vector4_t >(menu::config::config_map["bounding_box_color"]).color[2] });
			}

			{
				draw_list->AddRect({ root_screen_position.x - width + 1, root_screen_position.y + 1 }, { root_screen_position.x + width + 1, root_screen_position.y - height + 1 }, ImColor{ 0, 0, 0 });
			}
		}

		if (std::get< bool >(menu::config::config_map["display_filled_box"]))
		{
			{
				draw_list->AddRectFilled({ root_screen_position.x - width, root_screen_position.y }, { root_screen_position.x + width, root_screen_position.y - height }, ImColor{ std::get< engine::structures::vector4_t >(menu::config::config_map["filled_box_color"]).color[0], std::get< engine::structures::vector4_t >(menu::config::config_map["filled_box_color"]).color[1], std::get< engine::structures::vector4_t >(menu::config::config_map["filled_box_color"]).color[2], std::get< engine::structures::vector4_t >(menu::config::config_map["filled_box_color"]).color[3] });
			}
		}

		if (std::get< bool >(menu::config::config_map["display_corner_box"]) && !std::get< bool >(menu::config::config_map["display_bounding_box"]))
		{
			{
				draw_list->AddLine({ root_screen_position.x - width, root_screen_position.y - height }, { root_screen_position.x - width + line_len_width, root_screen_position.y - height }, ImColor{ std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[0], std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[1], std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[2], std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[3] });
				draw_list->AddLine({ root_screen_position.x - width, root_screen_position.y - height }, { root_screen_position.x - width, (root_screen_position.y - height) + line_len_width }, ImColor{ std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[0], std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[1], std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[2], std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[3] });

				draw_list->AddLine({ root_screen_position.x + width, root_screen_position.y - height }, { root_screen_position.x + width - line_len_width, root_screen_position.y - height }, ImColor{ std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[0], std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[1], std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[2], std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[3] });
				draw_list->AddLine({ root_screen_position.x + width, root_screen_position.y - height }, { root_screen_position.x + width, (root_screen_position.y - height) + line_len_width }, ImColor{ std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[0], std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[1], std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[2], std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[3] });

				draw_list->AddLine({ root_screen_position.x - width, root_screen_position.y }, { root_screen_position.x - width + line_len_width, root_screen_position.y }, ImColor{ std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[0], std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[1], std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[2], std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[3] });
				draw_list->AddLine({ root_screen_position.x - width, root_screen_position.y }, { root_screen_position.x - width, root_screen_position.y - line_len_width }, ImColor{ std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[0], std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[1], std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[2], std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[3] });

				draw_list->AddLine({ root_screen_position.x + width, root_screen_position.y }, { root_screen_position.x + width - line_len_width, root_screen_position.y }, ImColor{ std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[0], std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[1], std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[2], std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[3] });
				draw_list->AddLine({ root_screen_position.x + width, root_screen_position.y }, { root_screen_position.x + width, root_screen_position.y - line_len_width }, ImColor{ std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[0], std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[1], std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[2], std::get< engine::structures::vector4_t >(menu::config::config_map["corner_box_color"]).color[3] });
			}

			{
				draw_list->AddLine({ root_screen_position.x - width + 1, root_screen_position.y - height + 1 }, { root_screen_position.x - width + line_len_width, root_screen_position.y - height + 1 }, ImColor{ 0,0,0 });
				draw_list->AddLine({ root_screen_position.x - width + 1, root_screen_position.y - height + 1 }, { root_screen_position.x - width + 1, (root_screen_position.y - height) + line_len_width }, ImColor{ 0,0,0 });

				draw_list->AddLine({ root_screen_position.x + width - 1, root_screen_position.y - height + 1 }, { root_screen_position.x + width - line_len_width, root_screen_position.y - height + 1 }, ImColor{ 0,0,0 });
				draw_list->AddLine({ root_screen_position.x + width - 1, root_screen_position.y - height + 1 }, { root_screen_position.x + width - 1, (root_screen_position.y - height) + line_len_width }, ImColor{ 0,0,0 });

				draw_list->AddLine({ root_screen_position.x - width + 1, root_screen_position.y - 1 }, { root_screen_position.x - width + line_len_width, root_screen_position.y - 1 }, ImColor{ 0,0,0 });
				draw_list->AddLine({ root_screen_position.x - width + 1, root_screen_position.y - 1 }, { root_screen_position.x - width + 1, root_screen_position.y - line_len_width - 1 }, ImColor{ 0,0,0 });

				draw_list->AddLine({ root_screen_position.x + width - 1, root_screen_position.y - 1 }, { root_screen_position.x + width - line_len_width, root_screen_position.y - 1 }, ImColor{ 0,0,0 });
				draw_list->AddLine({ root_screen_position.x + width - 1, root_screen_position.y - 1 }, { root_screen_position.x + width - 1, root_screen_position.y - line_len_width - 1 }, ImColor{ 0,0,0 });
			}
		}

		if (std::get< bool >(menu::config::config_map["display_head_hitbox"]))
		{
			{
				draw_list->AddRect({ head_screen_position.x, head_screen_position.y }, { head_screen_position.x + 5.f, head_screen_position.y + 5.f }, ImColor{ std::get< engine::structures::vector4_t >(menu::config::config_map["hitbox_color"]).color[0], std::get< engine::structures::vector4_t >(menu::config::config_map["hitbox_color"]).color[1], std::get< engine::structures::vector4_t >(menu::config::config_map["hitbox_color"]).color[2], std::get< engine::structures::vector4_t >(menu::config::config_map["hitbox_color"]).color[3] });
			}

			{
				draw_list->AddRect({ head_screen_position.x - 1, head_screen_position.y - 1 }, { (head_screen_position.x + 5.f) - 1, (head_screen_position.y + 5.f) - 1 }, ImColor{ 0, 0, 0 }, 0, 0, 0.5f);
			}
		}

		if (std::get< bool >(menu::config::config_map["display_skeleton"]) && distance <= 150)
		{
			engine::types::transform_t bones[134];

			player.get_bone_transforms(bones);

			const auto r_forearm_pos = bones[engine::structures::engine_bones::HumanRForearm1].get_position(); const auto r_forearm_spos = camera_copy.world_to_screen(r_forearm_pos);
			const auto l_forearm_pos = bones[engine::structures::engine_bones::HumanLForearm1].get_position(); const auto l_forearm_spos = camera_copy.world_to_screen(l_forearm_pos);

			const auto r_digit_pos = bones[engine::structures::engine_bones::HumanRDigit11].get_position(); const auto r_digit_spos = camera_copy.world_to_screen(r_digit_pos);
			const auto l_digit_pos = bones[engine::structures::engine_bones::HumanLDigit11].get_position(); const auto l_digit_spos = camera_copy.world_to_screen(l_digit_pos);

			const auto spine1_pos = bones[engine::structures::engine_bones::HumanSpine1].get_position(); const auto spine1_spos = camera_copy.world_to_screen(spine1_pos);
			const auto spine2_pos = bones[engine::structures::engine_bones::HumanSpine2].get_position(); const auto spine2_spos = camera_copy.world_to_screen(spine2_pos);

			const auto r_calf_pos = bones[engine::structures::engine_bones::HumanRCalf].get_position(); const auto r_calf_spos = camera_copy.world_to_screen(r_calf_pos);
			const auto l_calf_pos = bones[engine::structures::engine_bones::HumanLCalf].get_position(); const auto l_calf_spos = camera_copy.world_to_screen(l_calf_pos);

			const auto r_toe_pos = bones[engine::structures::engine_bones::HumanRToe].get_position(); const auto r_toe_spos = camera_copy.world_to_screen(r_toe_pos);
			const auto l_toe_pos = bones[engine::structures::engine_bones::HumanLToe].get_position(); const auto l_toe_spos = camera_copy.world_to_screen(l_toe_pos);

			const auto color = ImColor{ std::get< engine::structures::vector4_t >(menu::config::config_map["skeleton_color"]).color[0], std::get< engine::structures::vector4_t >(menu::config::config_map["skeleton_color"]).color[1], std::get< engine::structures::vector4_t >(menu::config::config_map["skeleton_color"]).color[2] };

			if( /* >:) */
				r_forearm_spos.has_value() &&l_forearm_spos.has_value() &&
				r_digit_spos.has_value() && l_digit_spos.has_value() && 
				spine1_spos.has_value() && spine2_spos.has_value() &&
				r_calf_spos.has_value() && l_calf_spos.has_value() &&
				r_toe_spos.has_value() && l_toe_spos.has_value()
				)
			{
				draw_list->AddLine({ head_screen_position.x, head_screen_position.y }, { spine1_spos.value().x, spine1_spos.value().y }, color);

				draw_list->AddLine({ head_screen_position.x, head_screen_position.y }, { r_forearm_spos.value().x, r_forearm_spos.value().y }, color);

				draw_list->AddLine({ head_screen_position.x, head_screen_position.y }, { l_forearm_spos.value().x, l_forearm_spos.value().y }, color);

				draw_list->AddLine({ r_forearm_spos.value().x, r_forearm_spos.value().y }, { r_digit_spos.value().x, r_digit_spos.value().y }, color);

				draw_list->AddLine({ l_forearm_spos.value().x, l_forearm_spos.value().y }, { l_digit_spos.value().x, l_digit_spos.value().y }, color);

				draw_list->AddLine({ spine1_spos.value().x, spine1_spos.value().y }, { spine2_spos.value().x, spine2_spos.value().y }, color);

				draw_list->AddLine( { spine2_spos.value().x, spine2_spos.value().y }, { r_calf_spos.value().x, r_calf_spos.value().y }, color);

				draw_list->AddLine({ r_calf_spos.value().x, r_calf_spos.value().y }, { r_toe_spos.value().x, r_toe_spos.value().y }, color);

				draw_list->AddLine({ spine2_spos.value().x, spine2_spos.value().y }, { l_calf_spos.value().x, l_calf_spos.value().y }, color);

				draw_list->AddLine({ l_calf_spos.value().x, l_calf_spos.value().y }, { l_toe_spos.value().x, l_toe_spos.value().y }, color);
			}

			//for (const auto part : skeleton)
			//{
			//	auto prev_bone = engine::structures::engine_bones::Invalid;

			//	for (const auto bone : part)
			//	{
			//		if (prev_bone == engine::structures::engine_bones::Invalid)
			//		{
			//			prev_bone = static_cast<engine::structures::engine_bones>(bone);

			//			continue;
			//		}

			//		auto prev_bone_world = bones[static_cast<int>(prev_bone)].get_position(); auto prev_bone_pos = engine::values::camera.world_to_screen(prev_bone_world);

			//		auto bone_world = bones[static_cast<int>(bone)].get_position(); auto bone_pos = engine::values::camera.world_to_screen(bone_world);

			//		if (prev_bone_pos.has_value() && bone_pos.has_value())
			//		{

			//			auto a = prev_bone_pos.value();
			//			auto b = bone_pos.value();

			//			draw_list->AddLine({ a.x, a.y }, { b.x, b.y }, ImColor{ std::get< engine::structures::vector4_t >(menu::config::config_map["skeleton_color"]).color[0], std::get< engine::structures::vector4_t >(menu::config::config_map["skeleton_color"]).color[1], std::get< engine::structures::vector4_t >(menu::config::config_map["skeleton_color"]).color[2] });

			//			prev_bone = static_cast<engine::structures::engine_bones>(bone);

			//		}
			//	}
			//}

			//for (const auto part : skeleton)
			//{
			//	auto prev_bone = engine::structures::engine_bones::Invalid;

			//	for (const auto bone : part)
			//	{
			//		if (prev_bone == engine::structures::engine_bones::Invalid)
			//		{
			//			prev_bone = static_cast<engine::structures::engine_bones>(bone);

			//			continue;
			//		}

			//		unsigned short pb = 32 + static_cast<int>(prev_bone) * 8;

			//		unsigned short bn = 32 + static_cast<int>(bone) * 8;

			//		auto prev_bone_world = player.get_bone_transform((engine::structures::bones)prev_bone).get_position(); auto prev_bone_pos = engine::values::camera.world_to_screen(prev_bone_world);

			//		auto bone_world = player.get_bone_transform((engine::structures::bones)bone).get_position(); auto bone_pos = engine::values::camera.world_to_screen(bone_world);

			//		if (prev_bone_pos.has_value() && bone_pos.has_value())
			//		{

			//			auto a = prev_bone_pos.value();
			//			auto b = bone_pos.value();

			//			draw_list->AddLine({ a.x, a.y }, { b.x, b.y }, ImColor{ std::get< engine::structures::vector4_t >(menu::config::config_map["skeleton_color"]).color[0], std::get< engine::structures::vector4_t >(menu::config::config_map["skeleton_color"]).color[1], std::get< engine::structures::vector4_t >(menu::config::config_map["skeleton_color"]).color[2] });

			//			prev_bone = static_cast<engine::structures::engine_bones>(bone);

			//		}
			//	}
			//}
		}

		if (std::get< bool >(menu::config::config_map["display_distance"]))
		{
			std::string to_draw; to_draw.append(std::to_string(distance)); to_draw.append("M");

			{
				draw_list->AddLine({ head_screen_position.x, head_screen_position.y }, { head_screen_position.x - 5.f, head_screen_position.y - 10.f }, ImColor{ std::get< engine::structures::vector4_t >(menu::config::config_map["distance_color"]).color[0], std::get< engine::structures::vector4_t >(menu::config::config_map["distance_color"]).color[1], std::get< engine::structures::vector4_t >(menu::config::config_map["distance_color"]).color[2], std::get< engine::structures::vector4_t >(menu::config::config_map["distance_color"]).color[3] });
			}

			{
				draw_list->AddLine({ head_screen_position.x - 5.f, head_screen_position.y - 10.f }, { head_screen_position.x - 15.f, head_screen_position.y - 10.f }, ImColor{ std::get< engine::structures::vector4_t >(menu::config::config_map["distance_color"]).color[0], std::get< engine::structures::vector4_t >(menu::config::config_map["distance_color"]).color[1], std::get< engine::structures::vector4_t >(menu::config::config_map["distance_color"]).color[2], std::get< engine::structures::vector4_t >(menu::config::config_map["distance_color"]).color[3] });
			}

			draw_list->AddText(ImGui::GetDefaultFont(), 10.5f, { head_screen_position.x - 15.f, head_screen_position.y - 20.f }, ImColor{ std::get< engine::structures::vector4_t >(menu::config::config_map["distance_color"]).color[0], std::get< engine::structures::vector4_t >(menu::config::config_map["distance_color"]).color[1], std::get< engine::structures::vector4_t >(menu::config::config_map["distance_color"]).color[2], std::get< engine::structures::vector4_t >(menu::config::config_map["distance_color"]).color[3] }, to_draw.c_str());
		}
	}

	update_players_locker.lock();
}
