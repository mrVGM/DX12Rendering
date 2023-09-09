cmake_minimum_required (VERSION 3.8)

add_library (Animation
	"private/utils.cpp"
	"private/utils.h"
	"public/AnimRepo.cpp"
	"private/AnimRepoMeta.cpp" 
	"public/AnimRepo.h"
	"private/AnimRepoMeta.h"
	"public/Animation.h"
	"private/Animation.cpp"
)

target_include_directories(Animation INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Animation PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(Animation PUBLIC Scene)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Animation PROPERTY CXX_STANDARD 20)
endif()
