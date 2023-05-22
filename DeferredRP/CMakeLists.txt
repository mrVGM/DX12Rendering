cmake_minimum_required (VERSION 3.8)

add_library (DeferredRP
	"public/DXDeferredRPMeta.h"
	"private/DXDeferredRPMeta.cpp"
	"private/DXGBufferAmbientLitTexMeta.cpp"
	"public/DXGBufferAmbientLitTexMeta.h"
	"private/DXGBufferDiffuseLitTexMeta.cpp"
	"public/DXGBufferDiffuseLitTexMeta.h"
	"private/DXGBufferDiffuseTexMeta.cpp"
	"public/DXGBufferDuffuseTexMeta.h"
	"private/DXGBufferNormalTexMeta.cpp"
	"public/DXGBufferNormalTexMeta.h"
	"private/DXGBufferPositionTexMeta.cpp"
	"public/DXGBufferPositionTexMeta.h"
	"private/DXGBufferSpecularLitTexMeta.cpp"
	"public/DXGBufferSpecularLitTexMeta.h"
	"private/DXGBufferSpecularTexMeta.cpp"
	"public/DXGBufferSpecularTexMeta.h"
	"private/DeferredRendering.cpp"
	"public/DeferredRendering.h"
	"private/DXLightsCalculationsMaterial.cpp"
	"public/DXLightsCalculationsMaterial.h"
	"private/DXLightsCalculationsMaterialMeta.cpp"
	"public/DXLightsCalculationsMaterialMeta.h"
	"public/DXPostLightsCalculationsMaterialMeta.h"
	"private/DXPostLightsCalculationsMaterialMeta.cpp"
	"public/DXPostLightsCalculationsMaterial.h"
	"private/DXPostLightsCalculationsMaterial.cpp"
	"private/DXShadowMapDSDescriptorHeapMeta.cpp"
	"public/DXShadowMapDSDescriptorHeapMeta.h"
	"private/DXShadowMapDSMeta.cpp"
	"public/DXShadowMapDSMeta.h"
	"private/LightsManager.cpp"
	"public/LightsManager.h"
	"private/LightsManagerMeta.cpp"
	"public/LightsManagerMeta.h"
	"private/DXShadowMapMaterialMeta.cpp"
	"public/DXShadowMapMaterialMeta.h"
 "private/DXShadowMapMaterial.cpp" "public/DXShadowMapMaterial.h")

target_include_directories(DeferredRP INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(DeferredRP PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(DeferredRP INTERFACE PUBLIC RenderPass Materials Scene CommonResources)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET DeferredRP PROPERTY CXX_STANDARD 20)
endif()
