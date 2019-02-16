cmake_minimum_required(VERSION 3.6)
project(cameracalib)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

find_package(OpenCV REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)

find_package(GLFW REQUIRED)
link_libraries(${GLFW_LIBRARIES})

link_directories(/usr/local/lib)

find_package(ASSIMP REQUIRED)
link_libraries(${ASSIMP_LIBRARIES})

set(SOURCE_FILES src/main.cpp src/calibrator.cpp src/camera.cpp include/camera.hpp src/camera.cpp include/nlohmann/json.hpp
        include/calibrator.hpp include/camera.hpp include/utils.hpp src/app.cpp include/app.hpp modules/glad.c src/glfw.cpp
        include/vertex_shader.hpp src/program.cpp src/model.cpp src/mesh.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/modules/)

add_executable(cameracalib ${SOURCE_FILES})

find_package(X11 REQUIRED)

target_link_libraries(cameracalib PUBLIC Boost::boost Boost::filesystem dl)
target_link_libraries(cameracalib PUBLIC dl m "${X11_Xrandr_LIB}" "${X11_Xxf86vm_LIB}" "${X11_Xcursor_LIB}" "${X11_Xinerama_LIB}" "${CMAKE_THREAD_LIBS_INIT}" rt Xi X11 GL GLU)

target_link_libraries(cameracalib PUBLIC ${OpenCV_LIBS})
target_compile_features(cameracalib PUBLIC cxx_std_17)
target_compile_definitions(cameracalib PUBLIC GLFW_INCLUDE_NONE=1)