cmake_minimum_required (VERSION 3.8)

add_library (Rendering
	"public/Rendering.h"
	"private/Rendering.cpp"
	"public/DXRendererMeta.h"
	"private/DXRendererMeta.cpp"
	"public/DXRenderer.h"
	"private/DXRenderer.cpp"
	"private/RenderFenceMeta.h"
	"private/RenderFenceMeta.cpp"
	"private/RenderUtils.h"
	"private/RenderUtils.cpp"
	"public/DXCameraMeta.h"
	"private/DXCameraMeta.cpp"
	"public/DXCamera.h"
	"private/DXCamera.cpp"
	"private/DXComputeShaderMeta.h"
	"private/DXComputeShaderMeta.cpp"
	"private/Materials/DXUnlitErrorMaterial.h"
	"private/Materials/DXUnlitErrorMaterial.cpp"
	"private/Materials/DXUnlitErrorMaterialMeta.h"
	"private/Materials/DXUnlitErrorMaterialMeta.cpp"
	"private/Materials/DXUnlitErrorMaterial.h"
	"private/Materials/DXUnlitErrorMaterial.cpp"
	"public/UpdaterMeta.h"
	"private/UpdaterMeta.cpp"
	"public/Updater.h"
	"private/Updater.cpp"
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	"private/RenderPass/DXDisplaySMRPMeta.h"
	"private/RenderPass/DXDisplaySMRPMeta.cpp"
	"private/RenderPass/DXDisplaySMRP.h"
	"private/RenderPass/DXDisplaySMRP.cpp"
	"private/Materials/MaterialUtils.h"
	"private/Materials/MaterialUtils.cpp"
	"private/DXCameraUpdaterMeta.h"
	"private/DXCameraUpdaterMeta.cpp"
	"private/DXCameraUpdater.h"
	"private/DXCameraUpdater.cpp"
	"private/DXCameraRDUMeta.h"
	"private/DXCameraRDUMeta.cpp"
	"private/DXCameraRDU.h"
	"private/DXCameraRDU.cpp"
)

target_include_directories(Rendering INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Rendering PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(Rendering INTERFACE PUBLIC 
	Jobs 
	DataLib 
	Collada 
	RenderingCore 
	Scene 
	RenderPass 
	Materials 
	UnlitRP 
	DeferredRP
	CommonResources
)

target_link_libraries(Rendering PRIVATE "d3d12.lib" "dxgi.lib" "d3dcompiler.lib" "dxguid.lib")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Rendering PROPERTY CXX_STANDARD 20)
endif()
