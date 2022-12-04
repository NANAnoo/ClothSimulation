cmake_minimum_required(VERSION 3.24)
project(ClothSimulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

file(GLOB cpp_files
        "src/*.cpp"
        )

add_executable(ClothSimulation ${cpp_files})

if (CMAKE_HOST_APPLE)

set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt@5/lib/cmake" "/usr/local/lib/cmake")
target_link_libraries(ClothSimulation "-framework OpenGL")
set_target_properties(ClothSimulation PROPERTIES LINK_FLAGS "-Wl,-F/Library/Frameworks")

endif()

find_package(OpenCV COMPONENTS
        opencv_core
        opencv_videoio
        REQUIRED)
find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        OpenGL
        REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(ClothSimulation
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::OpenGL
        )
target_link_libraries(ClothSimulation ${OpenCV_LIBS})
