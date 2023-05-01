cmake_minimum_required (VERSION 3.8)

add_library (Rendering
	"public/d3dx12.h"
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
	"public/Rendering.h"
	"private/Rendering.cpp"
	"public/DXCommandQueue.h"
	"public/DXCommandQueueMeta.h"
	"private/DXCommandQueueMeta.cpp"
	"private/DXCommandQueue.cpp"
	"public/DXSwapChainMeta.h"
	"private/DXSwapChainMeta.cpp"
	"public/DXSwapChain.h"
	"private/DXSwapChain.cpp"
	"public/DXFenceMeta.h"
	"private/DXFenceMeta.cpp"
 "public/DXFence.h" "private/DXFence.cpp")

target_include_directories(Rendering INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Rendering PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(Rendering INTERFACE PUBLIC Jobs)

target_link_libraries(Rendering PRIVATE "d3d12.lib" "dxgi.lib" "d3dcompiler.lib" "dxguid.lib")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Rendering PROPERTY CXX_STANDARD 20)
endif()
