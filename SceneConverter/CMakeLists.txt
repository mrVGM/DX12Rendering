cmake_minimum_required (VERSION 3.8)

add_library (SceneConverter
	"public/SceneConverter.h"
	"private/SceneConverter.cpp"
	"private/SceneConverterEntryPointMeta.cpp"
	"private/SceneConverterEntryPointMeta.h"
	"private/SceneConverterEntryPoint.cpp"
	"private/SceneConverterEntryPoint.h"
	"private/SceneConverterSettingsMeta.cpp"
	"private/SceneConverterSettingsMeta.h"
	"private/SceneConverterSettings.cpp"
	"private/SceneConverterSettings.h"
)

target_include_directories(SceneConverter INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(SceneConverter PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(SceneConverter PRIVATE Collada)
target_link_libraries(SceneConverter INTERFACE PUBLIC
	AppSettings
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SceneConverter PROPERTY CXX_STANDARD 20)
endif()
