# CMakeLists.txt in the common folder

# Add all the common source files to a variable
set(COMMON_SOURCES
    src/mesh.cpp
    # Add more source files if needed
)

# Create a library from the common sources
add_library(common ${COMMON_SOURCES})

# Include the common/include folder as a public include directory
target_include_directories(common PUBLIC include)
