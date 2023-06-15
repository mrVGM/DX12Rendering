cmake_minimum_required (VERSION 3.8)

add_library (ShadowMapping
	"public/ShadowMapping.h"
	"private/ShadowMapping.cpp"
)

target_include_directories(ShadowMapping INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(ShadowMapping PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(ShadowMapping PRIVATE CascadedSM PSM)
target_link_libraries(ShadowMapping INTERFACE PUBLIC Base Jobs)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ShadowMapping PROPERTY CXX_STANDARD 20)
endif()
