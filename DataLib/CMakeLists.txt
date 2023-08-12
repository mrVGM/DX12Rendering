cmake_minimum_required (VERSION 3.8)

add_library (DataLib
	"public/DataLib.h"
	"public/json.hpp"
	"private/DataLib.cpp"
	"public/MemoryFile.h"
	"private/MemoryFile.cpp"
)

target_include_directories(DataLib INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(DataLib PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET DataLib PROPERTY CXX_STANDARD 20)
endif()
