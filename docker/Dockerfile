FROM ubuntu:bionic
RUN sed -i~orig -e 's/# deb-src/deb-src/' /etc/apt/sources.list   
RUN apt update && apt upgrade -y
RUN DEBIAN_FRONTEND=noninteractive apt -y install tzdata
RUN apt build-dep saga -y && apt install -y git-buildpackage
