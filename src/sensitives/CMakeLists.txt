add_library(Sensitives SHARED CloverSD.cc)
target_link_libraries(
    Sensitives
    PUBLIC G4Horus::definitions
    PRIVATE fmt::fmt ${Geant4_LIBRARIES})
target_include_directories(Sensitives PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(G4Horus::sensitives ALIAS Sensitives)
