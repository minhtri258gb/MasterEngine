cmake_minimum_required(VERSION 3.0.0)
project(MasterEngine VERSION 0.1.0)

include_directories(./src)
include_directories(./inc)
link_directories(./lib)
link_directories(./bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/../lib)

add_library(engine #SHARED
./src/engine/library.cpp
./src/engine/Config.h
./src/engine/Config.cpp
./src/engine/enumKey.h
./src/engine/Input.h
./src/engine/Input.cpp
./src/engine/Timer.h
./src/engine/Timer.cpp
#./src/engine/Cryptography.h
#./src/engine/Cryptography.cpp
./src/engine/datatype/Flag32.h
./src/engine/datatype/Flag32.cpp
./src/engine/datatype/3D/Math3D.h
./src/engine/datatype/3D/Vector2.h
./src/engine/datatype/3D/Vector2.cpp
./src/engine/datatype/3D/Vector2i.h
./src/engine/datatype/3D/Vector2i.cpp
./src/engine/datatype/3D/Vector3.h
./src/engine/datatype/3D/Vector3.cpp
./src/engine/datatype/3D/Vector3i.h
./src/engine/datatype/3D/Vector3i.cpp
./src/engine/datatype/3D/Vector4.h
./src/engine/datatype/3D/Vector4.cpp
./src/engine/datatype/3D/Vector4i.h
./src/engine/datatype/3D/Vector4i.cpp
./src/engine/datatype/3D/Matrix3x3.h
./src/engine/datatype/3D/Matrix3x3.cpp
./src/engine/datatype/3D/Matrix4x4.h
./src/engine/datatype/3D/Matrix4x4.cpp
./src/engine/datatype/3D/Quaternion.h
./src/engine/datatype/3D/Quaternion.cpp
./src/engine/exception/Exception.h
./src/engine/exception/Exception.cpp
./src/engine/exception/BoundingException.h
./src/engine/exception/BoundingException.cpp
./src/engine/exception/LoadException.h
./src/engine/exception/LoadException.cpp
./src/engine/exception/MathException.h
./src/engine/exception/MathException.cpp
./src/engine/load/TextFile.h
./src/engine/load/TextFile.cpp
./src/engine/load/FileIni.h
./src/engine/load/FileIni.cpp
./src/engine/math/Math.h
./src/engine/math/Math.cpp
./src/engine/utils/StringUtils.h
./src/engine/utils/StringUtils.cpp
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
./src/graphic/ShaderProgramMgr.h
./src/graphic/ShaderProgramMgr.cpp
./src/graphic/VertexArrayObject.h
./src/graphic/VertexArrayObject.cpp
./src/graphic/VertexBufferObject.h
./src/graphic/VertexBufferObject.cpp
./src/graphic/ElementBufferObject.h
./src/graphic/ElementBufferObject.cpp
./src/graphic/Texture.h
./src/graphic/Texture.cpp
# ./src/graphic/FrustumCulling.h
# ./src/graphic/FrustumCulling.cpp

./src/graphic/models/ModelMgr.h
./src/graphic/models/ModelMgr.cpp
./src/graphic/models/ModelBuilder.h
./src/graphic/models/ModelBuilder.cpp
./src/graphic/models/Model.h
./src/graphic/models/Model.cpp
./src/graphic/models/SimpleModel.h
./src/graphic/models/SimpleModel.cpp
./src/graphic/models/animation/AnimationModel.h
./src/graphic/models/animation/AnimationModel.cpp

# ./src/graphic/particles/Sprite.h
# ./src/graphic/particles/Sprite.cpp

)
target_link_libraries(graphic engine assimp glfw3)

add_library(game #SHARED
./src/game/Game.h
./src/game/Game.cpp
./src/game/Command.h
./src/game/Command.cpp

./src/game/maps/Map.h
./src/game/maps/Map.cpp
./src/game/maps/LobbyMap.h
./src/game/maps/LobbyMap.cpp

./src/game/entities/Entity.h
./src/game/entities/Entity.cpp
./src/game/entities/TestEnt.h
./src/game/entities/TestEnt.cpp
./src/game/entities/TestAnimEnt.h
./src/game/entities/TestAnimEnt.cpp
# ./src/game/entities/TestParticleEnt.h
# ./src/game/entities/TestParticleEnt.cpp
./src/game/entities/GroundEnt.h
./src/game/entities/GroundEnt.cpp
)
target_link_libraries(game graphic)

add_library(mod #SHARED
./src/mod/PluginBase.h
./src/mod/Plugin.h
./src/mod/Plugin.cpp
./src/mod/PluginSystem.h
./src/mod/PluginSystem.cpp
)

add_library(test #SHARED
./src/test/Test.h
./src/test/Test.cpp

# ./src/test/file/TestJson.h
# ./src/test/file/TestJson.cpp
# ./src/test/file/FileJson.h
# ./src/test/file/FileJson.cpp

./src/test/audio/Audio.h
./src/test/audio/Audio.cpp
./src/test/audio/AudioMgr.h
./src/test/audio/AudioMgr.cpp
./src/test/audio/Sound.h
./src/test/audio/Sound.cpp

./src/test/technique/TestTechnique.h
./src/test/technique/TestTechnique.cpp

./src/test/thread/Thread.h
./src/test/thread/Thread.cpp

)
target_link_libraries(test mod engine graphic bass)

add_executable(main
./src/main.cpp
)
target_link_libraries(main test game)


# add_library(plugin1 SHARED ./src/mod/Plugin1.cpp)
# target_link_libraries(plugin1 game engine graphic)
# set_target_properties(plugin1 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/plugins)
# set_target_properties(plugin1 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/plugins)

# add_library(plugin2 SHARED ./src/mod/Plugin2.cpp)
# target_link_libraries(plugin2 game engine graphic)
# set_target_properties(plugin2 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/plugins)
# set_target_properties(plugin2 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/plugins)

# add_library(plugin3 SHARED ./src/mod/Plugin3.cpp)
# target_link_libraries(plugin3 game engine graphic)
# set_target_properties(plugin3 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/plugins)
# set_target_properties(plugin3 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/plugins)


# -l -lGL -lX11 -lpthread -lXrandr -lXi -ldl

#newton raylib opengl32 gdi32 winmm
#set_target_properties(engine PROPERTIES LINK_FLAGS ./lib/resource.res)
