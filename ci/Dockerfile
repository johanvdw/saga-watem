ARG base_container=debian:bookworm
FROM ${base_container}
ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC
RUN apt-get update && apt-get upgrade -y && \
    apt-get -y install --no-install-recommends git build-essential libsaga-dev saga cmake python3-pytest python3-rasterio python3-geopandas python3-distutils ca-certificates debhelper devscripts&& \
    rm -rf /var/lib/apt/lists/*
    
