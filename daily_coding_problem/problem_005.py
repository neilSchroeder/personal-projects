"""
cons(a, b) constructs a pair, and car(pair) and cdr(pair) returns the first
 and last element of that pair. For example, car(cons(3, 4)) returns 3, and
  cdr(cons(3, 4)) returns 4.
"""

def cons(a, b):
    def pair(f):
        return f(a, b)
    return pair


def car(p):
    def x(a,b):
        return a
    return p(x)


def cdr(p):
    def x(a,b):
        return b
    return p(x)


def main():
    
    assert(car(cons(3,4)) == 3)
    assert(cdr(cons(3,4)) == 4)


if __name__ == '__main__':
    main()