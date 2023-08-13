cmake_minimum_required (VERSION 3.8)

add_library (DeferredRP
	"public/DXDeferredRPMeta.h"
	"private/DXDeferredRPMeta.cpp"
	"private/resources/DXGBufferAmbientLitTexMeta.cpp"
	"private/resources/DXGBufferAmbientLitTexMeta.h"
	"private/resources/DXGBufferDiffuseLitTexMeta.cpp"
	"private/resources/DXGBufferDiffuseLitTexMeta.h"
	"private/resources/DXGBufferSpecularLitTexMeta.cpp"
	"private/resources/DXGBufferSpecularLitTexMeta.h"
	"private/DeferredRendering.cpp"
	"private/DeferredRendering.h"
	"private/HelperMaterials/DXLightsCalculationsMaterial.cpp"
	"private/HelperMaterials/DXLightsCalculationsMaterial.h"
	"private/HelperMaterials/DXLightsCalculationsMaterialMeta.cpp"
	"private/HelperMaterials/DXLightsCalculationsMaterialMeta.h"
	"private/HelperMaterials/DXPostLightsCalculationsMaterialMeta.h"
	"private/HelperMaterials/DXPostLightsCalculationsMaterialMeta.cpp"
	"private/HelperMaterials/DXPostLightsCalculationsMaterial.h"
	"private/HelperMaterials/DXPostLightsCalculationsMaterial.cpp"
	"private/LightsManager.cpp"
	"private/LightsManager.h"
	"private/LightsManagerMeta.cpp"
	"private/LightsManagerMeta.h"
	"private/DXDeferredMaterialMeta.cpp"
	"public/DXDeferredMaterialMeta.h"
	"private/DXDeferredMaterial.cpp"
	"public/DXDeferredMaterial.h"
	"public/DXDeferredRP.h"
	"private/DXDeferredRP.cpp"
	"private/resources/DXLightsBufferMeta.h"
	"private/resources/DXLightsBufferMeta.cpp"
	"private/utils.h"
	"private/utils.cpp"
	"private/HelperMaterials/DXDisplaySMMaterialMeta.cpp"
	"private/HelperMaterials/DXDisplaySMMaterialMeta.h"
	"private/HelperMaterials/DXDisplaySMMaterial.cpp"
	"private/HelperMaterials/DXDisplaySMMaterial.h"
	"private/HelperMaterials/DXPostProcessMaterialMeta.h"
	"private/HelperMaterials/DXPostProcessMaterialMeta.cpp"
	"private/HelperMaterials/DXPostProcessMaterial.h"
	"private/HelperMaterials/DXPostProcessMaterial.cpp"
)

target_include_directories(DeferredRP INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(DeferredRP PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(DeferredRP PUBLIC RenderPass Materials Scene CommonResources ShadowMapping)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET DeferredRP PROPERTY CXX_STANDARD 20)
endif()
