cmake_minimum_required (VERSION 3.8)

add_library (Animation
	"private/utils.cpp"
	"private/utils.h"
	"private/AnimRepo.cpp"
	"private/AnimRepoMeta.cpp" 
	"public/AnimRepo.h"
	"public/AnimRepoMeta.h"
	"public/Animation.h"
	"private/Animation.cpp"
	"public/AnimatorMeta.h"
	"private/AnimatorMeta.cpp"
	"public/Animator.h"
	"private/Animator.cpp"
	"private/AnimatorUpdaterMeta.h"
	"private/AnimatorUpdaterMeta.cpp"
	"private/AnimatorUpdater.cpp"
	"private/AnimatorUpdater.h"
)

target_include_directories(Animation INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Animation PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(Animation PUBLIC Scene CommonResources)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Animation PROPERTY CXX_STANDARD 20)
endif()
