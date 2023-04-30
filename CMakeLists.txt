# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("DX12Rendering")

add_definitions(-DUNICODE)

# Include sub-projects.
add_subdirectory ("Main")
add_subdirectory ("Base")
add_subdirectory ("Jobs")
add_subdirectory ("Rendering")
add_subdirectory ("DataLib")
