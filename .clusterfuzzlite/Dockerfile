FROM gcr.io/oss-fuzz-base/base-builder:v1

# Install Meson and Ninja dependencies
RUN apt-get update && apt-get install -y python3-pip python3-setuptools python3-wheel ninja-build
RUN pip3 install meson

# Copy source code into the container
COPY . $SRC/opensea-common
WORKDIR $SRC/opensea-common