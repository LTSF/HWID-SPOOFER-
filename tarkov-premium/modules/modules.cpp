#include "modules.hpp"

#include <thread>

#include "weapons/no_sway/no_sway.hpp"
#include "weapons/no_recoil/no_recoil.hpp"
#include "../settings/settings.hpp"
#include "misc/night_vision/night_vision.hpp"
#include "misc/no_visor/no_visor.hpp"
#include "weapons/no_intertia/no_intertia.hpp"

std::array< modules::module_t, 5 > modules::module_array = 
{
	modules::module_t{ false, modules::weapon::no_sway::callback },
	modules::module_t{ false, modules::weapon::no_recoil::callback },
	modules::module_t{ false, modules::misc::night_vision::callback },
	modules::module_t{ false, modules::misc::no_visor::callback },
	modules::module_t{ true, modules::weapon::no_intertia::callback }
};

void modules::fire_modules( )
{
	while (true)
	{
		for (const auto& [premium, callback] : module_array)
		{
			if ( !premium || settings::values::premium )
				callback( );
		}

		std::this_thread::sleep_for( std::chrono::milliseconds( module_update_interval ) );
	}
}
