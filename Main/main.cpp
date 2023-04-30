#include <iostream>

#include "BaseFramework.h"

#include "Window.h"

int main()
{
	rendering::Window* wnd = new rendering::Window();

	std::cin.get();
	BaseFrameworkShutdown();

	return 0;
}
