cmake_minimum_required (VERSION 3.8)

add_library (CommonResources
	"public/DXCameraBufferMeta.h"
	"private/DXCameraBufferMeta.cpp"
	"public/ICameraMeta.h"
	"private/ICameraMeta.cpp"
	"public/ICamera.h"
	"private/ICamera.cpp"
	"public/TickUpdaterMeta.h"
	"private/TickUpdaterMeta.cpp"
	"public/TickUpdater.h"
	"private/TickUpdater.cpp"
	"private/DXGBufferDiffuseTexMeta.cpp"
	"public/DXGBufferDuffuseTexMeta.h"
	"private/DXGBufferNormalTexMeta.cpp"
	"public/DXGBufferNormalTexMeta.h"
	"private/DXGBufferPositionTexMeta.cpp"
	"public/DXGBufferPositionTexMeta.h"
	"private/DXGBufferSpecularTexMeta.cpp"
	"public/DXGBufferSpecularTexMeta.h"
	"public/DXRenderTextureVertexBufferMeta.h"
	"private/DXRenderTextureVertexBufferMeta.cpp"
	"public/LoadJobSystemMeta.h"	
	"private/LoadJobSystemMeta.cpp"
	"public/DXCameraDepthTexMeta.h"
	"private/DXCameraDepthTexMeta.cpp"
	"public/TextureLoadedNotificationMeta.h"
	"private/TextureLoadedNotificationMeta.cpp"
	"public/AsyncTickUpdaterMeta.h"
	"private/AsyncTickUpdaterMeta.cpp"
	"public/AsyncTickUpdater.h"
	"private/AsyncTickUpdater.cpp"
)

target_include_directories(CommonResources INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(CommonResources PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(CommonResources PUBLIC RenderingCore Notifications)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CommonResources PROPERTY CXX_STANDARD 20)
endif()
