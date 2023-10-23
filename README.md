# SAGA GIS WATEM module
This repository contains a [SAGA-GIS](https://saga-gis.org) translation of the [WATEM erosion model](https://cn-ws.github.io/). It also contains modules that can be used in preprocessing and postprocessing the result of that program.

More details on the modules can be found in:
- [Potential erosion map](docs/potential_erosion_map.md)
- [Topology](docs/topology.md)

# Installing SAGA

## On windows
On windows, download version SAGA 8.5.1 from the website, a direct link to the file is here [saga-8.5.1_x64.zip](https://sourceforge.net/projects/saga-gis/files/SAGA%20-%208/SAGA%20-%208.5.1/saga-8.5.1_x64.zip/download).

Get the windows built dll files from the releases page and copy these dll files into the tools subfolder of saga and restart. You can also drag and drop the dll on saga if they are stored in a different location.

If you want to test unreleased version of SAGA, go to the github action page for [windows builds](https://github.com/johanvdw/saga-watem/actions/workflows/cmake-windows.yml). You can find snapshots of the dll here.

## On linux
Packages are built for Debian Linux (Bullseye) and Ubuntu LTS (Focal). The pacakges can be grabbed from the release page.

# Building saga
## On windows

Download SAGA (compiled version) and create an environmental variable `SAGA` containing the path where saga was installed/unzipped. Install Visual Studio or Visual studio build tools and cmake and create the solution file using cmake:

```
mkdir build
cmake ..
```

This will create a solution that can be opened with visual studio, or can be built using msbuild.

## On debian/ubuntu linux:

Make sure you have build-essential, cmake and libsaga-dev installed

```
sudo apt install build-essential cmake libsaga-dev
```

Next, build and install from source:

```
mkdir build
cd build
cmake ..
make
```

## Running automated tests

A few automated tests are written in python. To run these under linux, run the compilation as mentioned above.

Next install the dependencies either in an environment or as apt packages and run

```
sudo apt install python-pytest python-rasterio python-geopandas
pytest-3
```

