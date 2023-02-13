#include "driver.hpp"

#include <tlhelp32.h>

#include <iostream>

driver_t driver;

namespace types
{
	template < class type_t, auto deleter_t > using managed_t = std::unique_ptr< std::remove_pointer_t< type_t >, std::integral_constant< decltype(deleter_t), deleter_t > >;
}

std::uintptr_t driver_t::get_base() const
{
	static const auto data = data_t{ data_t::command_t::get_base, to };

	if (!data.from_address)
		NtDCompositionSetChildRootVisual(data, nullptr);

	return data.from_address;
}

std::uintptr_t driver_t::get_module_base(std::wstring module_name)
{
	const auto peb = get_base();

	if (peb)
	{
		auto v5 = driver.read(peb + 0x18);
		if (v5)
		{
			auto v6 = driver.read(v5 + 16);
			if (v6)
			{
				while (driver.read(v6 + 0x30))
				{
					auto length = driver.read<USHORT>(v6 + 0x58);

					auto start = driver.read(v6 + 0x60);

					std::wstring name{};

					name.reserve(length / 2);

					for (auto i = 0u; i < length / 2; ++i)
					{
						name.push_back( driver.read< WCHAR>(start + i * 2));
					}

					if (name == module_name)
						return driver.read(v6 + 0x30);

					v6 = driver.read(v6);
					if (!v6)
						return 0;
				}
			}
		}
	}

	return 0;
}

std::string driver_t::read_string(std::uintptr_t address, const bool inner) const
{
	if (inner)
		address = driver.read(address);

	std::string string;

	do
		string.push_back(driver.read< char >(address++));
	while (driver.read< char >(address) != '\0');

	return string;
}

bool driver_t::initiate(const char* const window_name)
{
	target_window = lf(FindWindowA)(nullptr, window_name);

	lf(GetWindowThreadProcessId)(target_window, &target_id);

	return lf(GetWindowThreadProcessId)(lf(FindWindowA)(nullptr, window_name), &to);
}