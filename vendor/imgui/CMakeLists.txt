cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 23)

add_library(imgui STATIC
        imgui.cpp
        imgui_draw.cpp
        imgui_impl_sdl3.cpp
        imgui_impl_sdlrenderer3.cpp
        imgui_tables.cpp
        imgui_widgets.cpp
)

target_link_libraries(
        imgui
            PUBLIC
                SDL3::SDL3-static
)