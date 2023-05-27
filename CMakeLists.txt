cmake_minimum_required(VERSION 3.12)
project(Tarefa1)

# Find OpenGL package
find_package(OpenGL REQUIRED)

# Set source files for your application
set(SOURCES
    src/main.cpp
    glad.c
)

# Add executable target
add_executable(Tarefa1 ${SOURCES})

# Link OpenGL libraries
target_link_libraries(Tarefa1 PRIVATE OpenGL::GL)

# Platform-specific configurations
if(WIN32)
    # Windows-specific configuration
    # You may need to adjust these paths to match your setup
    target_link_libraries(Tarefa1 PRIVATE 
        ${CMAKE_SOURCE_DIR}/dependecies/libs/win32/glew32.lib
        ${CMAKE_SOURCE_DIR}/dependecies/libs/win32/glfw3.lib
    )
    target_include_directories(Tarefa1 PRIVATE ${CMAKE_SOURCE_DIR}/dependecies/include/win32)
    
    # Copy DLL files
    add_custom_command(TARGET Tarefa1 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/dependecies/libs/win32/glew32.dll
            $<TARGET_FILE_DIR:Tarefa1>
    )
    
elseif(UNIX AND NOT APPLE)
    # Linux-specific configuration
    # You may need to adjust these paths to match your setup
    target_link_libraries(Tarefa1 PRIVATE
        ${CMAKE_SOURCE_DIR}/dependecies/libs/linux/libGLEW.a
        ${CMAKE_SOURCE_DIR}/dependecies/libs/linux/libglfw3.a
        dl
        pthread
        X11
        Xrandr
        Xi
        Xxf86vm
        Xinerama
        Xcursor
    )
    target_include_directories(Tarefa1 PRIVATE ${CMAKE_SOURCE_DIR}/dependecies/include/linux)
    
elseif(APPLE)
    # macOS-specific configuration
    # You may need to adjust these paths to match your setup
    target_link_libraries(Tarefa1 PRIVATE
        ${CMAKE_SOURCE_DIR}/dependencies/libs/macos/libbox2d.a
        ${CMAKE_SOURCE_DIR}/dependencies/libs/macos/libglfw.3.3.dylib
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework CoreFoundation"
    )
    target_include_directories(Tarefa1 PRIVATE ${CMAKE_SOURCE_DIR}/dependencies/include/macos)
endif()
