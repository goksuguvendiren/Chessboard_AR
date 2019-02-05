cmake_minimum_required(VERSION 3.6)
project(cameracalib)

find_package(OpenCV REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)

set(SOURCE_FILES src/main.cpp src/calibrator.cpp src/camera.cpp include/camera.hpp src/camera.cpp include/nlohmann/json.hpp include/calibrator.hpp include/camera.hpp include/utils.hpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)

add_executable(cameracalib ${SOURCE_FILES})

target_link_libraries(cameracalib PUBLIC Boost::boost Boost::filesystem)

target_link_libraries(cameracalib PUBLIC ${OpenCV_LIBS})
target_compile_features(cameracalib PUBLIC cxx_std_17)
