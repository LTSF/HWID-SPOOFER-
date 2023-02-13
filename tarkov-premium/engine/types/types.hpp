#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include "../../driver/driver.hpp"
#include "../offsets/offsets.hpp"
#include "../structures/structures.hpp"

namespace engine::types
{
	enum class name_hash_t : std::uint64_t
	{
		fps_camera = 0x656D614320535046, // "FPS Came"
		game_world = 0x6C726f57656D6147,  // "GameWorl"
		optical_camera = 0x6974704f65736142 // "OpticCam"
	};

	struct object_t
	{
		template< class type_t = std::uintptr_t >
		[[nodiscard]] type_t as( ) const
		{
			return type_t{ instance };
		}

		std::uintptr_t instance;
	};

	struct shot_effector_t : public object_t
	{
		void set_recoil_strength_xy( const engine::structures::vector2_t recoil_strength_xy ) const;

		void set_recoil_strength_z( const engine::structures::vector2_t recoil_strength_z ) const;
	};

	struct breath_effector_t : public object_t
	{
		void set_sway_intensity( float intensity ) const;

		[[nodiscard]] bool is_aiming() const;
	};

	struct procedural_weapon_animation_t : public object_t
	{
		[[nodiscard]] shot_effector_t get_shot_effector() const;

		[[nodiscard]] breath_effector_t get_breath_effector() const;
	};

	struct class_object_t : public object_t
	{
		template< class type_t = std::uintptr_t >
		type_t get_object( ) const
		{
			return driver.read< type_t >( instance + engine::offsets::class_object::object_reference );
		}
	};

	struct game_object_t : public object_t
	{
		[[nodiscard]] bool is( name_hash_t hash ) const
		{
			return hash == driver.read< name_hash_t >( instance, { offsets::game_object::name, 0x0 } );
		}

		[[nodiscard]] class_object_t class_object( ) const;
	};

	struct base_object_t : public object_t
	{
		[[nodiscard]] game_object_t game_object( ) const;

		[[nodiscard]] base_object_t next_base_object( ) const;
	};

	struct transform_t : public object_t
	{
		[[nodiscard]] engine::structures::vector3_t get_position( ) const;
	};

	struct health_controller_t : public object_t
	{
		[[nodiscard]] std::pair<float, float> get_health() const;
	};

	struct bone_array_t
	{
		transform_t transform;
	};

	struct player_t : public object_t
	{
		[[nodiscard]] procedural_weapon_animation_t get_procedural_weapon_animation() const;

		[[nodiscard]] transform_t get_bone_transform( engine::structures::bones bone ) const;

		void get_bone_transforms( transform_t* buff ) const;

		[[nodiscard]] health_controller_t get_health_controller() const;
	};

	struct local_game_world_t : public object_t
	{
		[[nodiscard]] std::vector< player_t > get_players( ) const; /* Does not include local player */

		[[nodiscard]] player_t get_local_player( ) const;
	};

	struct camera_t : public object_t
	{
		union view_matrix_t
		{
			struct
			{
				const float        _11, _12, _13, _14;
				const float        _21, _22, _23, _24;
				const float        _31, _32, _33, _34;
				const float        _41, _42, _43, _44;

			};

			const float m[4][4];
		};

		[[nodiscard]] view_matrix_t get_view_matrix( ) const;

		[[nodiscard]] std::optional< engine::structures::vector2_t > world_to_screen( const engine::structures::vector3_t& position ) const;
	};

	struct game_object_manager_t : public object_t
	{
		[[nodiscard]] object_t last_tagged_object( ) const;
		[[nodiscard]] object_t first_tagged_object() const;

		[[nodiscard]] object_t last_camera_tagged_object() const;
		[[nodiscard]] object_t first_camera_tagged_object() const;

		[[nodiscard]] object_t last_active_object() const;
		[[nodiscard]] object_t first_active_object() const;

		[[nodiscard]] std::vector< base_object_t > get_tagged_objects( ) const;
		[[nodiscard]] std::vector< base_object_t > get_camera_objects( ) const;
		[[nodiscard]] std::vector< base_object_t > get_active_objects( ) const;

		[[nodiscard]] base_object_t get_tagged_object_by_name( const name_hash_t name ) const;
		[[nodiscard]] base_object_t get_camera_object_by_name( const name_hash_t name ) const;
		[[nodiscard]] base_object_t get_active_object_by_name( const name_hash_t name ) const;
	};
}