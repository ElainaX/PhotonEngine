# Photon Engine Root
set(ENGINE_ROOT_DIR "${PHOTON_ROOT_DIR}/Engine")
set(THIRD_PARTY_DIR "${ENGINE_ROOT_DIR}/3rdParty")
set(ENGINE_ASSET_DIR "/Asset")



set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Reset the configuration types" FORCE)

add_subdirectory(Src/Runtime)
add_subdirectory(Src/Editor)
add_subdirectory(3rdParty/spdlog)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "PhotonEditor")

message("---------${CMAKE_CURRENT_SOURCE_DIR}")
message("---------${CMAKE_CURRENT_DIR}")