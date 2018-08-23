# reasoncc

## Project goals

A project to learn facebook reason and test the benefits and tradeoffs
of immutable/functional programming while building a toy C compiler.

In theory we can use the features of reason to write less code and deliver faster.

To get a comparison, we can compare the reason code to C code:

- https://github.com/andrewchambers/c
- https://github.com/rui314/8cc
- https://github.com/rui314/9cc
- https://bellard.org/tcc/

Some interesting objective metrics are:

- Lines of code.
- Compiler speed.

Some subjective metrics:

- How enjoyable it is to work in.
- How easy the code is to understand.

# Build
```
npm run build
```
or
```
bsb
```

# Short Notes
- For now we target nodejs, but reason can generate native binaries.
  There is little point in setting that up now.

- Code is automatically formatted with refmt... very nice.

- I don't have much experience with reason.

- This project might be abandoned as soon as I am bored with it.

- It might be nice to make a blog using reason react for good visualizations
  of how everything works.

# Experience reports

## Package manager

I personally dislike nodejs and npm, and bs-platform, the reason/bucklescript
build system uses it. To avoid using npm, I personally use the nix package manager
and installed bs-platform via a nix package I made using node2nix. I will upstream
this package later.

What is nice though, is that people familiar with nodejs don't need to understand
reason to install the package, and I can distribute the package via npm while
needing very little interaction with the node ecosystem.


## Type inference

The inference of return type for records feels really pleasant, even
magical at times :).

## Immutable values

If you sacrifice performance, and don't update values in place, it is very
convenient to test functions via a repl. I am experimenting with a lexer
that is immutable. Functions go from lexer => (lexer, token), and you
can rerun them as much as you want in a repl.

## Compiler errors

Clearly the reason compiler uses something like yacc for parsing, This means
the compiler errors for syntax problems are very bad, simply a line and col with
no extra info.

However, the errors for semantic bugs, i.e. undefined variables, or type errors
are excellent, you can tell the developers spent lots of time on it.


