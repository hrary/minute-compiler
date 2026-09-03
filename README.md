# Minute Compiler

A compiler for a subset of C, written in C, targeting x86-64.

It emits AT&T-syntax assembly and hands the result to `as` and `ld` via `gcc`.
There is no preprocessor and no assembler; this is the middle of the toolchain:
source text in, assembly text out.

```
source.c
   ↓  lexer            characters → tokens
   ↓  parser           tokens → AST (recursive descent)
   ↓  symbol table     names → rbp-relative stack slots
   ↓  codegen          AST → x86-64 assembly
out.s
   ↓  gcc              assemble + link
executable
```

## Build and run

```sh
make
./mycc program.c > out.s
gcc out.s -o program
./program; echo $?
```

`--dump-tokens` and `--dump-ast` print the intermediate stages to stderr.

## Supported language

Type `int` only.

- Arithmetic `+ - * /` with correct precedence and left associativity
- Comparisons `== != < <= > >=`
- Assignment `=`, right-associative and chainable
- Local variables with optional initialisers
- `if` / `else`, `while`, `for`, and `{ }` blocks
- Function definitions and calls, up to 6 parameters
- Recursion, including mutual recursion

## Implementation notes

**Expression codegen holds one invariant: every expression leaves its result in
`%rax`.** Given that, compiling a binary operation is mechanical: generate the
right operand, push it, generate the left operand, pop into `%rdi`, emit the
instruction. It composes to any nesting depth because both subtrees honour the
same contract.

The cost is that the generated code is a stack machine. `2+3*4` becomes eight
instructions with four memory accesses where three registers would do. This is
not an oversight: a recursive AST walk structurally cannot know which registers
are free, because that depends on what is live in the enclosing expression and
in sibling subtrees. Answering that requires a flat representation with liveness
information, which is what the IR and register allocator below are for.

**Operator precedence is encoded in the grammar, not in a table.** Each
precedence level is a function that calls the next tighter one:

```
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = primary ("*" primary | "/" primary)*
```

By the time `add` sees a `+`, `mul` has already consumed the entire
multiplication to its right, so `+` can only ever receive a complete subtree as
an operand. Adding a precedence level means inserting one function; no existing
caller changes.

**`>` and `>=` do not exist in the AST.** `a > b` is parsed as `ND_LT` with the
operands swapped, which removes two cases from codegen. Similarly `while` is
parsed into the `for` node with null init and increment clauses, so both loops
share one code path.

**Function parameters live in two linked lists at once.** They are locals, so
they are on `fn->locals` (reverse declaration order, used for name lookup and
stack-slot assignment); they are also parameters, so they are on `fn->params`
(source order, used to emit the register-to-slot copies in the prologue). Each
`Obj` therefore carries two `next` pointers. Sharing one field makes the two
lists point into each other and the symbol-table walk never terminates.

## Calling convention

The System V AMD64 ABI is implemented for the integer-argument case:

- Arguments in `%rdi %rsi %rdx %rcx %r8 %r9`, return value in `%rax`
- Arguments are evaluated onto the stack and then popped into registers, so a
  nested call cannot clobber an argument register that is already set
- Prologue copies incoming registers into the callee's stack slots
- Frames are rounded up to a multiple of 16 bytes to keep `%rsp` aligned at
  `call` sites

Argument order is the failure mode worth testing deliberately: symmetric
operations like addition pass even when the order is reversed. `sub2(10, 3)`
returning `-7` instead of `7` is the diagnostic.

## Testing

The test suite is differential. Every program in `tests/` is compiled twice 
(once by `gcc`, once by this compiler) and the two binaries' exit codes are
compared. gcc is the oracle, so expected values are never written by hand.

```sh
./test.sh
```

52 programs currently pass. Comparing exit codes rather than generated assembly
is the point: the two compilers emit very different code and are only required
to agree on observable behaviour.

## Not supported

Deliberately out of scope: the preprocessor, types other than `int`, pointers,
arrays, structs, globals, `%`, unary operators, compound assignment, logical and
bitwise operators, `break` / `continue` / `switch`, comments, and more than six
parameters.

One real limitation rather than a scope decision: **there is no block scoping.**
A variable declared inside a `{ }` block or a `for` initialiser stays visible for
the remainder of the function. Fixing this needs a scope stack in the parser.

Function calls are not checked against a declaration, so calling an undefined
function is a link-time error rather than a compile-time one.

## Next

- Lower the AST to a three-address IR over unbounded virtual registers
- Linear-scan register allocation with live intervals and stack spilling
- Constant folding and dead-code elimination over the IR

## References

Rui Ueyama's [chibicc](https://github.com/rui314/chibicc) is the closest
reference implementation; its commit-by-commit history is worth reading.