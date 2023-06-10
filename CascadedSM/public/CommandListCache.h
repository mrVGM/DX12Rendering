#pragma once

#include <d3d12.h>

#include <string>
#include <map>

namespace rendering
{
	class CommandListCache
	{
	private:
		friend class CascadedSM;

		struct MaterialCLs
		{
			std::string m_materialName;
			std::map<std::string, ID3D12CommandList*> m_objectCL;
		};

		std::map<std::string, MaterialCLs> m_materialCLs;

		CommandListCache();

		ID3D12CommandList* GetCommandList(const std::string& materialName, const std::string& objectName);
		void CacheCommandList(const std::string& materialName, const std::string& objectName, ID3D12CommandList* commandList);
		void ResetMaterialCache(const std::string& materialName);
		void Reset(const std::string& materialName);
	};
}