#
# node-httpp packaging Dockerfile
#
# https://github.com/InstantWebP2P/node-httpp
# Copyright (c) 2020 Tom Zhou<appnet.link@gmail.com>

# Pull base image.
FROM ubuntu:18.04

# Install.
RUN \
 apt-get -y update && \
 apt-get install -y python g++ gcc make

# Add files.
COPY . /appins/

# Set environment variables.
ENV HOME /root
ENV PATH /appins/bin:$PATH
ENV LD_LIBRARY_PATH /appins/lib:$LD_LIBRARY_PATH

# Add files.
COPY * /appins/

# Set environment variables.
ENV HOME /root

# Define working directory.
WORKDIR /appins

# Define default command.
CMD ["node"]
