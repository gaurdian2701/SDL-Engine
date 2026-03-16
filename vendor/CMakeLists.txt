add_library(vendor INTERFACE)

set(SDL_STATIC ON)
set(BUILD_SHARED_LIBS OFF)
set(SDLTTF_VENDORED ON)
set(SDLTTF_HARFBUZZ OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glm)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SDL)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SDL_TTF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/imgui)

target_include_directories(
        vendor
            INTERFACE
                ${CMAKE_CURRENT_SOURCE_DIR}/glm
                ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

target_link_libraries(
        vendor
            INTERFACE
                SDL3::SDL3-static
                SDL3_ttf::SDL3_ttf-static
                imgui)