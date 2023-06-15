cmake_minimum_required (VERSION 3.8)

add_library (PSM
	"public/PSM.h"
	"private/PSM.cpp"
	"private/PSMMeta.h"
	"private/PSMMeta.cpp"
	"private/resources/DXShadowMapMeta.h"
	"private/resources/DXShadowMapMeta.cpp"
	"private/resources/DXShadowMapDSMeta.h"
	"private/resources/DXShadowMapDSMeta.cpp"
	"private/resources/DXShadowMaskMeta.h"
	"private/resources/DXShadowMaskMeta.cpp"
	"private/resources/DXSMSettingsBufferMeta.h"
	"private/resources/DXSMSettingsBufferMeta.cpp"
	"private/updaters/DXShadowMapUpdater.cpp"
	"private/updaters/DXShadowMapUpdater.h"
	"private/updaters/DXShadowMapUpdaterMeta.cpp"
	"private/updaters/DXShadowMapUpdaterMeta.h"
	"private/utils.cpp"
	"private/utils.h"
)

target_include_directories(PSM INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(PSM PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(PSM INTERFACE PUBLIC RenderingCore CommonResources Scene Materials ShadowMapping)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET PSM PROPERTY CXX_STANDARD 20)
endif()
