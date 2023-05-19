cmake_minimum_required (VERSION 3.8)

add_library (RenderingCore
	"private/d3dx12.h"
	"public/Window.h"
	"private/Window.cpp"
	"public/WindowMeta.h"
	"private/WindowMeta.cpp"
	"private/WindowUpdateJobSystemMeta.h"
	"private/WindowUpdateJobSystemMeta.cpp"
	"public/DXDevice.h"
	"private/DXDevice.cpp"
	"public/DXDeviceMeta.h"
	"private/DXDeviceMeta.cpp"
	"public/DXSwapChain.h"
	"public/DXSwapChainMeta.h"
	"private/DXSwapChain.cpp"
	"private/DXSwapChainMeta.cpp"
	"private/CoreUtils.h"
	"private/CoreUtils.cpp"
	"public/DXCommandQueue.h"
	"public/DXCommandQueueMeta.h"
	"private/DXCommandQueue.cpp"
	"private/DXCommandQueueMeta.cpp"
	"public/RenderingCore.h"
 "private/RenderingCore.cpp" "public/MainJobSystemMeta.h" "private/MainJobSystemMeta.cpp" "private/LoadJobSystemMeta.cpp" "public/LoadJobSystemMeta.h")

target_include_directories(RenderingCore INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(RenderingCore PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(RenderingCore INTERFACE PUBLIC Base Jobs)

target_link_libraries(RenderingCore PRIVATE "d3d12.lib" "dxgi.lib" "d3dcompiler.lib" "dxguid.lib")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RenderingCore PROPERTY CXX_STANDARD 20)
endif()
