# building-an-os
Trying to build an OS as a class project
Few things to keep in mind.
This is still a work in Progress.
The OS has some bugs regarding the file system. You can create, write, read, and delete a file but you may have run the command over and over again.
Also feel free to change the name of file or write some extra stuff on it.
I am trying to figure out what might be causing it.
It might have to do with the hdd.img not be accessed or something.

Now,
before you run this code,
You need Docker setup, Qemu installed, and then Run these commands.

These command need to be ran in order as it follows. 

docker build buildenv -t myos-buildenv
docker run --rm -it -v "%cd%":/root/env myos-buildenv
make build-x86_64
make create-hdd
make clean (Run this to reset the builds only.)
exit
qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso
docker rmi myos-buildenv -f