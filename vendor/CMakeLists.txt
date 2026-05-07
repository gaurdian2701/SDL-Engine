add_library(vendor INTERFACE)

option(ENABLE_IMGUI "Enable Imgui Debugging" ON)

set(SDL_STATIC ON)
set(BUILD_SHARED_LIBS OFF)
set(SDLTTF_VENDORED ON)
set(SDLTTF_HARFBUZZ OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glm)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SDL)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SDL_TTF)

target_include_directories(
        vendor
            INTERFACE
                ${CMAKE_CURRENT_SOURCE_DIR}/glm)

target_link_libraries(
        vendor
            INTERFACE
                SDL3::SDL3-static
                SDL3_ttf::SDL3_ttf-static)

if(ENABLE_IMGUI)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/imgui)

    target_include_directories(
            vendor
            INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}/imgui
    )

    target_link_libraries(
            vendor
            INTERFACE
            imgui
    )

    target_compile_definitions(
            vendor
            INTERFACE
            ENABLE_IMGUI
    )
endif()

if(TRACY_ENABLE)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tracy)
    target_link_libraries(
            vendor
                INTERFACE
                    Tracy::TracyClient
    )
    target_compile_definitions(
            vendor
                INTERFACE
                    TRACY_ENABLE
    )
endif()
