# Brainfuck interpreter
An interpreter for the esoteric Brainfuck programming language written in C.
It features extended functionality such as the # character to display the memory, and string literals.

## String literals
The interpreter can read strings and copy them to the memory tape. The copied string will be followed by the number of characters written to memory.
For example,
```bf
>"Hello world!" Copy the string 'Hello world'
```
The resulting memory:
| 0 | 72 | 101 | 108 | 108 | 111 | 32 | 119 | 111 | 114 | 108 | 100 | 33 | 12 |
|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
