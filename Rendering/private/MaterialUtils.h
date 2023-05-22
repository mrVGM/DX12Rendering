#pragma once

#include "ColladaEntities.h"

class BaseObjectMeta;

namespace rendering::material_utils
{
	void LoadMaterial(const collada::ColladaMaterial& material);
	void EnableMaterialLoading();
}