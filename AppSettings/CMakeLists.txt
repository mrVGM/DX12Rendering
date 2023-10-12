cmake_minimum_required (VERSION 3.8)

add_library (AppSettings
	"public/AppSettings.h"
	"private/AppSettings.cpp"
	"public/AppSettingsMeta.h"
	"private/AppSettingsMeta.cpp"
	"public/AppEntryPointMeta.h"
	"private/AppEntryPointMeta.cpp"
	"public/AppEntryPoint.h"
	"private/AppEntryPoint.cpp"
	"private/RendererEntryPointMetaTag.cpp"
	"public/RendererEntryPointMetaTag.h"
	"public/SceneConverterEntryPointMetaTag.h"
	"private/SceneConverterEntryPointMetaTag.cpp"
	"public/SettingsReaderMeta.h"
	"private/SettingsReaderMeta.cpp"
	"public/SettingsReader.h"
	"private/SettingsReader.cpp"
	"public/ReflectionEntryPointMetaTag.h"
	"private/ReflectionEntryPointMetaTag.cpp"
)

target_include_directories(AppSettings INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(AppSettings PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(AppSettings PUBLIC XMLReader)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET AppSettings PROPERTY CXX_STANDARD 20)
endif()
