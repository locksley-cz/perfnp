PerfNP
======

This is a tool for measuring performance of NP-hard algorithms. Properties:
- Focus on the command-line:
    - The benchmarked algorithm must be embedded in a CLI.
    - `perfnp` is a CLI-first program.
    - Errors are handled using exit codes.
- It is prepared for failures:
    - We assume that the algorithm may fail.
    - All executions have a defined time-limit.
    - Statistical measures are meaningful even if there are >90% failures.
- No micro-benchmarking: This tool is used for algorithms that run in seconds or minutes at least.



Quick usage
-----------

Assume that you want to measure how fast can you find the group of automorphisms using
[`nauty`](http://pallini.di.uniroma1.it/) on a set of a set of graphs.

First create a config file `config.json`:
```json
{
    "timeout" : 60,
    "command" : "nauty",
    "arguments" : ["%input_file%"],
    "parameters" : [{
        "name" : "input_file",
        "values" : ["graph1.txt", "graph2.txt", "graph3.txt"]
    }]
}
```

And then run
```
$ perfnp config.json
```

You should get a result
```
Jobs to execute: 3
Median runtime:  40s +- 10s
Successful runs: 20s +- 8s
```



Building
--------

### a) on Linux

[![Build Status](https://travis-ci.org/locksley-cz/perfnp.svg?branch=master)](https://travis-ci.org/locksley-cz/perfnp)

1) Install `git`, `cmake` and a C++ compiler (`gcc` or `clang`).

2) Clone the `perfnp` repository:
```
git clone https://github.com/locksley-cz/perfnp.git
cd perfnp
```

3) Run the build script and execute tests
```
misc/build.sh
```

4) All went fine if you see a result like this:
```
===============================================================================
All tests passed (73 assertions in 14 test cases)
```

5) The executable binary can be found in `build/perfnp`

### b) on Windows

[![Build status](https://ci.appveyor.com/api/projects/status/7e58e1g18pen8ben/branch/master?svg=true)](https://ci.appveyor.com/project/cernoch/perfnp/branch/master)

1) Install [git](https://git-scm.com/), [cmake](https://cmake.org/) and [Visual Studio](https://visualstudio.microsoft.com).

2) Clone the `perfnp` repository:
```
git clone https://github.com/locksley-cz/perfnp.git
cd perfnp
```

3) Run the build script and execute tests
```
misc\build.bat
```

4) All went fine if you see a result like this:
```
===============================================================================
All tests passed (73 assertions in 14 test cases)
```

5) The executable binary can be found in `build\Release\perfnp.exe`

### c) using Docker

1) Install [docker](https://www.docker.com/).

2) Choose your favourite distribution:
    - Alpine: `docker build -t perfnp:alpine -f misc/docker/alpine .`
    - Ubuntu: `docker build -t perfnp:ubuntu -f misc/docker/ubuntu .`

3) The executable binary can be found in the built image in `/opt/perfnp/build/perfnp`