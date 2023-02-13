#include "config.hpp"

#include "../menu.hpp"

std::unordered_map< const char*, std::variant< bool, int, float, engine::structures::vector4_t > > menu::config::config_map
{
	{ "assist_enable",			false },
	{ "assist_smoothness",		5 },
	{ "assist_bone",		    0 },
	{ "assist_max_distance",	30 },
	{ "assist_show_fov",		false },
	{ "assist_fov",	            50 },
	{ "assist_fov_segments",	 8 },
	{ "assist_hotkey",			VK_RBUTTON },

	{ "enable_no_recoil",		false },
	{ "enable_no_sway",			false },
	{ "enable_no_inertia",		false },
	{ "enable_night_vision",	false },
	{ "enable_remove_visor",	false },

	{ "visor_intensity",        0 },

	{ "recoil_x",               0 },
	{ "recoil_y",               0 },

	{ "recoil_depth_x",         0 },
	{ "recoil_depth_y",         0 },

	{ "recoil_random_x",        0 },
	{ "recoil_random_y",        0 },

	{ "sway_intensity",         0 },

	{ "display_max_distance",	750 },
	{ "display_bounding_box",	false },
	{ "display_filled_box",	    false },
	{ "display_corner_box",	    false },
	{ "display_head_hitbox",	false },
	{ "display_skeleton",	    false },
	{ "display_distance",	    false },
	{ "display_health",	        false },

	{ "bounding_box_color", engine::structures::vector4_t { 1.f, 1.f, 1.f, 1.f } },
	{ "filled_box_color", engine::structures::vector4_t { 1.f, 1.f, 1.f, 1.f } },
	{ "corner_box_color", engine::structures::vector4_t { 1.f, 1.f, 1.f, 1.f } },
	{ "hitbox_color", engine::structures::vector4_t { 1.f, 1.f, 1.f, 1.f } },
	{ "skeleton_color", engine::structures::vector4_t { 1.f, 1.f, 1.f, 1.f } },
	{ "distance_color", engine::structures::vector4_t { 1.f, 1.f, 1.f, 1.f } },
	{ "operator_color", engine::structures::vector4_t { 1.f, 1.f, 1.f, 1.f } },
	{ "health_color", engine::structures::vector4_t { 0.f, 1.f, 0.f, 1.f } },
	{ "fov_color", engine::structures::vector4_t { 1.f, 1.f, 1.f, 1.f } },
};