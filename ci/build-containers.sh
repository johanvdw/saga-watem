for i in "debian:bookworm" "debian:bullseye" "ubuntu:focal";
do 
	podman build --build-arg base_container=$i -t ghcr.io/johanvdw/saga-watem-build-$i .
	podman push ghcr.io/johanvdw/saga-watem-build-$i
done;
