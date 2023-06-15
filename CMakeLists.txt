# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("DX12Rendering")

add_definitions(-DUNICODE)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG=1)
ENDIF()

# Include sub-projects.
add_subdirectory ("Main")
add_subdirectory ("Base")
add_subdirectory ("Jobs")
add_subdirectory ("Renderer")
add_subdirectory ("DataLib")
add_subdirectory ("Parser")
add_subdirectory ("Collada")
add_subdirectory ("RenderingCore")
add_subdirectory ("Scene")
add_subdirectory ("RenderPass")
add_subdirectory ("Materials")
add_subdirectory ("UnlitRP")
add_subdirectory ("DeferredRP")
add_subdirectory ("CommonResources")
add_subdirectory ("Profiling")
add_subdirectory ("CascadedSM")
add_subdirectory ("Notifications")
add_subdirectory ("ShadowMapping")
add_subdirectory ("XMLReader")
add_subdirectory ("AppSettings")
