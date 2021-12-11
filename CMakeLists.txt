cmake_minimum_required(VERSION 3.0.0)
project(MasterEngine VERSION 0.1.0)

include_directories(./src)
include_directories(./inc)
link_directories(./lib)
link_directories(./bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_library(test #SHARED
./src/test/Test.h
./src/test/Test.cpp

./src/graphic/glad.c
./src/test/graphic/Graphic.h
./src/test/graphic/Graphic.cpp
)
target_link_libraries(test glfw3)

add_executable(main
./src/main.cpp
)
target_link_libraries(main test)

# -l -lGL -lX11 -lpthread -lXrandr -lXi -ldl

#newton raylib opengl32 gdi32 winmm
#set_target_properties(engine PROPERTIES LINK_FLAGS ./lib/resource.res)
