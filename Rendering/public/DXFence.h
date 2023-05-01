#pragma once

#include "BaseObject.h"

#include <string>
#include <d3d12.h>
#include <wrl.h>

class BaseObjectMeta;

namespace rendering
{
	class DXFence : public BaseObject
	{
	private:
		int m_eventCounter = 0;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
		bool Create(std::string& errorMessage);
	public:
		ID3D12Fence* GetFence() const;

		DXFence(const BaseObjectMeta& meta);
		virtual ~DXFence();
	};
}