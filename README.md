# conscious-db
A C++ DBMS from scratch.

### Build
- Unit tests in /tst
 - uses [gtest](https://github.com/google/googletest "gtest")
- Source code in /src

##### Steps
- Create and navigate to build directory:\
`mkdir build`
`cd build`
- Run CMake:
  - Windows:
`cmake .. -DCMAKE_BUILD_TYPE=Debug -G "MinGW Makefiles"`
  - Linux:
`cmake .. -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles"`
- Run Make\
`make all`
