cmake_minimum_required (VERSION 3.8)

add_library (Collada
	"private/Collada.h"
	"private/Collada.cpp"
	"private/ColladaTreeBuilder.h"
	"private/ColladaTreeBuilder.cpp"
	"public/ColladaReaderMeta.h"
	"private/ColladaReaderMeta.cpp"
	"public/ColladaReader.h"
	"private/ColladaReader.cpp"
)

target_include_directories(Collada INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Collada PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(Collada PRIVATE DataLib Parser Base)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Collada PROPERTY CXX_STANDARD 20)
endif()
