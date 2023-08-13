cmake_minimum_required (VERSION 3.8)

add_library (Scene
	"public/DXScene.h"
	"public/DXSceneMeta.h"
	"private/DXScene.cpp"
	"private/DXSceneMeta.cpp"
	"public/SceneLoadedNotificationMeta.h"
	"private/SceneLoadedNotificationMeta.cpp"
	"private/utils.h"
	"private/utils.cpp"
	"private/SceneMaterialsSettingsMeta.cpp"
	"private/SceneMaterialsSettingsMeta.h"
	"private/SceneMaterialsSettings.cpp"
	"private/SceneMaterialsSettings.h"
)

target_include_directories(Scene INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Scene PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(Scene PUBLIC Collada RenderingCore Notifications)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Scene PROPERTY CXX_STANDARD 20)
endif()
