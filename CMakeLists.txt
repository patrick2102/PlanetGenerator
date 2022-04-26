# ---------------------------------------------------------------------------------
# Projects Settings
# ---------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.3)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


MACRO(SUBDIRLIST result curdir)
    FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
    SET(dirlist "")
    FOREACH(child ${children})
        IF(IS_DIRECTORY ${curdir}/${child})
            LIST(APPEND dirlist ${child})
        ENDIF()
    ENDFOREACH()
    SET(${result} ${dirlist})
ENDMACRO()


set(EXTERNAL_LIBRARIES_SOURCE_PATH ${CMAKE_SOURCE_DIR}/common/third-party)

# ---------------------------------------------------------------------------------
# Cmake projects
# ---------------------------------------------------------------------------------
project(ITU-graphics-programming)

set(FBX_SUPPORT OFF)

# static libraries
add_subdirectory(${EXTERNAL_LIBRARIES_SOURCE_PATH}/glfw)
add_subdirectory(${EXTERNAL_LIBRARIES_SOURCE_PATH}/glad)
add_subdirectory(${EXTERNAL_LIBRARIES_SOURCE_PATH}/imgui)
add_subdirectory(${EXTERNAL_LIBRARIES_SOURCE_PATH}/assimp)




include_directories(
        include
        ${EXTERNAL_LIBRARIES_SOURCE_PATH}/glad/include
        ${EXTERNAL_LIBRARIES_SOURCE_PATH}/glfw//include
        ${EXTERNAL_LIBRARIES_SOURCE_PATH}/imgui/include
        ${EXTERNAL_LIBRARIES_SOURCE_PATH}/glm
        ${EXTERNAL_LIBRARIES_SOURCE_PATH}/STBImage
        ${EXTERNAL_LIBRARIES_SOURCE_PATH}/assimp/include
        ${EXTERNAL_LIBRARIES_SOURCE_PATH}/assimp
        )

## add the actual projects to build
IF(EXISTS ${CMAKE_SOURCE_DIR}/student_projects)
    add_subdirectory(${CMAKE_SOURCE_DIR}/student_projects)
ENDIF()

IF(EXISTS ${CMAKE_SOURCE_DIR}/assignments)
    add_subdirectory(${CMAKE_SOURCE_DIR}/assignments)
ENDIF()
