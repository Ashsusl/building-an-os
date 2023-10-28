# building-an-os
Trying to build an OS as a class project
Also here are the necessary commands to build the environment and emulate the OS in qemu
docker build buildenv -t myos-buildenv
docker run --rm -it -v "%cd%":/root/env myos-buildenv
make build-x86_64
exit
qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso
docker rmi myos-buildenv -f