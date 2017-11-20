# Game Maker Language beautifier

*Compatible with Game Maker Studio 1.x*

Yes, that's right, you no longer have to fiddle with use a javascript beautifier that doesn't support all of GML syntax!

The beautifier works by parsing the file into a (comment-decorated) syntax tree and printing out.

## Features

- Supports all syntax unique to GML, be it `with` or `repeat` or `do...until` statements, or `or` and `and` keywords, or funky `accessor[? x]` and `2Darray[x, y]` syntax.
- Idempotence: running the beautifier on previously-beautified code does not modify it.
- Compares order of all tokens between input and output to make sure the program's behaviour is identical
- Preserves comments
- Options to satisfy the variety of GML standards used out there:
  - line width
  - semicolons, no semicolons, or leave as-is
  - egpytian braces (same-line or next-line)
  - standardizing spaces at start of comments `//comment` <-> `// comment`
  - standardizing blank lines before comments
  - use parentheses with if, while, with, repeat, do..until statements: `if cond` <-> `if (cond)`
