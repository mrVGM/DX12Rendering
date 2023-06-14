cmake_minimum_required (VERSION 3.8)

add_library (XMLReader
	"public/XMLReader.h"
	"private/XMLReader.cpp"
	"private/TreeBuilder.h"
	"private/TreeBuilder.cpp"
	"private/XMLReaderMeta.h"
	"private/XMLReaderMeta.cpp"
)

target_include_directories(XMLReader INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(XMLReader PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(XMLReader PRIVATE DataLib)
target_link_libraries(XMLReader INTERFACE PUBLIC Parser Base)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET XMLReader PROPERTY CXX_STANDARD 20)
endif()
