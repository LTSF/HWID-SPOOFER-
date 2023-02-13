#pragma once

#include <windows.h>

#include <string>

#include "../dependencies/xorstring/xorstring.hpp"
#include "../dependencies/lazy_importer/lazy_importer.hpp"

class driver_t
{
	struct data_t
	{
		enum class command_t
		{
			get_base,
			copy
		}
		const command;

		const std::uint64_t from_process, to_process;

		const std::uintptr_t from_address, to_address;

		const std::size_t size;

		const std::uint16_t magic = 0xEAC;
	};

	using nt_d_composition_set_child_root_visual_t = NTSTATUS(*)(const data_t& data, void*);
	const nt_d_composition_set_child_root_visual_t NtDCompositionSetChildRootVisual = lf(NtDCompositionSetChildRootVisual).in< nt_d_composition_set_child_root_visual_t >(lm("win32u.dll").get());

	DWORD to, from = lf(GetCurrentProcessId)();

	HWND target_window; DWORD target_id;

public:
	template < class value_t = std::uintptr_t > value_t raw_read(const std::uintptr_t address) const
	{
		value_t value{};

		if (!address)
			return value;

		const auto data = data_t{ data_t::command_t::copy, to, from, address, reinterpret_cast<std::uintptr_t>(&value), sizeof(value_t) };
		NtDCompositionSetChildRootVisual(data, nullptr);

		return value;
	}

	template < class value_t = std::uintptr_t > void read_arr(const std::uintptr_t address, value_t* buffer, const std::size_t size) const
	{
		if (!address)
			buffer = nullptr;

		const auto data = data_t{ data_t::command_t::copy, to, from, address, reinterpret_cast<std::uintptr_t>(buffer), sizeof(value_t) * size };
		NtDCompositionSetChildRootVisual(data, nullptr);
	}

	template < class value_t >
	void raw_write(const std::uintptr_t address, const value_t& value) const
	{
		if (!address)
			return;

		const auto data = data_t{ data_t::command_t::copy, from, to, reinterpret_cast<std::uintptr_t>(&value), address, sizeof(value) };
		NtDCompositionSetChildRootVisual(data, nullptr);
	}

	template < class value_t = std::uintptr_t > value_t __forceinline read(const std::uintptr_t address)
	{
		return raw_read< value_t >(address);
	}

	template < class buffer_t = std::uintptr_t >
	__forceinline buffer_t read(std::uintptr_t address, const std::initializer_list< std::uint16_t >&& offsets)
	{
		buffer_t buffer;

		for (auto i = 0ull; i < offsets.size() - 1; ++i)
			address = raw_read(address + *(offsets.begin() + i));

		return raw_read< buffer_t >(address + *(offsets.end() - 1));
	}

	template < class value_t >
	void write(const std::uintptr_t address, const value_t& value)
	{
		raw_write(address, value);
	}

	std::uintptr_t get_base() const; std::uintptr_t get_module_base(std::wstring module_name);

	std::string read_string(std::uintptr_t address, const bool inner = true) const;

	bool initiate(const char* const window_name);
}
extern driver;