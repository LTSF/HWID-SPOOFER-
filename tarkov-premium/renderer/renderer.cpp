#include "renderer.hpp"

#include <thread>

#include <winuser.h>

#include "menu/menu.hpp"

#include "../integrity/integrity.hpp"


#include "../dependencies/xorstring/xorstring.hpp"
#include "../settings/settings.hpp"
#include "../modules/visuals/visuals.hpp"

std::unique_ptr< renderer::structures::renderer_t > renderer::values::renderer;

LONG_PTR renderer::original_wndproc = NULL; bool renderer::values::insert_hit = false;

constexpr const auto str_max_len = 15u;

bool renderer::structures::renderer_t::has_window_moved(const RECT& new_point)
{
	return old_pos.left != new_point.left || old_pos.right != new_point.right || old_pos.top != new_point.top || old_pos.bottom != new_point.bottom;
}

void renderer::structures::renderer_t::release_render_target()
{
	if (!render_target_view)
		return;

	render_target_view->Release();

	render_target_view = nullptr;
}

void renderer::structures::renderer_t::init_imgui()
{
	ImGui::CreateContext();

	renderer::imgui::elements::render::set_styles(NULL);

	ImGui_ImplWin32_Init(overlay_window);
	ImGui_ImplDX11_Init(global_device, global_device_context);
}

void renderer::structures::renderer_t::create_render_target()
{
	ID3D11Texture2D* temporary = nullptr;

	global_swap_chain->GetBuffer(0u, IID_PPV_ARGS(&temporary));

	global_device->CreateRenderTargetView(temporary, nullptr, &render_target_view);

	temporary->Release();
}

void renderer::structures::renderer_t::initialize_device()
{
	DXGI_SWAP_CHAIN_DESC scd;

	ZeroMemory(&scd, sizeof(scd));

	scd.BufferCount = 2;
	scd.BufferDesc.Width = screen_width;
	scd.BufferDesc.Height = screen_height;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 120;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = overlay_window;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	D3D_FEATURE_LEVEL level;
	D3D_FEATURE_LEVEL level_array[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

	if (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, level_array, 2, D3D11_SDK_VERSION, &scd, &global_swap_chain, &global_device, &level, &global_device_context) != S_OK)
		std::printf("FAILED\n");

	create_render_target();

	ShowWindow(overlay_window, SW_SHOWNORMAL);
	UpdateWindow(overlay_window);
}

void renderer::structures::renderer_t::clear_data()
{
	screen_width = GetSystemMetrics(SM_CXSCREEN);
	screen_height = GetSystemMetrics(SM_CYSCREEN);
}

void renderer::structures::renderer_t::render()
{
	RECT rect{ 0 };
	POINT point{ 0 };

	GetClientRect(game_window, &rect);
	ClientToScreen(game_window, &point);

	rect.left = point.x;
	rect.top = point.y;
	rect.right -= 2;
	rect.bottom -= 2;

	if (has_window_moved(rect))
	{
		SetWindowPos(overlay_window, nullptr, rect.left, rect.top, rect.right, rect.bottom, SWP_NOREDRAW);

		old_pos = rect;
	}

	MSG msg{ 0 };
	if (::PeekMessageA(&msg, nullptr, 0U, 0U, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessageA(&msg);
	}

	const auto foreground_window = GetForegroundWindow();

	bool overlay_context = (foreground_window == overlay_window || foreground_window == game_window);

	bool alt_tab = (!overlay_context && is_menu_open);

	if (alt_tab)
	{
		is_menu_open = false;
		SetWindowPos(game_window, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	modules::visuals::callback();

	renderer::menu::draw();

	ImGui::Render();

	float clr[4] = { 0, 0, 0 };

	global_device_context->OMSetRenderTargets(1, &render_target_view, nullptr);
	global_device_context->ClearRenderTargetView(render_target_view, clr);

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	global_swap_chain->Present(1, 0u);

	if (renderer::values::insert_hit)
	{
		renderer::values::insert_hit = false;

		if (overlay_context)
		{
			is_menu_open = !is_menu_open;

			if (is_menu_open)
			{
				SetForegroundWindow(overlay_window);

				SetWindowLong(overlay_window, GWL_EXSTYLE, ex_style & ~WS_EX_TRANSPARENT);
			}
			else
			{
				SetForegroundWindow(game_window);

				SetWindowLong(overlay_window, GWL_EXSTYLE, ex_style | WS_EX_TRANSPARENT);
			}
		}
	}

	if (!is_menu_open)
	{
		SetWindowPos(game_window, overlay_window, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
	else
	{
		if (!overlay_context)
			SetWindowPos(overlay_window, game_window, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
}

renderer::structures::renderer_t::renderer_t(HWND game_window)
	: game_window{ game_window }
{
	if (integrity_intellisense.gpu.type == xs("NVIDIA"))
		overlay_window = FindWindowA(nullptr, xs("NVIDIA GeForce Overlay"));
	else if (integrity_intellisense.gpu.type == xs("AMD"))
		overlay_window = FindWindowA(nullptr, xs("Overwolf"));

	if constexpr ( !settings::values::using_auth )
		overlay_window = FindWindowA(nullptr, xs("NVIDIA GeForce Overlay"));

	ex_style = GetWindowLong(overlay_window, GWL_EXSTYLE);

	SetWindowLong(overlay_window, GWL_EXSTYLE, ex_style | WS_EX_TRANSPARENT);

	SetWindowLong(overlay_window, GWL_EXSTYLE, ex_style & ~WS_EX_TOOLWINDOW);

	SetLayeredWindowAttributes(overlay_window, RGB(0, 0, 0), 240u, LWA_ALPHA);

	MARGINS margins{ -1 };
	DwmExtendFrameIntoClientArea(overlay_window, &margins);

	SetForegroundWindow(game_window); SetForegroundWindow(overlay_window);

	if (integrity_intellisense.gpu.type == xs("NVIDIA"))
		SetWindowLongPtrA(overlay_window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(renderer::wndproc));
	else if (integrity_intellisense.gpu.type == xs("AMD"))
		SetWindowLongPtrA(overlay_window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(renderer::wndproc));

	if constexpr ( !settings::values::using_auth )
		SetWindowLongPtrA(overlay_window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(renderer::wndproc));
}

void renderer::imgui::elements::render::set_styles(void*)
{
	ImGui::GetStyle().WindowPadding = { 0,0 };
	ImGui::GetStyle().WindowBorderSize = 0;
	ImGui::GetStyle().PopupBorderSize = 0;
	ImGui::GetStyle().PopupRounding = 5;
	ImGui::GetStyle().WindowRounding = 16;
	ImGui::GetStyle().ScrollbarSize = 4;

	ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImColor{ 18, 18, 18 };
	ImGui::GetStyle().Colors[ImGuiCol_PopupBg] = ImColor{ 33, 33, 33 };
	ImGui::GetStyle().Colors[ImGuiCol_Text] = ImColor{ 119, 119, 119 };
	ImGui::GetStyle().Colors[ImGuiCol_ScrollbarBg] = ImColor{ 119, 119, 119, 0 };

	ImGui::GetStyle().Colors[ImGuiCol_Header] = ImColor{ 35, 42, 51 };
	ImGui::GetStyle().Colors[ImGuiCol_HeaderActive] = ImColor{ 35, 42, 51 };
	ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered] = ImColor{ 35, 42, 51 };

	static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

	static const ImWchar ranges[] = {
		0x0020, 0x00FF,0x2000, 0x206F,0x3000, 0x30FF,0x31F0, 0x31FF, 0xFF00,
		0xFFEF,0x4e00, 0x9FAF,0x0400, 0x052F,0x2DE0, 0x2DFF,0xA640, 0xA69F, 0
	};

	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;

	ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(poppins_compressed_data, poppins_compressed_size, 14, NULL, ranges);
	ImGui::GetIO().Fonts->AddFontFromMemoryTTF(faprolight, sizeof faprolight, 18, &icons_config, icon_ranges);

	renderer::imgui::elements::fonts::druk_wide_bold = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(druk_wide_bold_compressed_data, druk_wide_bold_compressed_size, 30, NULL, ranges);

	renderer::imgui::elements::fonts::px18 = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(poppins_compressed_data, poppins_compressed_size, 18, NULL, ranges);
	renderer::imgui::elements::fonts::px24 = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(poppins_compressed_data, poppins_compressed_size, 24, NULL, ranges);
}

void renderer::start(std::string_view window_name)
{
	const auto game_window = FindWindowA(nullptr, window_name.data());

	renderer::values::renderer = std::make_unique< renderer::structures::renderer_t >(game_window);

	renderer::values::renderer->initialize_device();

	renderer::values::renderer->init_imgui();

	std::thread([=]() {
		RegisterHotKey(0, 1, 0, VK_INSERT);

		MSG hmsg = { 0 };
		while (true)
		{
			if (GetMessage(&hmsg, nullptr, 0, 0) && hmsg.message == WM_HOTKEY && !renderer::values::insert_hit)
				renderer::values::insert_hit = true;

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		}).detach();

		while (true) { renderer::values::renderer->render(); };
}

LRESULT renderer::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (msg)
	{
	case WM_SIZE:
	{
		if (renderer::values::renderer->global_device && wparam != SIZE_MINIMIZED)
		{
			renderer::values::renderer->release_render_target();

			renderer::values::renderer->screen_width = LOWORD(lparam);
			renderer::values::renderer->screen_height = HIWORD(lparam);

			renderer::values::renderer->global_swap_chain->ResizeBuffers(0u, renderer::values::renderer->screen_width, renderer::values::renderer->screen_height, DXGI_FORMAT_UNKNOWN, 0u);

			renderer::values::renderer->create_render_target();

			return S_OK;
		}
		break;
	}
	case WM_SYSCOMMAND:
	{
		if ((wparam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return S_OK;
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	}

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
		return S_OK;

	return ::DefWindowProc(hwnd, msg, wparam, lparam);
}