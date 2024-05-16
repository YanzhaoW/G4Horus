include(CTest)

find_package(GTest REQUIRED)

add_executable(
    G4HorusUnitTests
    DecayEngineTest.cc
    DecayIteratorTest.cc
    DecaySchemeTest.cc
    G4HorusUnitTests.cc
    JSONReaderTest.cc)

target_link_libraries(G4HorusUnitTests PRIVATE gtest::gtest G4Horus::cascade)

add_test(CloverTest ${CMAKE_BINARY_DIR}/G4Horus -t 4 -m ${CMAKE_SOURCE_DIR}/test/test.mac)
set_tests_properties(CloverTest PROPERTIES TIMEOUT "10000")

add_test(UnitTest ${CMAKE_CURRENT_BINARY_DIR}/G4HorusUnitTests)
set_tests_properties(UnitTest PROPERTIES TIMEOUT "10000")
