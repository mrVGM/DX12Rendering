cmake_minimum_required (VERSION 3.8)

add_library (AppSettings
	"public/AppSettings.h"
	"private/AppSettings.cpp"
	"public/AppSettingsMeta.h"
	"private/AppSettingsMeta.cpp"
)

target_include_directories(AppSettings INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(AppSettings PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(AppSettings PRIVATE XMLReader)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET AppSettings PROPERTY CXX_STANDARD 20)
endif()
