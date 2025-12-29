# 1 收集库的源代码 
file(GLOB SRC_LISTS "src/*.cpp")

# 2 添加构建⽬标 
add_library(MyMath STATIC ${SRC_LISTS})

# 3 设置库的使⽤要求 
target_include_directories(MyMath
 PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include
)

# 4 修改默认的输出路径 
set_target_properties(MyMath PROPERTIES
 ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
