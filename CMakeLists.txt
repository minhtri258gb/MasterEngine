cmake_minimum_required(VERSION 3.0.0)
project(MasterEngine VERSION 0.1.0)

include_directories(./src)
include_directories(./inc)
link_directories(./lib)
link_directories(./bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

add_library(engine #SHARED
./src/engine/library.cpp
./src/engine/Config.h
./src/engine/Config.cpp
./src/engine/exception/Exception.h
./src/engine/exception/Exception.cpp
./src/engine/load/TextFile.h
./src/engine/load/TextFile.cpp
)

add_library(graphic #SHARED
./src/graphic/glad.c
./src/graphic/Graphic.h
./src/graphic/Graphic.cpp
./src/graphic/Scene.h
./src/graphic/Scene.cpp
./src/graphic/Screen.h
./src/graphic/Screen.cpp
./src/graphic/Camera.h
./src/graphic/Camera.cpp
./src/graphic/ShaderProgram.h
./src/graphic/ShaderProgram.cpp
./src/graphic/VertexArrayObject.h
./src/graphic/VertexArrayObject.cpp
./src/graphic/VertexBufferObject.h
./src/graphic/VertexBufferObject.cpp
./src/graphic/ElementBufferObject.h
./src/graphic/ElementBufferObject.cpp
./src/graphic/Texture.h
./src/graphic/Texture.cpp

./src/graphic/models/ModelMgr.h
./src/graphic/models/ModelMgr.cpp
./src/graphic/models/ModelBuilder.h
./src/graphic/models/ModelBuilder.cpp
./src/graphic/models/Model.h
./src/graphic/models/Model.cpp
./src/graphic/models/SimpleModel.h
./src/graphic/models/SimpleModel.cpp
)
target_link_libraries(graphic engine glfw3)

add_library(game #SHARED
./src/game/Game.h
./src/game/Game.cpp

./src/game/maps/Map.h
./src/game/maps/Map.cpp
./src/game/maps/LobbyMap.h
./src/game/maps/LobbyMap.cpp

./src/game/entities/Entity.h
./src/game/entities/Entity.cpp
./src/game/entities/TestEnt.h
./src/game/entities/TestEnt.cpp
)
target_link_libraries(game graphic)

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
target_link_libraries(main test game)

# -l -lGL -lX11 -lpthread -lXrandr -lXi -ldl

#newton raylib opengl32 gdi32 winmm
#set_target_properties(engine PROPERTIES LINK_FLAGS ./lib/resource.res)
