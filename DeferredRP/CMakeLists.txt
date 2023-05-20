cmake_minimum_required (VERSION 3.8)

add_library (DeferredRP
	"public/DXDeferredRPMeta.h"
	"private/DXDeferredRPMeta.cpp"
 "private/DXGBufferAmbientLitTexMeta.cpp" "public/DXGBufferAmbientLitTexMeta.h" "private/DXGBufferDiffuseLitTexMeta.cpp" "public/DXGBufferDiffuseLitTexMeta.h" "private/DXGBufferDiffuseTexMeta.cpp" "public/DXGBufferDuffuseTexMeta.h" "private/DXGBufferNormalTexMeta.cpp" "public/DXGBufferNormalTexMeta.h" "private/DXGBufferPositionTexMeta.cpp" "public/DXGBufferPositionTexMeta.h" "private/DXGBufferSpecularLitTexMeta.cpp" "public/DXGBufferSpecularLitTexMeta.h" "private/DXGBufferSpecularTexMeta.cpp" "public/DXGBufferSpecularTexMeta.h" "private/DeferredRendering.cpp" "public/DeferredRendering.h")

target_include_directories(DeferredRP INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(DeferredRP PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(DeferredRP INTERFACE PUBLIC RenderPass Materials Scene)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET DeferredRP PROPERTY CXX_STANDARD 20)
endif()
