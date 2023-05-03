#include "DXShader.h"

#include "DataLib.h"

#include <d3dcompiler.h>


rendering::DXShader::DXShader(const BaseObjectMeta& meta, const std::string& filePath) :
    BaseObject(meta)
{
    LoadPrecompiledShader(filePath);
}

rendering::DXShader::~DXShader()
{
}

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

void rendering::DXShader::LoadPrecompiledShader(const std::string& filePath)
{
    std::string fullPath = data::GetLibrary().GetRootDir() + filePath;
    std::wstring preCompiledNameW(fullPath.begin(), fullPath.end());

    THROW_ERROR(
        D3DReadFileToBlob(preCompiledNameW.c_str(), &m_shader),
        "Can't load the precompiled shader!"
    )
}

const ID3DBlob* rendering::DXShader::GetCompiledShader() const
{
	return m_shader.Get();
}

#undef THROW_ERROR
