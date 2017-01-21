# d16-jit: A JIT emulator for the D16 Processor

## Installation:

d16-jit depends on GLib and requires a lua interpreter to compile (Not required when running the program). On Debian, they can be installed by running
```
sudo apt-get install libglib2.0-dev luajit
```
or on a mac:
```
brew install glib luajit
```
It can then be compiled by running:
```
mkdir build && cd build
cmake ..
make
make install
```

#Attribution
d16-jit uses [DynASM](https://luajit.org/dynasm.html) written by Mike Pall for JIT compilation.
