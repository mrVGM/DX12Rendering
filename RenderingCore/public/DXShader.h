#pragma once

#include "BaseObject.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>

class BaseObjectMeta;

namespace rendering
{
	class DXShader : public BaseObject
	{
		Microsoft::WRL::ComPtr<ID3DBlob> m_shader;
		void LoadPrecompiledShader(const std::string& filePath);
	public:
		DXShader(const BaseObjectMeta& meta, const std::string& filePath);
		virtual ~DXShader();

		ID3DBlob* GetCompiledShader() const;
	};
}