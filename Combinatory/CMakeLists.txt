cmake_minimum_required (VERSION 3.8)

add_library (Combinatory
	"public/Combinatory.h"
	"private/Combinatory.cpp"
	"private/CombinatoryEntryPointMeta.h"
	"private/CombinatoryEntryPointMeta.cpp"
	"private/CombinatoryEntryPoint.h"
	"private/CombinatoryEntryPoint.cpp"
	"private/CombinatorySettings.cpp"
	"private/CombinatorySettings.h"
	"private/CombinatorySettingsMeta.h"
	"private/CombinatorySettingsMeta.cpp"
	"private/utils.cpp"
	"private/utils.h"
	"private/Entities.h"
	"private/ItemManager.h"
	"private/ItemManager.cpp"
	"private/VariationNumber.h"
	"private/VariationNumber.cpp"
	"private/Entities.cpp"
	"private/BlockGroupProcessor.h"
	"private/BlockGroupProcessor.cpp"
)

target_include_directories(Combinatory INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Combinatory PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(Combinatory PRIVATE XMLReader Jobs)
target_link_libraries(Combinatory INTERFACE PUBLIC
	AppSettings
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Combinatory PROPERTY CXX_STANDARD 20)
endif()
