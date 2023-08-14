cmake_minimum_required (VERSION 3.8)

add_library (PostProcessRP
	"public/DXPostProcessRPMeta.h"
	"private/DXPostProcessRPMeta.cpp"
	"private/DXPostProcessRP.cpp"
	"public/DXPostProcessRP.h"
	"private/utils.cpp"
	"private/utils.h"
	"private/Materials/DXOutlineMaterial.cpp"
	"private/Materials/DXOutlineMaterial.h"
	"private/Materials/DXOutlineMaterialMeta.cpp"
	"private/Materials/DXOutlineMaterialMeta.h"
)

target_include_directories(PostProcessRP INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(PostProcessRP PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(PostProcessRP PUBLIC RenderPass Materials CommonResources)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET PostProcessRP PROPERTY CXX_STANDARD 20)
endif()
