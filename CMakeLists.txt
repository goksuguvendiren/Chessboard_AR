cmake_minimum_required(VERSION 3.6)
project(cameracalib)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

find_package(OpenCV REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)

find_package(GLFW REQUIRED)
link_libraries(${GLFW_LIBRARIES})

set(SOURCE_FILES src/main.cpp src/calibrator.cpp src/camera.cpp include/camera.hpp src/camera.cpp include/nlohmann/json.hpp
        include/calibrator.hpp include/camera.hpp include/utils.hpp src/app.cpp include/app.hpp modules/glad.c src/glfw.cpp
        include/vertex_shader.hpp src/program.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/modules/)

add_executable(cameracalib ${SOURCE_FILES})

target_link_libraries(cameracalib PUBLIC Boost::boost Boost::filesystem)

target_link_libraries(cameracalib PUBLIC ${OpenCV_LIBS})
target_compile_features(cameracalib PUBLIC cxx_std_17)
target_compile_definitions(cameracalib PUBLIC GLFW_INCLUDE_NONE=1)