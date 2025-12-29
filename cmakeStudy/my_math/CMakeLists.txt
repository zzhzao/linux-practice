# 1 设置最低版本号

cmake_minimum_required(VERSION 3.18)
# 2 设置项⽬名称

project(TestMyMath
 LANGUAGES CXX
)

# 3 添加⽬录层级

add_subdirectory(my_lib)
add_subdirectory(app)
