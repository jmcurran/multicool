# accel_asc: generate all partitions of n as ascending compositions.
#
# Author: Jerome Kelleher
#
# This is the Python code from which the C++ function generateCompositions
# in src/compositions.cpp (used by genComp()) was ported. It is included
# here, unchanged, to credit the author and so that the original source is
# available now that it is no longer online.
#
# Source: Jerome Kelleher, "Generating integer partitions", published on
# jeromekelleher.net on 26 May 2014. That site is no longer available; this
# copy was taken from the Internet Archive's snapshot of 23 April 2026
# (retrieved 22 October 2026). The page itself carried no licence statement.
# The package DESCRIPTION records the code as distributed without conditions.
#
# The algorithm is described in:
#   Kelleher, J. and O'Sullivan, B. (2009), Generating All Partitions: A
#   Comparison Of Two Encodings. arXiv:0909.2331.
#   https://doi.org/10.48550/arXiv.0909.2331
#
# Each partition is yielded as a list in ascending order, e.g. for n = 4:
#   [1, 1, 1, 1], [1, 1, 2], [1, 3], [2, 2], [4]


def accel_asc(n):
    a = [0 for i in range(n + 1)]
    k = 1
    y = n - 1
    while k != 0:
        x = a[k - 1] + 1
        k -= 1
        while 2 * x <= y:
            a[k] = x
            y -= x
            k += 1
        l = k + 1
        while x <= y:
            a[k] = x
            a[l] = y
            yield a[:k + 2]
            x += 1
            y -= 1
        a[k] = x + y
        y = x + y - 1
        yield a[:k + 1]
