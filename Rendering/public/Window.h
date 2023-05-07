#pragma once

#include "BaseObject.h"

#include <Windows.h>

#include <set>

namespace rendering
{
	class Window;

	struct InputInfo
	{
		Window* m_source = nullptr;
		std::set<WPARAM> m_keysDown;
		bool m_leftMouseButtonDown = false;
		bool m_rightMouseButtonDown = false;
		long m_mouseMovement[2];
	};

	class Window : public BaseObject
	{
	private:
		InputInfo m_inputInfo;

		void RegisterWindowClass();
		void Create();
		void Destroy();

		void RegisterRawInputDevice();
	public:
		HWND m_hwnd = nullptr;
		const UINT m_width = 600;
		const UINT m_height = 400;

		Window();
		virtual ~Window();

		static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

		void WindowTick();
	};
}