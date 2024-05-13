#pragma once

#include "G4LogicalVolume.hh"
// #include "G4SystemOfUnits.hh"
// #include "G4Transform3D.hh"
#include <string>

class Detector
{
  public:
    explicit Detector(std::string name, const G4double& length = 0.);
    virtual ~Detector() = default;
    Detector(const Detector&) = delete;
    Detector(Detector&&) = delete;
    auto operator=(const Detector&) -> Detector& = delete;
    auto operator=(Detector&&) -> Detector& = delete;

    [[nodiscard]] virtual auto GetLogical() const -> G4LogicalVolume* { return detector_LV_; }
    [[nodiscard]] virtual auto GetLength() const -> G4double { return fLength; }
    [[nodiscard]] virtual auto GetOffset() const -> G4double { return fLength / 2; }
    [[nodiscard]] virtual auto GetWidth() const -> G4double { return fWidth; }
    [[nodiscard]] virtual auto GetName() const -> std::string { return fName; }

  protected:
    void SetLogical(G4LogicalVolume* volume) { detector_LV_ = volume; }
    void SetLength(double length) { fLength = length; }
    void SetWidth(double width) { fWidth = width; }
    void SetName(std::string_view name) { fName = name; }

  private:
    G4LogicalVolume* detector_LV_;
    double fLength;
    double fWidth;
    std::string fName;
};
