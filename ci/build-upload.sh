#docker build  . -f Dockerfile-bionic -t registry.fluves.net/drone/saga-watem/build-bionic
docker build  . -f Dockerfile-bullseye -t registry.fluves.net/drone/saga-watem/build-bullseye
docker build  . -f Dockerfile-focal -t registry.fluves.net/drone/saga-watem/build-focal
#docker push registry.fluves.net/drone/saga-watem/build-bionic
docker push registry.fluves.net/drone/saga-watem/build-bullseye
docker push registry.fluves.net/drone/saga-watem/build-focal
