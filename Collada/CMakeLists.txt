cmake_minimum_required (VERSION 3.8)

add_library (Collada
	"private/Collada.h"
	"public/ColladaEntities.h"
	"private/ColladaSceneBuilder.cpp"
	"public/ColladaSceneMeta.h"
	"private/ColladaSceneMeta.cpp"
	"public/ColladaScene.h"
	"private/ColladaScene.cpp"
)

target_include_directories(Collada INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Collada PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(Collada PRIVATE DataLib)
target_link_libraries(Collada INTERFACE PUBLIC Parser Base XMLReader)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Collada PROPERTY CXX_STANDARD 20)
endif()
