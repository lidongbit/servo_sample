cmake_minimum_required(VERSION 2.8.3)
project(servo_sample)

include_directories(
	../
	include
	../buff_help/include/
	../list_help/include/
	${rtm_INCLUDE_DIRS}
)

aux_source_directory(src SRC)

add_executable(servo_sample
	src/sample.c
)

add_library(sample_help STATIC
	src/sample_help.c
)
target_link_libraries(sample_help
	${rtm_LIBRARIES}
	buff_help
	list_help
)

link_directories(
	../../install/lib
)

target_link_libraries(servo_sample
	${rtm_LIBRARIES}
	buff_help
	list_help
	sample_help
)


