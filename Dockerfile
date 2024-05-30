FROM debian:stable-slim
RUN apt-get update && apt-get install -y build-essential libc6-dbg 
WORKDIR /build
RUN mkdir -p in out
COPY . .
RUN make
