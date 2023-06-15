#include "RendererEntryPoint.h"

#include "Renderer.h"

rendering::RendererEntryPoint::RendererEntryPoint(const BaseObjectMeta& meta) :
	settings::AppEntryPoint(meta)
{
}

rendering::RendererEntryPoint::~RendererEntryPoint()
{
}

void rendering::RendererEntryPoint::Boot()
{
	rendering::Boot();
}