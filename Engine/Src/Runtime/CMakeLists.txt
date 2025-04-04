﻿# Define Variables
file(GLOB_RECURSE SOURCE_FILES "*.cpp")
file(GLOB_RECURSE HEAD_FILES "*.h")
file(GLOB_RECURSE HLSL_FILES "*.hlsl")

message(STATUS "SOURCE_FILES: ${SOURCE_FILES}")
message(STATUS "HEAD_FILES: ${HEAD_FILES}")

set(vProjectFolder "Engine")
set(DebugOutputPostfix "_d")


# Project Configuration
project(PhotonRuntime)


# file group
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCE_FILES} ${HEAD_FILES} ${HLSL_FILES})

# build
add_library(PhotonRuntime STATIC ${SOURCE_FILES} ${HEAD_FILES} ${HLSL_FILES})
target_link_libraries(PhotonRuntime PUBLIC spdlog)
target_link_libraries(PhotonRuntime PUBLIC ImGui)
target_link_libraries(PhotonRuntime PUBLIC d3d12.lib)
target_link_libraries(PhotonRuntime PUBLIC d3dcompiler.lib)
target_link_libraries(PhotonRuntime PUBLIC dxgi.lib)
target_link_libraries(PhotonRuntime PUBLIC $<$<CONFIG:Debug>:${PHOTON_ROOT_DIR}/Engine/3rdparty/assimp/assimp-vc143-mtd.lib>)
target_link_libraries(PhotonRuntime PUBLIC $<$<CONFIG:Release>:${PHOTON_ROOT_DIR}/Engine/3rdparty/assimp/assimp-vc143-mt.lib>)

target_include_directories(PhotonRuntime PRIVATE ${CMAKE_SOURCE_DIR}/3rdParty/include)
target_include_directories(PhotonRuntime PUBLIC "./")
    
target_compile_options(PhotonRuntime PRIVATE $<$<CONFIG:Debug>:/Zi /Od /DDEBUG>)
target_compile_options(PhotonRuntime PRIVATE $<$<CONFIG:Release>:/MP /O2 /DNDEBUG>)

set_target_properties(PhotonRuntime PROPERTIES FOLDER "${vProjectFolder}")
set_target_properties(PhotonRuntime PROPERTIES
    OUTPUT_NAME "PhotonRuntime"              # 基本名称
    DEBUG_POSTFIX "${DebugOutputPostfix}"          # Debug 配置下的后缀为 "_d"
)
set_source_files_properties(${HLSL_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
if(MSVC)
    target_compile_options(PhotonRuntime PUBLIC "/Zc:preprocessor")
    target_compile_options(PhotonRuntime PUBLIC /fp:fast)
endif()
# Clear Variable
# unset(SOURCE_FILES)
# unset(HEAD_FILES)