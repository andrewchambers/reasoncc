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

- lines of code
- compiler speed

Some subjective metrics:

- how enjoyable it is to work in.
- how easy the code is to understand.

# Build
```
npm run build
```
or
```
bsb
```

# Notes

- For now we target nodejs, but reason can generate native binaries.
  There is little point in setting that up now.
- Code is automatically formatted with refmt
- I don't have much experience with reason.
- This project might be abandoned as soon as I am bored with it.