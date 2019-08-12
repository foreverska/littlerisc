# LittleRisc #

This is a library which provides a RISC-V rv32i emulator.  It isn't pretty yet and it's rather simplistic but I have waited until it can (with some finessing) run a C program before uploading it.

This was primarily built as an academic excercise.

### Build it ###

The build system for the library is cmake.  The tests can be run using ctest.  I don't claim the test are exhaustive but they got the project here.
The hello_world example C program is built using make.

### Run it ###

An example program is compiled as part of cmake called riscv_emu.

### License ###

This project is licensed under GNU General Public License v3.0.  See LICENSE.txt for more details.

