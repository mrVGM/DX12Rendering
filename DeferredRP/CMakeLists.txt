cmake_minimum_required (VERSION 3.8)

add_library (DeferredRP
	"public/DXDeferredRPMeta.h"
	"public/DXDeferredRPMeta.cpp"
)

target_include_directories(DeferredRP INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(DeferredRP PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(DeferredRP INTERFACE PUBLIC RenderPass Materials Scene)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET DeferredRP PROPERTY CXX_STANDARD 20)
endif()
