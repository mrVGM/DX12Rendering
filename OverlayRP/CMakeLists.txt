cmake_minimum_required (VERSION 3.8)

add_library (OverlayRP
	"private/DXOverlayRPMeta.cpp"
	"public/DXOverlayRP.h"
	"private/DXOverlayRP.cpp"
	"private/utils.cpp"
	"private/utils.h"
	"private/Resources/QuadVertexBufferMeta.h"
	"private/Resources/QuadVertexBufferMeta.cpp"
	"private/Resources/QuadIndexBufferMeta.h"
	"private/Resources/QuadIndexBufferMeta.cpp"
	"private/Resources/QuadInstanceBufferMeta.h"
	"private/Resources/QuadInstanceBufferMeta.cpp"
)

target_include_directories(OverlayRP INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(OverlayRP PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(OverlayRP INTERFACE PUBLIC RenderPass Materials CommonResources)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET OverlayRP PROPERTY CXX_STANDARD 20)
endif()
