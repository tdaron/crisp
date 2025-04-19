# Crisp

Crisp is still a POC in development.

## How to run

Simply compile using

    make

Then you can launch the REPL using

    ./crisp-repl


Current supported features:

- [x] Defines
- [x] Basic addition
- [x] Functions
- [x] Recursion


## Samples

```sc
crisp> (define x 100)
type: NUM
value: 100
crisp> (define y 200)
type: NUM
value: 200
crisp> (+ x y (+ 1 2))
type: NUM
value: 303
crisp> (define (fact n) (if (= n 0) 1 (* n (fact (+ n -1)))))
crisp> (fact 5)
type: NUM
value: 120
crisp>
```
