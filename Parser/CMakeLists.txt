cmake_minimum_required (VERSION 3.8)

add_library (Parser
	"private/codeSource.cpp"
	"public/codeSource.h"
	"private/grammar.cpp"
	"public/grammar.h"
	"private/parser.cpp"
	"public/parser.h"
	"private/parserState.cpp"
	"public/parserState.h"
	"private/parserTable.cpp"
	"public/parserTable.h"
	"private/rule.cpp"
	"public/rule.h"
	"private/symbol.cpp"
	"public/symbol.h"
	"private/tokenizer.cpp"
	"private/tokenizer.h"
 )

target_include_directories(Parser INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Parser PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(Parser PUBLIC DataLib)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Parser PROPERTY CXX_STANDARD 20)
endif()
