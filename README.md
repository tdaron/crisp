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
crisp>
```
