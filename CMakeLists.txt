cmake_minimum_required(VERSION 3.12)
project(TarefasCG)

# get_filename_component(PARENT_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)
set(LIBS "${CMAKE_SOURCE_DIR}/dependencies/libs")
set(INCLUDE "${CMAKE_SOURCE_DIR}/dependencies/include")

# set(COMMON "${CMAKE_SOURCE_DIR}/common")

# add_subdirectory(${COMMON})

# Find OpenGL package
find_package(OpenGL REQUIRED)

# Link OpenGL libraries
# target_link_libraries(TarefasCG PRIVATE OpenGL::GL ${COMMON})

# Platform-specific configurations
if(WIN32)
    # Windows-specific configuration
    # You may need to adjust these paths to match your setup
    link_libraries(
        ${LIBS}/win32/glew32.lib
        ${LIBS}/win32/glfw3.lib
    )
    include_directories(${INCLUDE}/win32)
    
    # Copy DLL files
    # add_custom_command(TARGET TarefasCG POST_BUILD
    #     COMMAND ${CMAKE_COMMAND} -E copy_if_different
    #         ${LIBS}/win32/glew32.dll
    #         $<TARGET_FILE_DIR:TarefasCG>
    # )
    
elseif(UNIX AND NOT APPLE)
    # Linux-specific configuration
    # You may need to adjust these paths to match your setup
    link_libraries(
        ${LIBS}/linux/libGLEW.a
        ${LIBS}/linux/libglfw3.a
        dl
        pthread
        X11
        Xrandr
        Xi
        Xxf86vm
        Xinerama
        Xcursor
    )
    include_directories(${INCLUDE}/linux)
    
elseif(APPLE)
    # macOS-specific configuration
    # You may need to adjust these paths to match your setup
    link_libraries(
        ${LIBS}/macos/libbox2d.a
        ${LIBS}/macos/libglfw.3.3.dylib
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework CoreFoundation"
    )
    include_directories(${INCLUDE}/macos)
endif()

add_library(CommonLib STATIC 
    common/mesh.cpp
)

# Add executable target
add_executable(Tarefa1 
    tarefa-1/main.cpp
    glad.c
)

add_executable(Tarefa2 
    tarefa-2/main.cpp
    glad.c
)

add_executable(AtividadeVivencial1 
    tarefa-atividade-vivencial-1/main.cpp
    glad.c
)

add_executable(Tarefa3
    tarefa-3/main.cpp
    glad.c
    stb_image.cpp
    shader.cpp
)

add_executable(Tarefa4
    tarefa-4/main.cpp
    glad.c
    stb_image.cpp
    shader.cpp
)

add_executable(Tarefa5
    tarefa-5/main.cpp
    glad.c
    stb_image.cpp
    shader.cpp
    camera.cpp
)

target_link_libraries(Tarefa5 CommonLib)
