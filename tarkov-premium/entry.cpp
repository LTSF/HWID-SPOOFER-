#include <Windows.h>
#include <thread>

#include "dependencies/xorstring/xorstring.hpp"
#include "driver/driver.hpp"
#include "engine/engine.hpp"
#include "integrity/integrity.hpp"
#include "io/io.hpp"
#include "renderer/renderer.hpp"
#include "settings/settings.hpp"
#include "modules/modules.hpp"
#include "modules/aimbot/aimbot.hpp"

int core( )
{
	bool integrity_status = integrity::check( );

	if constexpr ( !settings::values::using_auth )
		integrity_status = true;

	if( integrity_status )
	{
		while ( !FindWindowA( nullptr, xs( "EscapeFromTarkov" ) ) )
			std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

		if constexpr ( settings::values::debug )
			io::open_console( xs( "mutex-debug console" ) );

		driver.initiate( xs( "EscapeFromTarkov" ) );

		std::thread{ engine::functions::initiate }.detach( );

		std::thread{ engine::functions::update_players_vector }.detach( );

		std::thread{ modules::fire_modules }.detach( );

		std::thread{ modules::aimbot::callback }.detach();

		std::thread([]() { renderer::start(xs("EscapeFromTarkov")); }).detach();
	}
	else
	{
		_Exit( 0 );
	}

	return 0;
}

bool DllMain( void*, const std::uint32_t injection_reason, void* )
{
	if ( injection_reason == DLL_PROCESS_ATTACH )
		std::thread{ core }.detach( );

	return true;
}