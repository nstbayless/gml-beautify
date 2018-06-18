# Game Maker Language beautifier

*Compatible with Game Maker Studio 1.4.x*

Yes, that's right, you no longer have to fiddle with a javascript beautifier that doesn't support all of GML syntax!

The beautifier works by parsing the file into a (comment-decorated) syntax tree and printing out.

## Features

- Supports all syntax unique to GML, be it `with` or `repeat` or `do...until` statements, or `or` and `and` keywords, or funky `accessor[? x]` and `2Darray[x, y]` syntax.
- Idempotence: running the beautifier on previously-beautified code does not modify it.
- Compares order of all tokens between input and output to make sure the program's behaviour is identical
- Preserves comments
- Configurable options to satisfy the variety of GML standards used out there:
  - line width
  - semicolons, no semicolons, or leave as-is
  - block brace styles: same-line (Egyptian) or next-line
  - standardizing spaces at start of comments `//comment` <-> `// comment`
  - standardizing blank lines before comments
  - use parentheses with if, while, with, repeat, do..until statements: `if cond` <-> `if (cond)`

## Build Instructions

### Linux

Run `cmake .` and `make`. No dependencies. Run `./gml-tools-test` and make sure the unit tests pass.

### Windows

1. Install [CMake](https://cmake.org/download/). **Make sure to add it to the PATH**.
2. Install [MinGW](https://sourceforge.net/projects/mingw/files/latest/download). Use the default install options.
3. Once MinGW is installed, press "Continue" (or open the MinGW Installation Manager). Select `mingw32-base`, `mingw32-gcc-g++`, and `msys-base`; mark them for installation. Click on `Installation -> Apply Changes` to begin the install. This shouldn't take more than 10 minutes. 
4. Wait until everything above is installed. Add `C:\MinGW\bin` to the PATH (assuming you installed it there.) 
5. Open the CMake GUI. Select this repository as the source code and binary build location, then click Generate. It should output "Configuring Done" and "Generating Done" at the end. If not, an error occurred and you must fix it before proceeding.
6. Open a new command window (`cmd.exe`), navigate to this repository, and run `mingw32-make`. This should succeed without error.

The commands below assume the executable is called `./gml-tools` and `./gml-tool-test`. On Windows, you may need to use `gml-tools.exe` and `gml-tools-test.exe` respectively. Run `gml-tools-test.exe` to confirm the build is working.

## Usage

### Beautification:

After building, run `./gml-tools path/to/file`. If the file is a script (.gml file) then the script will be beautified and printed out to the console. If the file is a project file, then the whole project at that directory will be beautified in-place. **Back up your project before beautifying! Use version control. Although there are unit tests, it's always possible there is a bug in the code and the beautifier could malfunction and delete everything.** 

Include the comment `//@noformat` somewhere in a code action / file to prevent that action / file from being formatted.
