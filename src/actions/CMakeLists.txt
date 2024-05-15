project(G4HorusActions)

add_subdirectory(generators)

add_library(
    ${PROJECT_NAME}
    ActionInitialization.cc
    EventActionHistogram.cc
    EventActionNtuple.cc
    EventActionSoco.cc
    PhysicsList.cc
    RunActionHistogram.cc
    RunActionNtuple.cc
    RunActionSoco.cc)

add_library(G4Horus::actions ALIAS ${PROJECT_NAME})
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE G4Horus::soco G4Horus::definitions
    PUBLIC G4Horus::generators G4Horus::sensitives)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
