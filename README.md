[![MIT License](https://img.shields.io/badge/License-MIT-green.svg)](https://choosealicense.com/licenses/mit/)

# Crisp


Crisp is a Lisp-like programming language designed for both scripting and configuration tasks. While it doesn't have a specific purpose, it's lightweight (though it still requires libc) and can be embedded wherever needed. This repository includes both a compiler that targets a custom stack-based virtual machine and the VM implementation itself. Crisp also includes basic Raylib bindings, allowing you to create graphical applications!

## Syntax

The syntax is easy to learn (if you love parentheses)!

Each expression has the form `(operation arg1 arg2 ...)`.

For example:
- `(+ 1 2 3 4)` will output `10.0`
- Use `if` to create conditional branches with the syntax `(if condition true_result false_result)`. For example, `(if (= 2 3) 5 10)` means "If 2 equals 3, output 5; otherwise, output 10" (the result will be 10)
- Define variables with `(define x 100)`
- Define functions with `(define (function_name param1 param2) (body))`

The `begin` function is special - it allows you to execute multiple expressions in sequence:

```lisp
(begin
    (define x 100)
    (define y 200)
    (define z (+ x y))
    (print z)
)
```

With just these concepts, you can build anything you want in Crisp!

## Installation

Clone the project:
```bash
git clone https://github.com/tdaron/crisp
```

Navigate to the project directory:
```bash
cd crisp
```

Build the project:
```bash
make -j8
```

This will compile the project and create an executable named `crisp`.

## Usage Examples

Running the executable gives you a REPL (Read-Eval-Print Loop) where you can try Crisp. Note that multiline editing isn't supported, so everything must be written on a single line:

```lisp
crisp> (define x 100)
crisp> (define y 200)
crisp> (+ x y (+ 1 2))
Stack: [303.000000 ]
crisp> (define (fact n) (if (= n 0) 1 (* n (fact (+ n -1)))))
Registered function 026B603F at position 237 with 1 args
crisp> (fact 5)
Stack: [120.000000 ]
crisp> (define (fib n) (if (< n 2) n (+ (fib (- n 1)) (fib (- n 2)))))
Registered function BCE49236 at position 354 with 1 args
crisp> (fib 28)
Stack: [317811.000000 ]
```

While the REPL is useful for testing, it's not yet refined enough for pleasant development.

You can also execute code from a file by providing the filename as an argument to `crisp`. In these files, you can spread your code across multiple lines. For multiple operations, wrap the entire file in a `begin` block:

```lisp
(begin
    (define x 20)
    (define y 100)
    (define z (+ x y))
    (print z)
)
```

## Graphics with Raylib

The repository includes Raylib bindings, allowing you to create video games in Crisp.

To enable Raylib support, build with:
```bash
make -B WITH_RAYLIB=1 -j8
```

Here's an example (from demo.lisp) that creates a simple animation:

```lisp
(begin
  (define x 20)
  (init-window 600 800)
  (define (frame) (
      if (window-should-close) 0 (begin
          (begin-drawing)
          (clear 200 0 0)
          (rect x 50 20 20 0 255 0)
          (define x (+ x 0.01))
          (end-drawing)
          (frame)
        )
  ))
  (frame)
)
```

## API Reference

This section of the documentation is still in development. For now, you can examine the `raylib.c` file to learn how to create simple bindings.

## Roadmap

- Optimize the VM
- Add objects (like lists) and internal objects (files, etc.)
- Add transducers/contracts
- Build an optional standard library for basic functionality

