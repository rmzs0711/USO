cmake_minimum_required(VERSION 3.17)
project(constructor)
project(USO)

set(CMAKE_CXX_STANDARD 20)

add_executable(USO
        include/base_logic.h
        source/base_logic.cpp
        source/menu.cpp
        include/menu.h
        include/main.h
        source/main.cpp
        source/menu_objects.cpp
        include/menu_objects.h
        include/map_objects_management.h
        source/map_objects_management.cpp
        include/maps.h
        source/taiko_map_run_impl.cpp
        source/aim_map_run_impl.cpp
        include/map_objects.h
        source/map_objects.cpp
        source/maps.cpp
        source/map_constructor_impl.cpp
        source/conveyor_map_run_impl.cpp
        source/loading_menu.cpp
        include/loading_menu.h)

add_executable(constructor
        include/base_logic.h
        source/base_logic.cpp
        source/menu.cpp
        include/menu.h
        include/main.h
        source/main.cpp
        source/menu_objects.cpp
        include/menu_objects.h
        include/map_objects_management.h
        source/map_objects_management.cpp
        include/maps.h
        source/taiko_map_run_impl.cpp
        source/aim_map_run_impl.cpp
        include/map_objects.h
        source/map_objects.cpp
        source/maps.cpp
        source/map_constructor_impl.cpp
        source/conveyor_map_run_impl.cpp
        source/loading_menu.cpp
        include/loading_menu.h)

set(SFML_DIR "include/SFML-2.5.1/lib/cmake/SFML")

find_package(SFML REQUIRED system window graphics network audio)
if (SFML_FOUND)
    include_directories(include)
    target_link_libraries(USO ${SFML_LIBRARIES})
    target_link_libraries(USO sfml-graphics sfml-audio)
    target_link_libraries(USO opengl32)

    target_link_libraries(constructor ${SFML_LIBRARIES})
    target_link_libraries(constructor sfml-graphics sfml-audio)
    target_link_libraries(constructor opengl32)
endif ()