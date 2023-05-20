cmake_minimum_required (VERSION 3.8)

add_library (Materials
	"public/DXMaterial.h"
	"private/DXMaterial.cpp"
	"public/DXMaterialMeta.h"
	"private/DXMaterialMeta.cpp"
 "private/DXUnlitMaterialMetaTag.cpp" "public/DXUnlitMaterialMetaTag.h" "private/DXDeferredMaterialMetaTag.cpp" "public/DXDeferredMaterialMetaTag.h" "public/DXMaterialRepo.h" "public/DXMaterialRepoMeta.h" "private/DXMaterialRepo.cpp" "private/DXMaterialRepoMeta.cpp")

target_include_directories(Materials INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Materials PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(Materials INTERFACE PUBLIC RenderingCore Collada)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Materials PROPERTY CXX_STANDARD 20)
endif()
