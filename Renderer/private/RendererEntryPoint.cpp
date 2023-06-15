#include "RendererEntryPoint.h"

#include "RendererAppEntryPointMeta.h"

#include "Renderer.h"

rendering::RendererEntryPoint::RendererEntryPoint() :
	settings::AppEntryPoint(RendererAppEntryPointMeta::GetInstance())
{
}

rendering::RendererEntryPoint::~RendererEntryPoint()
{
}

void rendering::RendererEntryPoint::Boot()
{
	rendering::Boot();
}