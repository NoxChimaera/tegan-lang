# tegan-lang
Toy programming language

## Build
Prerequisites:
- Clang 3.5
- LLVM 3.5

Build with command line:
```bash
$ make
```

## Run
Run compiler directly:
```bash
$ dist/tlc <input> <output>
```

Run by script (will automatically run generated file):
```bash
$ ./run.sh <input> <output>
```

## Optimization
Available optimization passes:
- instruction combining,
- expressions reassociating,
- dead code elimination,
- global value numbering,
- CFG simplifying.

Read further: [Wiki](https://github.com/NoxChimaera/tegan-lang/wiki/%D0%9E%D0%BF%D1%82%D0%B8%D0%BC%D0%B8%D0%B7%D0%B0%D1%86%D0%B8%D1%8F)(Russian)
