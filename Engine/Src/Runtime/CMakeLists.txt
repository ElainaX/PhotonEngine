# Define Variables
file(GLOB_RECURSE SOURCE_FILES "*.cpp")
file(GLOB_RECURSE HEAD_FILES "*.h")

message(STATUS "SOURCE_FILES: ${SOURCE_FILES}")
message(STATUS "HEAD_FILES: ${HEAD_FILES}")

set(vProjectFolder "Engine")
set(DebugOutputPostfix "_d")


# Project Configuration
project(PhotonRuntime)


# file group
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCE_FILES} ${HEAD_FILES})


# build
add_library(PhotonRuntime STATIC ${SOURCE_FILES} ${HEAD_FILES})
target_link_libraries(PhotonRuntime PUBLIC spdlog)
target_include_directories(PhotonRuntime PRIVATE ${CMAKE_SOURCE_DIR}/3rdParty/include)
target_include_directories(PhotonRuntime PUBLIC "./")
    
target_compile_options(PhotonRuntime PRIVATE $<$<CONFIG:Debug>:/Zi /Od /DDEBUG>)
target_compile_options(PhotonRuntime PRIVATE $<$<CONFIG:Release>:/MP /O2 /DNDEBUG>)

set_target_properties(PhotonRuntime PROPERTIES FOLDER "${vProjectFolder}")
set_target_properties(PhotonRuntime PROPERTIES
    OUTPUT_NAME "PhotonRuntime"              # 基本名称
    DEBUG_POSTFIX "${DebugOutputPostfix}"          # Debug 配置下的后缀为 "_d"
)


# Clear Variable
# unset(SOURCE_FILES)
# unset(HEAD_FILES)