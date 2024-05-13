#include "Setup.hh"
#include "G4PhysicalConstants.hh"
#include <utility>

Setup::Setup(G4LogicalVolume* theMother, std::map<std::string, coordinate> positions)
    : fPositions(std::move(positions))
    , fMotherLV(theMother)
    , fDetDB(DetectorLibrary())
{
}

void Setup::Place(G4LogicalVolume* logicalVolume,
                  const std::string& name,
                  const std::string& position,
                  const G4double distance)
{
    const auto transform = GetTransform(CoordinateForPosition(position), distance);
    new G4PVPlacement(transform, logicalVolume, name, fMotherLV, false, 0, true);
}

void Setup::PlaceDetector(const std::string& det_id,
                          const std::string& position,
                          G4double distance,
                          const std::vector<Filter::_filter>& filters)
{
    // Place HPGe
    auto const* thedet = DetectorLibrary::GetDetector(det_id, position).release();

    for (const auto& filter_spec : filters)
    {
        const auto filter = Filter{ filter_spec, thedet->GetWidth() };
        Place(
            filter.GetLogical(), position + "_filter_" + filter_spec.material, position, distance + filter.GetOffset());
        distance += filter_spec.length;
    }

    Place(thedet->GetLogical(), position, position, distance + thedet->GetOffset());
}

void Setup::PlaceDetector(const BGO::_type& bgo,
                          const std::string& position,
                          G4double distance,
                          const std::vector<Filter::_filter>& filters)
{
    // Place BGO
    auto const* thebgo = new BGO(bgo, position);

    for (const auto& filter_spec : filters)
    {
        const auto filter = Filter{ filter_spec, thebgo->GetWidth() };
        Place(filter.GetLogical(),
              position + "_BGO_filter_" + filter_spec.material,
              position,
              distance + filter.GetOffset());
        distance += filter_spec.length;
    }

    Place(thebgo->GetLogical(), position, position, distance + thebgo->GetOffset());
}

auto Setup::CoordinateForPosition(const std::string& position) const -> coordinate
{
    auto _pos = fPositions.find(position);
    if (_pos == fPositions.end())
    {
        G4Exception("Setup::PlaceDetector",
                    "Unknown position",
                    FatalException,
                    ("Position " + position + " is not a valid position").c_str());
    }
    return _pos->second;
}

auto Setup::GetTransform(const coordinate& pos, const G4double& distance) const -> G4Transform3D
{
    auto moveto = G4ThreeVector{};
    moveto.setRThetaPhi(distance, pos.theta, pos.phi);

    const auto rotation_axis = G4ThreeVector(0, 0, -1).cross(moveto).unit();
    const auto rotation_angle = acos(G4ThreeVector(0, 0, -1).dot(moveto) / moveto.mag()) + pi;
    const auto rotation_matrix = G4RotationMatrix{ rotation_axis, rotation_angle };

    return G4Transform3D{ rotation_matrix, moveto };
}
