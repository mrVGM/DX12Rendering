cmake_minimum_required (VERSION 3.8)

add_library (UnlitRP
	"public/DXUnlitRPMeta.h"
	"private/DXUnlitRPMeta.cpp"
	"private/DXUnlitRP.cpp"
	"public/DXUnlitRP.h"
	"private/DXUnlitRP.cpp"
	"private/DXUnlitRPMeta.cpp"
	"public/DXUnlitRP.h"
	"public/DXUnlitRPMeta.h"
	"private/DXUnlitErrorMaterialMeta.cpp"
	"public/DXUnlitErrorMaterialMeta.h"
	"private/DXUnlitErrorMaterial.cpp"
	"public/DXUnlitErrorMaterial.h"
	"private/utils.h"
	"private/utils.cpp"
)

target_include_directories(UnlitRP INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(UnlitRP PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(UnlitRP INTERFACE PUBLIC RenderPass Materials Scene CommonResources)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET UnlitRP PROPERTY CXX_STANDARD 20)
endif()
