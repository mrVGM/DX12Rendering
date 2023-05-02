#pragma once

#include "BaseObject.h"

#include <d3d12.h>
#include <string>

namespace rendering
{
	class DXRenderer : public BaseObject
	{
	private:
		UINT64 m_counter = 1;
	public:
		DXRenderer();
		virtual ~DXRenderer();

		bool Render(std::string& errorMessage);
		void RenderFrame();
		void StartRendering();
	};
}