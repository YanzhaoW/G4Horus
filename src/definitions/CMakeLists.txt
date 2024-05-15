add_library(Definitions INTERFACE)
target_include_directories(Definitions INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
add_library(G4Horus::definitions ALIAS Definitions)
