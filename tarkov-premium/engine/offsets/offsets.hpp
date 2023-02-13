#pragma once

namespace engine::offsets
{
	constexpr auto game_object_manager = 0x17F1CE8;
}

namespace engine::offsets::object
{
	constexpr auto last_tagged_object = 0x0;
	constexpr auto first_tagged_object = 0x8;

	constexpr auto last_camera_tagged_object = 0x10;
	constexpr auto first_camera_tagged_object = 0x18;

	constexpr auto last_active_object = 0x20;
	constexpr auto first_active_object = 0x28;
}

namespace engine::offsets::base_object
{
	constexpr auto next_base_object = 0x8;
	constexpr auto game_object = 0x10;
}

namespace engine::offsets::game_object
{
	constexpr auto class_object = 0x30;
	constexpr auto name = 0x60;
}

namespace engine::offsets::class_object
{
	constexpr auto transform = 0x8;
	constexpr auto object_reference = 0x18;
}

namespace engine::offsets::game_world
{
	constexpr auto self = 0x28;

	constexpr auto registered_players = 0x80;
}

namespace engine::offsets::generic_list
{
	constexpr auto elements = 0x10;
	constexpr auto size = 0x18;

	constexpr auto first_element = 0x20;
}

namespace engine::offsets::fps_camera
{
	constexpr auto view_matrix = 0xDC;
}

namespace engine::offsets::player
{
	constexpr auto player_body = 0xA8;
	constexpr auto procedural_weapon_animation = 0x190;
	constexpr auto health_controller = 0x4e8;
	constexpr auto physical = 0x4c0;
}

namespace engine::offsets::physical
{
	constexpr auto inertia = 0x98;


}

namespace engine::offsets::shot_effector // { nil, "ShotEffector" }
{
	constexpr auto recoil_strength_xy = 0x38;
	constexpr auto recoil_strength_z = 0x40;
	constexpr auto recoil_degree = 0x48;
	constexpr auto recoil_direction = 0x50;
	constexpr auto recoil_radian = 0x5C;
	constexpr auto shot_vals = 0x10;
	constexpr auto stiffness = 0x64;
	constexpr auto recoil = 0x18;
	constexpr auto recoil_power = 0x20;
	constexpr auto intensity = 0x68;
	constexpr auto _x_intensity_by_pose = 0x6C;
	constexpr auto _y_intensity_by_pose = 0x78;
	constexpr auto _z_intensity_by_pose = 0x84;
	constexpr auto _separate_intensity_factors = 0x90;
	constexpr auto _weapon = 0x28;
	constexpr auto _buffs = 0x30;
	constexpr auto _pose = 0x9C;
}

namespace engine::offsets::breath_effector
{
	constexpr auto intensity = 0xa4;
	constexpr auto is_aiming = 0xa0;
}

namespace engine::offsets::procedural_weapon_animation // { "EFT.Animations", "ProceduralWeaponAnimation" }
{
	constexpr auto breath = 0x28;

	constexpr auto shot_effector = 0x48;
}