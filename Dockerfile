#
# node-httpp packaging Dockerfile
#
# https://github.com/InstantWebP2P/node-httpp
# Copyright (c) 2020 Tom Zhou<appnet.link@gmail.com>

# Pull base image.
FROM ubuntu:18.04

# Install.

# Add files.
COPY * /appins/

# Set environment variables.
ENV HOME /root

# Define working directory.
WORKDIR /appins

# Define default command.
CMD ["./bin/node"]
