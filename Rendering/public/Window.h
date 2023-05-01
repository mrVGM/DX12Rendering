#pragma once

#include "BaseObject.h"

#include <Windows.h>

namespace rendering
{
	class Window : public BaseObject
	{
	private:

		void RegisterWindowClass();
		void Create();
		void Destroy();

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