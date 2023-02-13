#include "engine.hpp"

std::mutex engine::values::global_locker;
std::mutex engine::values::player_vector_locker;

std::uintptr_t engine::values::unity_player = 0;

engine::types::game_object_manager_t engine::values::game_object_manager{ 0};
engine::types::camera_t engine::values::camera{ 0};
engine::types::local_game_world_t engine::values::local_game_world{ 0 };

std::vector< engine::types::player_t > engine::values::players{};

std::once_flag setup_bases;

void engine::functions::initiate( )
{
	while( true )
	{
		std::call_once( setup_bases, [=]()
			{
				engine::values::unity_player = driver.get_module_base( L"UnityPlayer.dll" );

				values::game_object_manager.instance = driver.read(engine::values::unity_player + engine::offsets::game_object_manager);

				if (!values::game_object_manager.instance)
					std::printf(xs(" Failed to get game_object_manager "));
			});

		const auto temporary_camera_value = engine::values::game_object_manager.get_camera_object_by_name( engine::types::name_hash_t::fps_camera ).game_object( ).class_object( ).get_object< types::camera_t >( );

		const auto temporary_local_game_world_value = driver.read< types::local_game_world_t >( engine::values::game_object_manager.get_active_object_by_name( engine::types::name_hash_t::game_world ).game_object( ).class_object( ).get_object( ) + engine::offsets::game_world::self );

		std::unique_lock<std::mutex> initiate_thread_locker( engine::values::global_locker );

		engine::values::camera = temporary_camera_value;

		engine::values::local_game_world = temporary_local_game_world_value;

		initiate_thread_locker.unlock( );

		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	}
}

void engine::functions::update_players_vector()
{
	while( true )
	{
		std::unique_lock<std::mutex> update_players_global_locker( engine::values::global_locker );

		if ( !engine::values::camera.instance || !engine::values::local_game_world.instance )
		{
			update_players_global_locker.unlock( );

			std::unique_lock<std::mutex> update_players_locker( engine::values::player_vector_locker );

			engine::values::players.clear( );

			update_players_locker.unlock( );

			continue;
		}

		types::local_game_world_t local_game_world_clone { engine::values::local_game_world.instance };

		update_players_global_locker.unlock( );

		const auto temporary_players = local_game_world_clone.get_players( );

		std::unique_lock<std::mutex> update_players_locker( engine::values::player_vector_locker );

		engine::values::players = temporary_players;

		update_players_locker.unlock();

		std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
	}
}
