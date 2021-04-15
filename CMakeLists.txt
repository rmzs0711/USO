cmake_minimum_required(VERSION 3.17)
project(USO)

set(CMAKE_CXX_STANDARD 20)

add_executable(USO source/main.cpp include/base_logic.h source/base_logic.cpp include/map_management.h source/map_management.cpp include/maps.h source/aim_map_impl.cpp source/conveyor_map_impl.cpp source/bulletproof_map_impl.cpp include/map_objects.h source/map_objects.cpp source/maps.cpp source/map_constructor.cpp include/map_constructor.h)

set(SFML_DIR "include/SFML-2.5.1/lib/cmake/SFML")

find_package(SFML REQUIRED system window graphics network audio)
if (SFML_FOUND)
    include_directories(include)
    target_link_libraries(USO ${SFML_LIBRARIES})
    target_link_libraries(USO sfml-graphics sfml-audio)
    target_link_libraries(USO opengl32)
endif ()

