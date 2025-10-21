# NOTE : cmake 4.2.1 is currently only available in edge
FROM alpine:edge AS base
WORKDIR /app
# Refreshes the list of available packages
RUN apk update
# Installs the newer versions of already installed packages
RUN apk upgrade
# Installs standard C++ library
RUN apk add --no-cache libstdc++


# BUILDER STAGE
FROM base AS builder
WORKDIR /app
# Refreshes the list of available packages
RUN apk update
# Installs the newer versions of already installed packages
RUN apk upgrade
# Installs necessary packages without caching anything
RUN apk add --no -cache g++ make cmake>4.1.2
# Copies necessary root files and directories
COPY CMakeLists.txt CMakeLists.txt
COPY core core
COPY server server
# Creates build
RUN cmake -S . -B build
RUN cmake --build build


# RUNNER STAGE
FROM base AS runner
WORKDIR /app
# Copies Server executable from builder stage into root
COPY --from=builder /app/build/server/Server Server
# Executes Server executale
CMD ["./Server"]
