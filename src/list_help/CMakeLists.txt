cmake_minimum_required(VERSION 2.8.3)
project(list_help)

include_directories(
	include
	${rtm_INCLUDE_DIRS}
)

aux_source_directory(src SRC)
add_library(list_help STATIC
	${SRC}
)

target_link_libraries(list_help
    ${rtm_LIBRARIES}
)
