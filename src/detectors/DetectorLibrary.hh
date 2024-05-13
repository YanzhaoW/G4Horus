#pragma once

#include "Detector.hh"
#include "HPGe.hh"
#include "HPGeClover.hh"
#include "HPGeCoaxial.hh"
#include "HPGeHexagonal.hh"
#include "PIPS.hh"

class DetectorLibrary {
  public:
    DetectorLibrary() = default;

    static auto GetDetector(const std::string &det_id, const std::string &position) -> std::unique_ptr<Detector>;

    static const std::map<std::string, HPGe::_spec> specifications;
};
