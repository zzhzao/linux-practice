file(GLOB SRC_LISTS "src/*.cpp")

add_library(MyMath STATIC $(SRC_LISTS))

target_include_directories(MyMath
PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include
)

set_target_properties(MyMath PROPERTIES
ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)