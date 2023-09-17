cmake_minimum_required (VERSION 3.8)

add_library (Frontend
	"public/Frontend.h"
	"private/Frontend.cpp"
	"private/FrontendJobSystemMeta.h"
	"private/FrontendJobSystemMeta.cpp"
	"private/utils.cpp"
	"private/utils.h"
	"private/FrontendManager.h"
	"private/FrontendManager.cpp"
	"private/FrontendManagerMeta.cpp"
	"private/FrontendManagerMeta.h"
)

target_include_directories(Frontend INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Frontend PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(Frontend PUBLIC Jobs CommonResources)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Frontend PROPERTY CXX_STANDARD 20)
endif()
