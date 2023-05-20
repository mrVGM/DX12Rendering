cmake_minimum_required (VERSION 3.8)

add_library (RenderPass
	"public/RenderPass.h"
	"private/RenderPass.cpp"
	"public/RenderPassMeta.h"
	"private/RenderPassMeta.cpp"
	"private/DXClearRTRP.cpp"
	"public/DXClearRTRP.h"
	"private/DXClearRTRPMeta.cpp"
	"public/DXClearRTRPMeta.h"
)

target_include_directories(RenderPass INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(RenderPass PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(RenderPass INTERFACE PUBLIC RenderingCore)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RenderPass PROPERTY CXX_STANDARD 20)
endif()
