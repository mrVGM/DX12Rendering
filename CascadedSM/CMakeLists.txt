cmake_minimum_required (VERSION 3.8)

add_library (CascadedSM
	"private/CascadedSMMeta.h"
	"private/CascadedSMMeta.cpp"
	"public/CascadedSM.h"
	"private/CascadedSM.cpp"
	"private/resources/DXShadowMapDSDescriptorHeapMeta.cpp"
	"private/resources/DXShadowMapDSDescriptorHeapMeta.h"
	"private/resources/DXShadowMapDSMeta.cpp"
	"private/resources/DXShadowMapDSMeta.h"
	"private/resources/DXShadowMapFilterTexMeta.cpp"
	"private/resources/DXShadowMapFilterTexMeta.h"
	"private/resources/DXShadowMapMeta.cpp"
	"private/resources/DXShadowMapMeta.h"
	"private/resources/DXShadowMaskMeta.cpp"
	"private/resources/DXShadowMaskMeta.h"
	"private/resources/DXShadowSQMapMeta.cpp"
	"private/resources/DXShadowSQMapMeta.h"
	"private/resources/DXSMSettingsBufferMeta.cpp"
	"private/resources/DXSMSettingsBufferMeta.h"
	"private/utils.cpp"
	"private/utils.h"
	"private/updaters/DXShadowMapRDU.cpp"
	"private/updaters/DXShadowMapRDU.h"
	"private/updaters/DXShadowMapRDUMeta.cpp"
	"private/updaters/DXShadowMapRDUMeta.h"
	"private/updaters/DXShadowMapUpdater.cpp"
	"private/updaters/DXShadowMapUpdater.h"
	"private/updaters/DXShadowMapUpdaterMeta.cpp"
	"private/updaters/DXShadowMapUpdaterMeta.h"
	"private/HelperMaterials/DXShadowMapFilterMaterial.cpp"
	"private/HelperMaterials/DXShadowMapFilterMaterial.h"
	"private/HelperMaterials/DXShadowMapFilterMaterialMeta.cpp"
	"private/HelperMaterials/DXShadowMapFilterMaterialMeta.h"
	"private/HelperMaterials/DXShadowMapMaterial.cpp"
	"private/HelperMaterials/DXShadowMapMaterial.h"
	"private/HelperMaterials/DXShadowMapMaterialMeta.cpp"
	"private/HelperMaterials/DXShadowMapMaterialMeta.h"
	"private/HelperMaterials/DXShadowMaskFilterMaterial.cpp"
	"private/HelperMaterials/DXShadowMaskFilterMaterial.h"
	"private/HelperMaterials/DXShadowMaskFilterMaterialMeta.cpp"
	"private/HelperMaterials/DXShadowMaskFilterMaterialMeta.h"
	"private/HelperMaterials/DXShadowMaskMaterial.cpp"
	"private/HelperMaterials/DXShadowMaskMaterial.h"
	"private/HelperMaterials/DXShadowMaskMaterialMeta.cpp"
	"private/HelperMaterials/DXShadowMaskMaterialMeta.h"
	"public/CommandListCache.h"
	"private/CommandListCache.cpp"
)

target_include_directories(CascadedSM INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(CascadedSM PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(CascadedSM INTERFACE PUBLIC RenderingCore CommonResources Scene Materials ShadowMapping)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CascadedSM PROPERTY CXX_STANDARD 20)
endif()
