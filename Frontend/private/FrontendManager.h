#pragma once

#include "BaseObject.h"

namespace frontend
{
	class FrontendManager : public BaseObject
	{
	public:
		FrontendManager();
		virtual ~FrontendManager();

		void OpenConnection();
	};
}