# Game Maker Language beautifier

*Compatible with Game Maker Studio 1.4.x*

Yes, that's right, you no longer have to fiddle with use a javascript beautifier that doesn't support all of GML syntax!

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

Run `cmake .` and `make`. No dependencies. Run `./gml-tools-test` and make sure the unit tests pass.

## Usage

### Beautification:

After building, run `./gml-tools path/to/file`. If the file is a script (.gml file) then the script will be beautified and printed out to the console. If the file is a project file, then the whole project at that directory will be beautified in-place. **Back up your project before beautifying! Use version control. Although there are unit tests, it's always possible there is a bug in the code and the beautifier could malfunction and delete everything.** 