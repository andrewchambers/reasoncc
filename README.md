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

# Experience reports

## Package manager

I dislike nodejs and npm, however bs-platform, the reason/bucklescript
build system uses it. What is nice though, is that people familiar with
nodejs don't need to understand reason to install the package due to the
package.json facade wrapping the bucklescript config.

To avoid using npm, I use the nix package manager
and installed bs-platform via a nix package I made using node2nix.
It is nice that I can distribute the package via npm while
needing very little interaction with the node ecosystem, while at the same time,
they don't need to understand or care about my workflow or tools either.

It is actually quite impressive to watch npm build the ocaml compiler and ninja
build tool while hiding this information from the end user, all they see is 
a nodejs workflow initially.

## Type inference

The inference of return type for records feels really pleasant, even
magical at times :). In fact it almost ruins writing code in languages
like go or c. Sometimes I worry it hurts readability, but perhaps an 
editor plugin could show the types in long form as ghost text somewhere.
Usually I don't like using an ide, but if it was easy enough to setup
I wouldn't mind.

## Immutable values

If you sacrifice performance, and don't update values in place, it is very
convenient to test functions via a repl. I am experimenting with a lexer
that is immutable. Functions go from lexer => (lexer, token), and you
can rerun them as much as you want in a repl.

The syntax {...old, v=newv} to make copy of a record/struct with a small change is nice.

## Compiler errors

Clearly the reason compiler uses something like yacc for parsing, This means
the compiler errors for syntax problems are very bad, simply a line and col with
no extra info.

The errors for semantic bugs (i.e. undefined variables, or type errors)
are excellent, you can tell the developers spent lots of time making them
understandable and useful.

Reframing the idea of a type error to 'finding bugs' is a fun concept,
clearly this is marketed to javascript developers, and maybe it works for
the purpose.

## Code formatter

The code formatter is really nice for someone who doesn't know idiomatic formatting,
and doesn't even have an advanced text editor. I can just hack code to make it work, 
and let the formatter fix it up.

Having this in a programming language is becomming a requirement, people are realizing just
how must time formatting wastes...

## Host platform Interop

This was easy to use. I only need to command line arguments, and the ability to read files
from disk to strings, and it only took me a few mintes to call the native nodejs readFileSync
from a type safe interface.

# Short Notes
- For now we target nodejs, but reason can generate native binaries.
  There is little point in setting that up now.

- Code is automatically formatted with refmt... very nice.

- I don't have much experience with reason.

- This project might be abandoned as soon as I am bored with it.

- It might be nice to make a blog using reason react for good visualizations
  of how everything works.
