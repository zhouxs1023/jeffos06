This is the main directory for the JeffOS which appeared in Phrack #52.

The directory structure is as follows:

article - Contains the phrack article on this source code

boot - Contains source for first stage kernel boot loader

build - Contains binaries from builds

kernel - Contains source for the main kernel

kernelv2 - Contains source for the second version of the kernel which will be
           used as a base for all future kernels.

lib - Contains source code for user level libraries

loader - Contains floppy loader source code
  |
  --------first - Contains loader source for first stage floppy boot loader
  |
  --------second - Cotnains loader source for second stage floppy boot loader

user - Contains user level source code to test kernel

ver0.1 - Contains original binaries from build 0.1
