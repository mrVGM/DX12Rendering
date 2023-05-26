cmake_minimum_required (VERSION 3.8)

add_library (CommonResources
	"public/DXCameraBufferMeta.h"
	"private/DXCameraBufferMeta.cpp"
	"public/ICameraMeta.h"
	"private/ICameraMeta.cpp"
	"public/ICamera.h"
	"private/ICamera.cpp"
	"public/RenderDataUpdaterMeta.h"
	"private/RenderDataUpdaterMeta.cpp"
	"public/RenderDataUpdater.h"
	"private/RenderDataUpdater.cpp"
	"public/TickUpdaterMeta.h"
	"private/TickUpdaterMeta.cpp"
	"public/TickUpdater.h"
	"private/TickUpdater.cpp")

target_include_directories(CommonResources INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(CommonResources PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(CommonResources INTERFACE PUBLIC RenderingCore)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CommonResources PROPERTY CXX_STANDARD 20)
endif()
