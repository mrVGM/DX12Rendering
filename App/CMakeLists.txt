cmake_minimum_required (VERSION 3.8)

add_library (App
	"public/App.h"
	"private/App.cpp"
)

target_include_directories(App INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(App PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(App PRIVATE
	AppSettings
	Renderer
	SceneConverter
	Reflection
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET App PROPERTY CXX_STANDARD 20)
endif()
