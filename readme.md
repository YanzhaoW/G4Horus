# G4Horus

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3692474.svg)](https://doi.org/10.5281/zenodo.3692474)
[![Build Status](https://travis-ci.org/janmayer/G4Horus.svg?branch=master)](https://travis-ci.org/janmayer/G4Horus)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/aed36e462b55424f8903bbc12afee877)](https://app.codacy.com/gh/YanzhaoW/G4Horus/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)

An implementation of the HORUS High-Purity Germanium (HPGe) γ-ray spectrometer and associated equipment in Geant4.

<!-- ![G4Horus Default Geometry](doc/Clover.png) -->
<p align="center">
<img src="doc/Clover.png" width = 400>
</p>

## Overview

The main goal of this project is to provide an accurate, easy-to-use efficiency simulation for the HORUS setup at the Institute for Nuclear Physics, University of Cologne.

Components:

- Geometries
    - Target Chambers
        - Astro Target Chamber
        - Astro Target Chamber V2
        - SONIC
        - SONIC V3
        - SONIC V3 ΔEE
    - Setups (HPGe Arrays with positions for HPGe Detectors)
        - HORUS (14)
        - Cologne Clover Counting Setup (2)
    - Detectors
        - High-Purity Germanium (HPGe)*
            - Coaxial (default)
            - Clover
            - Encapsulated Hexagonal
        - Bismuth Germanate (BGO) anti-Compton shields
        - (WIP) Passivated Implanted Planar Silicon (PIPS)
    - Auxiliary equipment
        - γ-Filter-Disks
        - (NYI) Passive γ-Supershields
- Actions
    - Event Generators
        - Particle Gun
        - (WIP) Scattering Gun
        - (NYI) Coincidence Gun
        - Cascade Gun
    - Output Formats
        - ROOT Histograms
        - ROOT Ntuples (not recommended)
        - SOCOv2 Events
    - Evaluation
        - Visualization Mode
        - Batch processing Mode
        - Automated efficiency evaluation

\*) Note that each HPGe detector has unique properties, stored in the `DetectorLibrary`, and is referenced by its manufactured ID.

## Getting Started

### Dependencies

- CMake version >= 3.28.1
- [conan](https://docs.conan.io/2/installation.html) version >= 2.0.1
- gcc version >= 13
- [Geant4](https://github.com/Geant4/geant4) version >= 11.2

### Installation

```sh
git clone https://github.com/YanzhaoW/G4Horus.git
cd G4Horus
git submodule init && git submodule update
cmake --workflow --preset default
```

The project should compile successfully.

### Quick Simulation

```shell
cd build
./G4Horus -m ../scripts/batch_default.mac
```

## Usage

All user configurations of this program should be specified in a Geant4 macro file. The executable located in the build folder has two modes: visual mode and batch mode.

### Visual mode

Visual mode provides a graphical overview of detector geometries (Clover detector by default). It does not generate any simulated data.
```shell
./G4Horus -v
```

### Batch mode

Batch mode provides simulated data according to the user configurations defined in a macro file:
```shell
./G4Hours -m [macro_file_path.mac]
```

## Configurations in macro files

Parameters like detector distances or particle energies are set by different commands in a macro file. Some (pre-init) commands can only be used before the initialization command (`/g4horus/init`), such as `output_type` or `distance`. An example of available commands can be found in the file [batch_default.mac](../scripts/batch_default.mac).

### Commands in the macro file

| commands       | required | pre-init | default | actions |
|----------------|----------|----------|--------|---------|
| `/g4horus/init` |    :heavy_check_mark:      |    :x:      |  none  |    Initialises the geometry and user defined actions in the simulation.     |
| `/g4horus/output_type` | :x: | :heavy_check_mark: | hist |Specifies the output format of the simulation. Available options: [hist, ntuple, soco]|
| `/g4horus/gun/read_decay_scheme` | :heavy_check_mark: | :heavy_check_mark: | none | Specifies the JSON file path of the input decay scheme. The generation of the JSON file can be done in this [program](https://github.com/YanzhaoW/NuclearChartConverter).|
| `/g4horus/detector/distance` | :x: | :heavy_check_mark: | 1.0 cm | Specifies the distance between the radiation source and the detector front.|
| `/g4horus/gun/type` | :x: | :heavy_check_mark: | cascade | Specifies the type of the event generator. Available options: [single, cascade, scattering]|
| `/g4horus/gun/energy` | :x: | :x: | 0. keV | Specifies the energy of the decay.|
| `/g4horus/gun/cascade` | :x:| :x: | true | emits all decays of a cascade if true and emit a single decay if false.|
| `/analysis/setFileName` | :x:| :x: | hist.root | Set the output file name.|
| `/run/printProgress` | :x: | :x: | 1 | Show the progress per number of events.|
|`/run/beamOn` | :heavy_check_mark: | :x: | none | Set the number of events for the run.|

Example:
```text
/g4horus/output_type hist
/g4horus/gun/read_decay_scheme ../test/test.json
/g4horus/detector/distance 1.3 cm
/g4horus/gun/type cascade
/g4horus/init

/g4horus/gun/energy 919.337 keV
/g4horus/gun/cascade true
/run/printProgress 100
/analysis/setFileName test
/run/beamOn 1000
```
