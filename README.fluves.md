# Building saga
## On windows

Make sure you install the libs4saga package and environment variables as outlined in [the saga wiki](https://sourceforge.net/p/saga-gis/wiki/Compiling%20SAGA%20on%20Windows/).
Use Visual Studio 2015 or more recent and open the file 
`src/saga-gis/saga.vc14.sln`
You can also look at the `appveyor.yml` file which provides automated builds.

To create a release go to saga-gis\src\accessories\helper and run make_saga_snapshot (or make saga_release)

## On debian/ubuntu linux:

Make sure you have `debuild` and all build dependencies installed:

```
sudo apt install devscripts build-essential lintian
sudo apt build-dep saga
```

Next, build and install from source:

```
debuild -b  # or 'fakeroot debian/rules binary'
sudo dpkg -i ../*saga*deb
```

To test your installation, run saga from terminal:

```
saga_cmd
```

Automated builds for ubuntu bionic are built for every commit on master using drone.
These are available at:
https://apt.fluves.net/amd64/