#include "CommandListCache.h"

rendering::CommandListCache::CommandListCache()
{
}

ID3D12CommandList* rendering::CommandListCache::GetCommandList(const std::string& materialName, const std::string& objectName)
{
	auto matIt = m_materialCLs.find(materialName);

	if (matIt == m_materialCLs.end())
	{
		return nullptr;
	}

	MaterialCLs& matCLs = matIt->second;
	auto objIt = matCLs.m_objectCL.find(objectName);

	if (objIt == matCLs.m_objectCL.end())
	{
		return nullptr;
	}

	return objIt->second;
}

void rendering::CommandListCache::CacheCommandList(const std::string& materialName, const std::string& objectName, ID3D12CommandList* commandList)
{
	auto matIt = m_materialCLs.find(materialName);

	if (matIt == m_materialCLs.end())
	{
		m_materialCLs[materialName] = MaterialCLs();
		matIt = m_materialCLs.find(materialName);
	}

	MaterialCLs& matCLs = matIt->second;
	matCLs.m_objectCL[objectName] = commandList;
}

void rendering::CommandListCache::ResetMaterialCache(const std::string& materialName)
{
	auto matIt = m_materialCLs.find(materialName);

	if (matIt == m_materialCLs.end())
	{
		return;
	}

	matIt->second.m_objectCL.clear();
}

void rendering::CommandListCache::Reset(const std::string& materialName)
{
	m_materialCLs.clear();
}