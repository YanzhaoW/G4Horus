project(G4HorusGenerators)

add_subdirectory(cascade)

add_library(
    G4HorusGenerators
    PrimaryGeneratorAction.cc
    PrimaryGeneratorActionScattering.cc
    PrimaryGeneratorScattering.cc
    PrimaryGeneratorScattering.cc)

add_library(G4Horus::generators ALIAS G4HorusGenerators)
target_link_libraries(G4HorusGenerators PUBLIC G4Horus::cascade)
target_include_directories(G4HorusGenerators PUBLIC ${PROJECT_SOURCE_DIR})
