cmake_minimum_required (VERSION 3.8)

add_library (Reflection
	"private/utils.cpp"
	"private/utils.h"
	"public/Reflection.h"
	"private/Reflection.cpp"
	"public/Type.h"
	"private/Type.cpp"
	"public/TypeManager.h"
	"private/TypeManager.cpp"
	"public/Property.h"
	"private/Property.cpp"
	"private/GeneratedObjectMeta.cpp"
	"private/GeneratedObjectMeta.h"
	"private/ReflectionSettings.cpp"
	"private/ReflectionSettings.h"
	"private/ReflectionSettingsMeta.cpp"
	"private/ReflectionSettingsMeta.h"
)

target_include_directories(Reflection INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Reflection PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(Reflection PUBLIC Base CommonResources AppSettings)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Reflection PROPERTY CXX_STANDARD 20)
endif()
