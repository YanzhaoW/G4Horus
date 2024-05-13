#include "Detector.hh"
#include <utility>

Detector::Detector(std::string name, const G4double& length)
    : detector_LV_(nullptr)
    , fLength(length)
    , fWidth(0)
    , fName(std::move(name))
{
}
