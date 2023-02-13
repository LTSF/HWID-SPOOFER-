#include "types.hpp"

#include "../engine.hpp"

#include <mutex>

engine::types::object_t engine::types::game_object_manager_t::last_tagged_object() const
{
	return driver.read< engine::types::object_t >( instance + offsets::object::last_tagged_object );
}

engine::types::object_t engine::types::game_object_manager_t::first_tagged_object() const
{
	return driver.read< engine::types::object_t >( instance + offsets::object::first_tagged_object);
}

engine::types::object_t engine::types::game_object_manager_t::last_camera_tagged_object() const
{
	return driver.read< engine::types::object_t >( instance + offsets::object::last_camera_tagged_object);
}

engine::types::object_t engine::types::game_object_manager_t::first_camera_tagged_object() const
{
	return driver.read< engine::types::object_t >( instance + offsets::object::first_camera_tagged_object);
}

engine::types::object_t engine::types::game_object_manager_t::last_active_object() const
{
	return driver.read< engine::types::object_t >( instance + offsets::object::last_active_object);
}

engine::types::object_t engine::types::game_object_manager_t::first_active_object() const
{
	return driver.read< engine::types::object_t >( instance + offsets::object::first_active_object);
}

engine::types::class_object_t engine::types::game_object_t::class_object() const
{
	return driver.read< class_object_t >( instance + offsets::game_object::class_object );
}

engine::types::game_object_t engine::types::base_object_t::game_object() const
{
	return driver.read< game_object_t >( instance + offsets::base_object::game_object );
}

engine::types::base_object_t engine::types::base_object_t::next_base_object() const
{
	return driver.read< base_object_t >( instance + offsets::base_object::next_base_object );
}

std::vector< engine::types::player_t > engine::types::local_game_world_t::get_players() const
{
	std::vector< engine::types::player_t > players;

	if ( !instance )
		return players;

	const auto generic_list = driver.read( instance + engine::offsets::game_world::registered_players );

	const auto size = driver.read< std::uint32_t >(generic_list + offsets::generic_list::size );

	if ( !size )
		return players;

	const auto list = driver.read(generic_list + offsets::generic_list::elements ) + offsets::generic_list::first_element;

	for ( auto iterator = 1u; iterator < size; ++iterator )
	{
		const auto player = driver.read< engine::types::player_t >( list + ( iterator * sizeof( void* ) ) );

		if ( player.instance )
			players.push_back( player );
	}

	return players;
}

engine::types::player_t engine::types::local_game_world_t::get_local_player( ) const
{
	if ( !instance )
		return { 0 };

	const auto generic_list = driver.read( instance + engine::offsets::game_world::registered_players );

	const auto size = driver.read< std::uint32_t >(generic_list + offsets::generic_list::size );

	if ( !size )
		return { 0 };

	const auto list = driver.read(generic_list + offsets::generic_list::elements ) + offsets::generic_list::first_element;

	if (!list)
		return { 0 };

	return driver.read< engine::types::player_t >( list );
}

engine::types::camera_t::view_matrix_t engine::types::camera_t::get_view_matrix() const
{
	return driver.read< engine::types::camera_t::view_matrix_t  >( instance + offsets::fps_camera::view_matrix );
}

void engine::types::shot_effector_t::set_recoil_strength_xy( const engine::structures::vector2_t recoil_strength_xy ) const
{
	driver.write( instance + offsets::shot_effector::recoil_strength_xy, recoil_strength_xy );
}

void engine::types::shot_effector_t::set_recoil_strength_z( const engine::structures::vector2_t recoil_strength_z ) const
{
	driver.write( instance + offsets::shot_effector::recoil_strength_z, recoil_strength_z );
}

void engine::types::breath_effector_t::set_sway_intensity( float intensity ) const
{
	driver.write( instance + offsets::breath_effector::intensity, intensity );
}

bool engine::types::breath_effector_t::is_aiming( ) const
{
	return driver.read< bool >( instance + offsets::breath_effector::is_aiming );
}

engine::types::shot_effector_t engine::types::procedural_weapon_animation_t::get_shot_effector( ) const
{
	return driver.read< shot_effector_t >(instance + offsets::procedural_weapon_animation::shot_effector );
}

engine::types::breath_effector_t engine::types::procedural_weapon_animation_t::get_breath_effector( ) const
{
	return driver.read< engine::types::breath_effector_t >( instance + offsets::procedural_weapon_animation::breath );
}

engine::types::procedural_weapon_animation_t engine::types::player_t::get_procedural_weapon_animation( ) const
{
	return driver.read< procedural_weapon_animation_t >( instance + offsets::player::procedural_weapon_animation );
}

engine::types::health_controller_t engine::types::player_t::get_health_controller() const
{
	return driver.read< health_controller_t >(instance + offsets::player::health_controller);
}

std::pair<float, float> engine::types::health_controller_t::get_health() const
{
	if (const auto dictionary = driver.read< std::uintptr_t >(instance + 0x50))
	{

		if (const auto count = driver.read< std::uint32_t >(dictionary + 0x40))
		{

			const auto entry_start = driver.read< std::uintptr_t >(dictionary + 0x18);

			float current = 0.0f;
			float max = 0.f;

			for (auto i = 0u; i < count; ++i)
			{
				if (const auto body_part_state = driver.read< std::uintptr_t >(entry_start + 0x30 + 0x18 * i))
				{
					if (const auto health_wrapper = driver.read< std::uintptr_t >(body_part_state + 0x10))
					{

						current += driver.read< float >(health_wrapper + 0x10);

						max += driver.read< float >(health_wrapper + 0x14);
					}
				}
			}

			return { current, max };
		}
	}

	return { 0.f, 0.f };
}

constexpr auto M_PI = 3.14159265358979323846f;

std::optional<engine::structures::vector2_t> engine::types::camera_t::world_to_screen(const engine::structures::vector3_t& position) const
{
	static const auto width = lf( GetSystemMetrics )( SM_CXSCREEN ), height = lf( GetSystemMetrics )( SM_CYSCREEN );

	std::unique_lock<std::mutex> initiate_thread_locker(engine::values::global_locker);

	const auto game_object_manager_copy = engine::values::game_object_manager;

	const auto camera_copy = engine::values::camera;

	const auto local_game_world_copy = engine::values::local_game_world;

	if (!camera_copy.instance || !local_game_world_copy.instance)
		return std::nullopt;

	const auto local_player_copy = local_game_world_copy.get_local_player();

	initiate_thread_locker.unlock();

	const auto last_camera_object = game_object_manager_copy.last_tagged_object( ).as< types::base_object_t >( );

	const auto current_game_object = last_camera_object.game_object( );

	const auto current_class_object = current_game_object.class_object( );

	auto view_matrix = local_player_copy.get_procedural_weapon_animation().get_breath_effector().is_aiming() && current_game_object.is( types::name_hash_t::optical_camera ) ? current_class_object.get_object< types::camera_t >().get_view_matrix() : get_view_matrix();

	const auto first = engine::structures::vector3_t{ view_matrix._11, view_matrix._21, view_matrix._31 };
	const auto second = engine::structures::vector3_t{ view_matrix._12, view_matrix._22, view_matrix._32 };
	const auto fourth = engine::structures::vector3_t{ view_matrix._14, view_matrix._24, view_matrix._34 };

	float y = second.dot(position) + view_matrix._42;
	float x = first.dot(position) + view_matrix._41;

	if ( local_player_copy.instance && local_player_copy.get_procedural_weapon_animation().get_breath_effector().is_aiming() && current_game_object.is( types::name_hash_t::optical_camera ) )
	{
		x /= cos((M_PI / 180) * driver.read<float>(current_game_object.instance + 0x15C) * 0.5f) / sin((M_PI / 180) * driver.read<float>(current_game_object.instance + 0x15C) * 0.5f) * driver.read<float>(current_game_object.instance + 0x4C8 ) * 0.5f;
		y /= cos((M_PI / 180) * driver.read<float>(current_game_object.instance + 0x15C) * 0.5f) / sin((M_PI / 180) * driver.read<float>(current_game_object.instance + 0x15C) * 0.5f) * 0.5f;
	}

	const auto w = fourth.dot( position ) + view_matrix._44;

	if ( w < 0.098f )
		return std::nullopt;

	const auto pos = engine::structures::vector2_t{ (1.f + (x) / w) * (width / 2), (1.f - (y) / w) * (height / 2) };

	if (pos.x > 0 && pos.x <= static_cast<float>(width) && pos.y > 0 && pos.y <= static_cast<float>(height))
		return pos;

	return std::nullopt;
}

std::vector<engine::types::base_object_t> engine::types::game_object_manager_t::get_tagged_objects() const
{
	std::vector< base_object_t > tagged_objects;

	auto last_tagged_object = this->last_tagged_object( ).as< types::base_object_t >( );

	if( !last_tagged_object.instance )
		return tagged_objects;

	auto current_tagged_object = types::base_object_t{ driver.read( this->first_tagged_object( ).instance ) };

	while( current_tagged_object.instance && current_tagged_object.game_object().instance && current_tagged_object.game_object( ).instance != last_tagged_object.game_object().instance )
	{
		if( current_tagged_object.instance )
			tagged_objects.push_back(current_tagged_object.as< types::base_object_t >( ) );

		current_tagged_object.instance = current_tagged_object.next_base_object( ).instance;
	}

	if( last_tagged_object.instance )
		tagged_objects.push_back( last_tagged_object );

	return tagged_objects;
}

std::vector<engine::types::base_object_t> engine::types::game_object_manager_t::get_camera_objects( ) const
{
	std::vector< base_object_t > camera_objects;

	auto last_camera_object = this->last_camera_tagged_object( ).as< types::base_object_t >( );

	if( !last_camera_object.instance )
		return camera_objects;

	auto current_camera_object = types::base_object_t{ driver.read( this->first_camera_tagged_object( ).instance ) };

	while( current_camera_object.instance && current_camera_object.game_object().instance && current_camera_object.game_object( ).instance != last_camera_object.game_object( ).instance )
	{
		if( current_camera_object.instance )
			camera_objects.push_back(current_camera_object.as< types::base_object_t >( ) );

		current_camera_object.instance = current_camera_object.next_base_object( ).instance;
	}

	if( last_camera_object.instance )
		camera_objects.push_back( last_camera_object);

	return camera_objects;
}

std::vector<engine::types::base_object_t> engine::types::game_object_manager_t::get_active_objects() const
{
	std::vector< base_object_t > active_objects;

	auto last_active_object = this->last_active_object( ).as< types::base_object_t >( );

	if( !last_active_object.instance )
		return active_objects;

	auto current_active_object = types::base_object_t{ driver.read( this->first_active_object( ).instance ) };

	while( current_active_object.instance && current_active_object.game_object().instance && current_active_object.game_object( ).instance != last_active_object.game_object( ).instance )
	{
		if( current_active_object.instance )
			active_objects.push_back( current_active_object.as< types::base_object_t >( ) );

		current_active_object.instance = current_active_object.next_base_object( ).instance;
	}

	if( last_active_object.instance )
		active_objects.push_back( last_active_object );

	return active_objects;
}

engine::types::base_object_t engine::types::game_object_manager_t::get_tagged_object_by_name( const name_hash_t name ) const
{
	auto last_tagged_object = this->last_tagged_object( ).as< types::base_object_t >() ;

	if( !last_tagged_object.instance )
		return { 0 };

	auto current_tagged_object = types::base_object_t{ driver.read( this->last_tagged_object( ).instance ) };

	while( current_tagged_object.instance && current_tagged_object.game_object( ).instance != last_tagged_object.instance )
	{
		if( current_tagged_object.instance )
		{
			const auto game_object = current_tagged_object.game_object( );

			if ( game_object.instance && game_object.is( name ) )
			{
				return current_tagged_object;
			}
		}

		current_tagged_object.instance = current_tagged_object.next_base_object( ).instance;
	}

	if( last_tagged_object.instance )
	{
		const auto game_object = last_tagged_object.game_object( );

		if ( game_object.instance && game_object.is( name ) )
		{
			return last_tagged_object.as< base_object_t >( );
		}
	}

	return { 0 };
}

engine::types::base_object_t engine::types::game_object_manager_t::get_camera_object_by_name( const name_hash_t name ) const
{
	auto last_camera_object = this->last_camera_tagged_object( ).as< types::base_object_t >( );

	if( !last_camera_object.instance )
		return { 0 };

	auto current_camera_object = types::base_object_t{ driver.read( this->first_camera_tagged_object( ).instance ) };

	while( current_camera_object.instance && current_camera_object.game_object( ).instance && current_camera_object.game_object( ).instance != last_camera_object.game_object( ).instance )
	{
		if( current_camera_object.instance )
		{
			const auto game_object = current_camera_object.game_object();

			if ( game_object.instance && game_object.is( name ) )
			{
				return current_camera_object;
			}
		}

		current_camera_object.instance = current_camera_object.next_base_object( ).instance;
	}

	if( last_camera_object.instance )
	{
		const auto game_object = last_camera_object.game_object( );

		if ( game_object.instance && game_object.is( name ) )
		{
			return last_camera_object.as< base_object_t >( );
		}
	}

	return { 0 };
}

engine::types::base_object_t engine::types::game_object_manager_t::get_active_object_by_name( const name_hash_t name ) const
{
	auto last_active_object = this->last_active_object( ).as< types::base_object_t >( );

	if( !last_active_object.instance )
		return { 0 };

	auto current_active_object = types::base_object_t{ driver.read( this->first_active_object( ).instance ) };

	while( current_active_object.instance && current_active_object.game_object().instance && current_active_object.game_object( ).instance != last_active_object.game_object( ).instance )
	{
		if( current_active_object.instance )
		{
			const auto game_object = current_active_object.game_object( );

			if ( game_object.instance && game_object.is( name ) )
			{
				return current_active_object;
			}
		}

		current_active_object.instance = current_active_object.next_base_object( ).instance;
	}

	if( last_active_object.instance )
	{
		const auto game_object = last_active_object.game_object( );

		if (game_object.instance && game_object.is(name))
		{
			return last_active_object.as< base_object_t >( );
		}
	}

	return { 0 };
}

engine::structures::vector3_t engine::types::transform_t::get_position() const
{
	if(!instance)
	{
		return {};
	}

	auto a1 = driver.read<uint64_t>(instance + 0x10);
	if (!a1)
		return engine::structures::vector3_t{ };

	auto v8 = driver.read<std::uintptr_t>(a1 + 0x38);

	__int64 v2; // r8
	__int64 arr; // r11
	__int64 dia; // rdx
	int di; // eax
	__int64 v7; // rcx
	__int64 ri; // rax
	__m128i temp_0; // xmm0
	__m128 v10; // xmm11
	__m128 xxxx; // xmm3
	__m128 yxwy; // xmm7
	__m128 zwxy; // xmm9
	__m128 yyyy; // xmm10
	__m128 zzzz; // xmm8
	__m128 wzyw; // xmm4
	__m128 v17{}; // xmm6

	__m128 xmmword_1220320 = { -2.f, 2.f, -2.f, 0.f };
	__m128 xmmword_1220330 = { 2.f, -2.f, -2.f, 0.f };
	__m128 xmmword_1220300 = { -2.f, -2.f, 2.f, 0.f };

	v2 = driver.read<unsigned int>(a1 + 0x38 + 8);

	int failsafe = 0;

	arr = driver.read < std::uint64_t>(v8 + 0x18);
	dia = driver.read < std::uint64_t>(v8 + 0x20);
	di = driver.read<DWORD>(dia + 4 * v2);
	while (di >= 0)
	{
		if (++failsafe > 50)
		{
			std::printf("hit failsafe\n");

			break;
		}

		v7 = di;
		ri = 6 * di;
		temp_0 = driver.read < __m128i>(arr + 8 * ri + 0x10);
		v10 = _mm_mul_ps(driver.read < __m128>(arr + 8 * ri + 0x20), v17);
		xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(temp_0, 0));
		yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(temp_0, 0x71));
		zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(temp_0, 0x8E));
		yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(temp_0, 0x55));
		zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(temp_0, 0xAA));
		wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(temp_0, 0xDB));
		v17 = _mm_add_ps(
			_mm_add_ps(
				_mm_add_ps(
					_mm_mul_ps(
						_mm_sub_ps(
							_mm_mul_ps(_mm_mul_ps(xxxx, (__m128)xmmword_1220330), zwxy),
							_mm_mul_ps(_mm_mul_ps(yyyy, (__m128)xmmword_1220300), wzyw)),
						_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v10), 0xAA))),
					_mm_mul_ps(
						_mm_sub_ps(
							_mm_mul_ps(_mm_mul_ps(zzzz, (__m128)xmmword_1220300), wzyw),
							_mm_mul_ps(_mm_mul_ps(xxxx, (__m128)xmmword_1220320), yxwy)),
						_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v10), 0x55)))),
				_mm_add_ps(
					_mm_mul_ps(
						_mm_sub_ps(
							_mm_mul_ps(_mm_mul_ps(yyyy, (__m128)xmmword_1220320), yxwy),
							_mm_mul_ps(_mm_mul_ps(zzzz, (__m128)xmmword_1220330), zwxy)),
						_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v10), 0))),
					v10)),
			driver.read < __m128>(arr + 8 * ri));
		di = driver.read<DWORD>(dia + 4 * v7);
	}
	return { v17.m128_f32[0], v17.m128_f32[1], v17.m128_f32[2] };
}

engine::types::transform_t engine::types::player_t::get_bone_transform( engine::structures::bones bone ) const
{
	if (!instance)
		return { 0 };

	if( const auto body = driver.read(instance + engine::offsets::player::player_body ) )
	{
		if ( auto temp = driver.read(body + 0x28 ) ) //diz.skinning.skeleton
		{
			if ( temp = driver.read(temp + 0x28) ) // _values generic_list<transform>
			{
				const auto size = driver.read< std::uint32_t >(temp + offsets::generic_list::size);

				if (!size)
					return {0};

				if ( const auto bone_array = driver.read(temp + offsets::generic_list::elements) )
				{
					return driver.read< engine::types::transform_t >( bone_array + offsets::generic_list::first_element + bone * 8);
				}
			}
		}
	}

	return { 0 };
}

void engine::types::player_t::get_bone_transforms( transform_t* buff ) const
{

	if (const auto body = driver.read(instance + engine::offsets::player::player_body))
	{
		if (auto temp = driver.read(body + 0x28)) //diz.skinning.skeleton
		{
			if (temp = driver.read(temp + 0x28)) // _values generic_list<transform>
			{
				const auto size = driver.read< std::uint32_t >(temp + offsets::generic_list::size);

				if (!size)
					return;

				if (const auto bone_array = driver.read(temp + offsets::generic_list::elements))
				{

					driver.read_arr< transform_t >(bone_array + 32, buff, 134);
				}
			}
		}
	}
}


