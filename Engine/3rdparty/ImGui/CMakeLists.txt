file(GLOB_RECURSE SOURCE_FILES "*.cpp")
file(GLOB_RECURSE HEAD_FILES "*.h")

project(ImGui)

add_library(ImGui STATIC ${SOURCE_FILES})
set_target_properties(ImGui PROPERTIES FOLDER "ThirdParty")
target_compile_options(ImGui PRIVATE $<$<CONFIG:Debug>:/Zi /Od /DDEBUG>)
target_compile_options(ImGui PRIVATE $<$<CONFIG:Release>:/MP /O2 /DNDEBUG>)
