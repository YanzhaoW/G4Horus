add_library(
    G4HorusGeneratorCascade
    # CascadeMessenger.cc
    CascadeGeneratorAction.cc
    DecayEngine.cc
    DecayJSONReader.cc
    DecayScheme.cc
    DecayTypes.cc)

set(JSON_ImplicitConversions OFF)
find_package(nlohmann_json REQUIRED)

target_link_libraries(
    G4HorusGeneratorCascade
    PUBLIC ${Geant4_LIBRARIES} fmt::fmt
    PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(G4HorusGeneratorCascade PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_library(G4Horus::cascade ALIAS G4HorusGeneratorCascade)
