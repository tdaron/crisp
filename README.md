# Crisp

Crisp is still a POC in development.

## How to run

Simply compile using

    make

Then you can launch the REPL using

    ./crisp-repl


Current supported features:

- [x] Defines
- [x] Basic addition / multiplication
- [x] Branching (only if atm)
- [x] Functions
- [x] Recursion


## Samples

```sc
crisp> (define x 100)
crisp> (define y 200)
crisp> (+ x y (+ 1 2))
Stack: [303.000000 ]
crisp>  (define (fact n) (if (= n 0) 1 (* n (fact (+ n -1)))))
Registered function 026B603F at position 237 with 1 args
crisp> (fact 5)
Stack: [120.000000 ]
crisp> (define (fib n) (if (< n 2) n (+ (fib (- n 1)) (fib (- n 2)))))
Registered function BCE49236 at position 354 with 1 args
crisp> (fib 28)
Stack: [317811.000000 ]
crisp>
```
