#include "Window.h"

#include "WindowMeta.h"

#include "WindowUpdateJobSystemMeta.h"
#include "BaseObjectContainer.h"
#include "JobSystem.h"
#include "Job.h"

#include <hidusage.h>

namespace
{
	bool m_classRegistered = false;
	const wchar_t* m_className = L"MyWindow";
}

rendering::Window::Window() :
	BaseObject(rendering::WindowMeta::GetInstance())
{
	RegisterWindowClass();

	BaseObject* obj = BaseObjectContainer::GetInstance().GetObjectOfClass(WindowUpdateJobSystemMeta::GetInstance());
	if (!obj)
	{
		obj = new jobs::JobSystem(WindowUpdateJobSystemMeta::GetInstance(), 1);
	}

	jobs::JobSystem* jobSystem = static_cast<jobs::JobSystem*>(obj);

	class WindowUpdateJob : public jobs::Job
	{
	private:
		Window* m_wnd = nullptr;
	public:
		WindowUpdateJob(Window* window) :
			m_wnd(window)
		{
		}
		void Do()
		{
			m_wnd->Create();
			while (m_wnd->m_hwnd)
			{
				m_wnd->WindowTick();
			}
		}
	};

	jobSystem->ScheduleJob(new WindowUpdateJob(this));
}

rendering::Window::~Window()
{
	Destroy();

	if (m_hwnd != nullptr) {
		DestroyWindow(m_hwnd);
		m_hwnd = nullptr;
	}
}

void rendering::Window::RegisterWindowClass()
{
	if (!m_classRegistered)
	{
		WNDCLASSEXW wcex;

		ZeroMemory(&wcex, sizeof(wcex));
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wcex.lpfnWndProc = &StaticWndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = GetModuleHandle(NULL);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = m_className;

		RegisterClassExW(&wcex);

		m_classRegistered = true;
	}
}

LRESULT rendering::Window::StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CREATE)
	{
		LPCREATESTRUCT data = (LPCREATESTRUCT)lParam;
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)data->lpCreateParams);
		auto* window = (Window*)data->lpCreateParams;
		window->m_hwnd = hWnd;

		window->RegisterRawInputDevice();
	}

	// Process messages by window message function
	Window* window = (Window*) ::GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (window)
	{
		return window->WndProc(uMsg, wParam, lParam);
	}
	else
	{
		return static_cast<LRESULT>(DefWindowProc(hWnd, uMsg, wParam, lParam));
	}
}


void rendering::Window::Create()
{
	DWORD dwStyle = WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_VISIBLE;
	DWORD dxExStyle = 0;

	RECT windowRect;
	windowRect.left = 50;
	windowRect.top = 50;
	windowRect.right = windowRect.left + m_width;
	windowRect.bottom = windowRect.top + m_height;

	AdjustWindowRect(&windowRect, dwStyle, FALSE);

	CreateWindow(
		m_className,
		L"Render Window",
		dwStyle,
		windowRect.left, windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL, NULL, GetModuleHandle(NULL), this);
}

void rendering::Window::Destroy()
{
	if (m_hwnd != NULL) {
		DestroyWindow(m_hwnd);
		m_hwnd = nullptr;
	}
}

LRESULT rendering::Window::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		{
			PostQuitMessage(0);
			Destroy();
			break;
		}

	case WM_KEYDOWN:
	{
		m_inputInfo.m_keysDown.insert(wParam);
		return 0;
	}

	case WM_KEYUP:
	{
		m_inputInfo.m_keysDown.erase(wParam);
		return 0;
	}

	case WM_LBUTTONDOWN:
	{
		m_inputInfo.m_leftMouseButtonDown = true;
		return 0;
	}
	case WM_LBUTTONUP:
	{
		m_inputInfo.m_leftMouseButtonDown = false;
		return 0;
	}

	case WM_RBUTTONDOWN:
	{
		m_inputInfo.m_rightMouseButtonDown = true;
		return 0;
	}
	case WM_RBUTTONUP:
	{
		m_inputInfo.m_rightMouseButtonDown = false;
		return 0;
	}

	case WM_INPUT:
	{
		UINT dwSize = sizeof(RAWINPUT);
		static BYTE lpb[sizeof(RAWINPUT)];
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

		RAWINPUT* raw = (RAWINPUT*)lpb;
		if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			if (raw->data.mouse.usFlags == MOUSE_MOVE_RELATIVE) {
				m_inputInfo.m_mouseMovement[0] += raw->data.mouse.lLastX;
				m_inputInfo.m_mouseMovement[1] += raw->data.mouse.lLastY;
			}
			else if (raw->data.mouse.usFlags == MOUSE_MOVE_ABSOLUTE) {
				m_inputInfo.m_mouseMovement[0] = raw->data.mouse.lLastX;
				m_inputInfo.m_mouseMovement[1] = raw->data.mouse.lLastY;
			}
		}
		return 0;
	}

	}

	return static_cast<LRESULT>(DefWindowProc(m_hwnd, uMsg, wParam, lParam));
}

void rendering::Window::WindowTick()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
		if (!GetMessage(&msg, NULL, 0, 0)) {
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void rendering::Window::RegisterRawInputDevice()
{
	RAWINPUTDEVICE rid[1];
	rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	rid[0].dwFlags = RIDEV_INPUTSINK;
	rid[0].hwndTarget = m_hwnd;
	RegisterRawInputDevices(rid, 1, sizeof(rid[0]));
}

const rendering::InputInfo& rendering::Window::GetInputInfo()
{
	return m_inputInfo;
}