#pragma once

#include "BaseObject.h"

#include <Windows.h>

namespace frontend
{
	class FrontendManager : public BaseObject
	{
	private:
		HANDLE m_pipe = nullptr;
		bool m_toShutdown = false;

	public:
		FrontendManager();
		virtual ~FrontendManager();

		void OpenConnection();
		void Shutdown();
	};
}