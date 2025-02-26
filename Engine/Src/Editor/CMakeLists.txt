# Define Variables
file(GLOB_RECURSE SOURCE_FILES "*.cpp")
file(GLOB_RECURSE HEAD_FILES "*.h")


# Project Configuration
project(PhotonEditor)

# file group 
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCE_FILES} ${HEAD_FILES})

# Build config
add_executable(PhotonEditor ${SOURCE_FILES} ${HEAD_FILES})

set_target_properties(PhotonEditor PROPERTIES FOLDER "Engine")
target_link_libraries(PhotonEditor PRIVATE PhotonRuntime)
target_compile_options(PhotonEditor PRIVATE $<$<CONFIG:Debug>:/Zi /Od /DDEBUG>)
target_compile_options(PhotonEditor PRIVATE $<$<CONFIG:Release>:/MP /O2 /DNDEBUG>)

# startup project
message("StartUp Project: ${CMAKE_SOURCE_DIR}/${ProjectName}")

# Clear Variable
# unset(EDITOR_SOURCE)
# unset(EDITOR_HEAD)