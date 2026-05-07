cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 23)
set(PROJECT_NAME SDLEngine)

project(${PROJECT_NAME})

add_subdirectory(vendor)

set(ENGINE_BINARY_NAME SDLEngineLib)
set(VENDOR_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/include)
set(BUILD_SHARED_LIBS OFF)
option(ENABLE_ASAN "Enable Windows Address Sanitizer" OFF)

function(EnableASANCompileOptions targetProject)
    if(${CMAKE_GENERATOR} MATCHES "Visual Studio" AND ENABLE_ASAN)
        target_compile_options(
                ${targetProject}
                    PRIVATE
                        $<$<CONFIG:Debug>:/fsanitize=address>)
    endif()
endfunction()


add_library(
        ${ENGINE_BINARY_NAME}
            STATIC
                ${CMAKE_CURRENT_SOURCE_DIR}/src/Application/Application.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/GameScene.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/ECS/ECSManager.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/ECS/Systems/RenderingSystem.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/CoreSystems/ResourceManager.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/CoreSystems/InputSystem.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/ECS/Systems/ParticleSystem.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/ECS/Systems/PhysicsSystem.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/Scene/SceneManager.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/ECS/Systems/DebugDrawSystem.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Editor.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Physics/NarrowPhase.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Physics/Solver.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Physics/NaiveBroadPhase.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/DataStructures/AABBQuadTree.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Physics/AABBTreeBroadPhase.cpp
        include/Core/Debug/TracyMalloc.h
)

EnableASANCompileOptions(${ENGINE_BINARY_NAME})

target_include_directories(
        ${ENGINE_BINARY_NAME}
            PUBLIC
                ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(
        ${ENGINE_BINARY_NAME}
            PUBLIC
                vendor)

set_target_properties(
        ${ENGINE_BINARY_NAME}
            PROPERTIES
                LINKER_LANGUAGE CXX)
