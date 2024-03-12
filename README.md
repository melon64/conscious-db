# conscious-db
A C++ DBMS from scratch.
- B+ Tree implementation as underlying data structure (internal + leaf node splitting and insertion, **switch to BTree branch**)
- Custom tokenizer and parser for SQL-like language (**WIP**)
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

##### Currently supports:
>insert
>select
