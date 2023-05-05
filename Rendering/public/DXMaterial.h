#pragma once

#include "BaseObject.h"

#include <string>
#include <d3d12.h>
#include <wrl.h>

#include <list>

class BaseObjectMeta;

namespace rendering
{
	class DXShader;
	class DXBuffer;

	class DXMaterial : public BaseObject
	{
	protected:
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		std::list<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> > m_commandLists;

		const DXShader& m_vertexShader;
		const DXShader& m_pixelShader;

	public:
		DXMaterial(const BaseObjectMeta& meta, const DXShader& vertexShader, const DXShader& pixelShader);
		virtual ~DXMaterial();

		void ResetCommandLists();
		virtual void GenerateCommandList(const DXBuffer& vertexBuffer, const DXBuffer& indexBuffer, const DXBuffer& instanceBuffer) = 0;

		const std::list<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> >& GetGeneratedCommandLists() const;
	};
}