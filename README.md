
# conscious-db

A C++ DBMS from scratch with included unit testing via GTest.




## Features

- Interactive REPL
- Custom memory Pager class for data persistence
- B+ Tree key-value store for efficient lookup and insertion (`git checkout BTree`)
- Fully supported dynamic data types for tables (**WIP**)
- Custom tokenizer and parser for SQL-like language (**WIP**)

## Run Locally

Clone the project

```bash
  git clone https://github.com/melon64/conscious-db.git
```

Go to the project directory

```bash
  cd conscious-db
```

Create a build directory and build using CMake

```bash
  mkdir build
  cd build
  cmake ..
  make all
```

Run the tests
```bash
  cd build/tst
  DBMS_tst.exe

```
Run the REPL
```bash
  cd build/src
  DBMS_run.exe
```



## Usage

Create an empty `.db` file and run the REPL:

```bash
  DBMS_run.exe [file.db]
```
You should see
```bash
db > 
```





## Commands
### Requirements
- C++ compiler (gcc, g++)

#### insert

Creates a Row object in the Table instantiated by the file supplied, inserts it into the B+ Tree, and serializes it into memory. **O(logN) amortized** 

```bash
  db > insert 1 user user@gmail.com
```

| Parameter | Type     | Description                        |
| :-------- | :------- | :--------------------------------  |
| `id`| `int` | any number  |
| `username`  | `string` | 32 character length           |
| `email`     | `string` | 255 character length|

#### select

Returns all the rows ordered by primary key (id). **O(N)**

```bash
  db > select
```

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `N/A` | `N/A` | `N/A` |

#### .exit

Exits the REPL and writes all serialized memory into the initial `.db` file. **O(N)**

```bash
  db > .exit
```

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `N/A` | `N/A` | `N/A` |

#### .btree

Formats and prints the underlying B+ Tree key-value storage. **O(N)**

```bash
  db > .btree
```

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `N/A` | `N/A` | `N/A` |

#### .constants

Prints the sizes of the B+ Tree nodes and Row in memory. **O(1)**

```bash
  db > .constants
```

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `N/A` | `N/A` | `N/A` |
