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
	"private/Materials/DXUnlitMaterialMeta.h"
	"private/Materials/DXUnlitMaterialMeta.cpp"
	"private/Materials/DXUnlitMaterial.h"
	"private/Materials/DXUnlitMaterial.cpp"
	
	
	
	
	
	
	"private/RenderPass/DXDeferredRP.h"
	"private/RenderPass/DXDeferredRP.cpp"
	"private/Materials/DXDeferredMaterial.h"
	"private/Materials/DXDeferredMaterial.cpp"
	"private/Materials/DXDeferredMaterialMeta.h"
	"private/Materials/DXDeferredMaterialMeta.cpp"
	
	
	
	
	
	
	
	
	"private/Lights/LightsManagerMeta.h"
	"private/Lights/LightsManagerMeta.cpp"
	"private/Lights/LightsManager.h"
	"private/Lights/LightsManager.cpp"
	
	
	"private/Materials/DXShadowMapMaterialMeta.h"
	"private/Materials/DXShadowMapMaterialMeta.cpp"
	"private/Materials/DXShadowMapMaterial.h"
	"private/Materials/DXShadowMapMaterial.cpp"
	"private/Lights/DXShadowMapDSMeta.h"
	"private/Lights/DXShadowMapDSMeta.cpp"
	"private/Lights/DXShadowMapDSDescriptorHeapMeta.h"
	"private/Lights/DXShadowMapDSDescriptorHeapMeta.cpp"
	"private/RenderPass/DXDisplaySMRPMeta.h"
	"private/RenderPass/DXDisplaySMRPMeta.cpp"
	"private/RenderPass/DXDisplaySMRP.h"
	"private/RenderPass/DXDisplaySMRP.cpp"
	"private/Materials/MaterialUtils.h"
	"private/Materials/MaterialUtils.cpp"
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
