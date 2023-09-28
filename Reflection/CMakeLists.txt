cmake_minimum_required (VERSION 3.8)

add_library (Reflection
	"private/utils.cpp"
	"private/utils.h"
	"public/Reflection.h"
	"private/Reflection.cpp"
	"public/Type.h"
	"private/Type.cpp"
	"private/TypeManager.h"
	"private/TypeManager.cpp"
)

target_include_directories(Reflection INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Reflection PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(Reflection PUBLIC Base)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Reflection PROPERTY CXX_STANDARD 20)
endif()
