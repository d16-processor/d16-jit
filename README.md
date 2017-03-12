# d16-jit: A JIT emulator for the D16 Processor


## Installation:

d16-jit depends on GLib and requires a lua interpreter to compile (Not required when running the program). 
Additionally, it requires a C compiler that supports C11 or provides `atomic.h`.


On Debian, `glib` and `lua` can be installed by running:

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
## Incompatibilities

Because d16-jit is partially written in assembly, it will only run on x86_64 systems using the System V ABI. This means that it will not run natively on windows, but may run under the Windows Subsystem for Linux. D16-jit has been tested on mac osx, and supports linux. 


#Attribution
d16-jit uses [DynASM](https://luajit.org/dynasm.html) written by Mike Pall for JIT compilation.
