## set target project
file(GLOB target_src "*.h" "*.cpp") # look for source files
file(GLOB target_shaders "shaders/*.vert" "shaders/*.frag") # look for shaders
add_executable(${subdir} ${target_src} ${target_shaders})

# list of libraries
set(libraries glad glfw imgui assimp)

if(APPLE)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(OPENGL_LIBRARY OpenGL)
    find_library(COREVIDEO_LIBRARY CoreVideo)

    list(APPEND libraries
            ${OPENGL_LIBRARY}
            ${COCOA_LIBRARY}
            ${IOKIT_LIBRARY}
            ${COREVIDEO_LIBRARY}
            )
endif()

## set link libraries
target_link_libraries(${subdir} ${libraries})

## add local source directory to include paths
target_include_directories(${subdir} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

## copy shaders folder to build folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

## copy models and textures
file(COPY ${CMAKE_SOURCE_DIR}/common/models/car DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/common/models/floor DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/common/models/skybox DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

## copy again at the time the current target gets compiled
add_custom_command(
        TARGET ${subdir} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders ${CMAKE_CURRENT_BINARY_DIR}/shaders
        COMMENT "Copying shaders" VERBATIM
)