cmake_minimum_required (VERSION 3.8)

add_library (CommonResources
	"public/DXCameraBufferMeta.h"
	"private/DXCameraBufferMeta.cpp"
 "public/DXLightsBufferMeta.h" "private/DXLightsBufferMeta.cpp" "private/DXShadowMapMeta.cpp" "public/DXShadowMapMeta.h")

target_include_directories(CommonResources INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(CommonResources PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(CommonResources INTERFACE PUBLIC RenderingCore)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CommonResources PROPERTY CXX_STANDARD 20)
endif()
