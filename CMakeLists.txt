cmake_minimum_required(VERSION 3.12)
project(TarefasCG)


set(TAREFA_1 tarefa-1)
set(TAREFA_2 tarefa-2)
set(ATIVIDADE_VIVENCIAL_1 tarefa-atividade-vivencial-1)


# get_filename_component(PARENT_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)
set(LIBS "${CMAKE_SOURCE_DIR}/dependencies/libs")
set(INCLUDE "${CMAKE_SOURCE_DIR}/dependencies/include")

# set(COMMON "${CMAKE_SOURCE_DIR}/common")

# add_subdirectory(${COMMON})

# Find OpenGL package
find_package(OpenGL REQUIRED)

# Set source files for your application
set(SOURCES
    ${ATIVIDADE_VIVENCIAL_1}/main.cpp
    glad.c
)

# Add executable target
add_executable(TarefasCG ${SOURCES})

# Link OpenGL libraries
# target_link_libraries(TarefasCG PRIVATE OpenGL::GL ${COMMON})

# Platform-specific configurations
if(WIN32)
    # Windows-specific configuration
    # You may need to adjust these paths to match your setup
    target_link_libraries(TarefasCG PRIVATE 
        ${LIBS}/win32/glew32.lib
        ${LIBS}/win32/glfw3.lib
    )
    target_include_directories(TarefasCG PRIVATE ${INCLUDE}/win32)
    
    # Copy DLL files
    add_custom_command(TARGET TarefasCG POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${LIBS}/win32/glew32.dll
            $<TARGET_FILE_DIR:TarefasCG>
    )
    
elseif(UNIX AND NOT APPLE)
    # Linux-specific configuration
    # You may need to adjust these paths to match your setup
    target_link_libraries(TarefasCG PRIVATE
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
    target_include_directories(TarefasCG PRIVATE ${INCLUDE}/linux)
    
elseif(APPLE)
    # macOS-specific configuration
    # You may need to adjust these paths to match your setup
    target_link_libraries(TarefasCG PRIVATE
        ${LIBS}/macos/libbox2d.a
        ${LIBS}/macos/libglfw.3.3.dylib
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework CoreFoundation"
    )
    target_include_directories(TarefasCG PRIVATE ${INCLUDE}/macos)
endif()
