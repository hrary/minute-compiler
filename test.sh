#!/bin/bash
pass=0; fail=0
shopt -s nullglob
files=(tests/*.c)
if [ ${#files[@]} -eq 0 ]; then echo "no tests found"; exit 1; fi
for f in tests/*.c; do
    gcc "$f" -o /tmp/ref 2>/dev/null; /tmp/ref; expected=$?
    ./mycc "$f" /tmp/got.s >/dev/null 2>&1 || { echo "COMPILE FAIL $f"; fail=$((fail+1)); continue; }
    gcc /tmp/got.s -o /tmp/got 2>/dev/null || { echo "ASSEMBLE FAIL $f"; fail=$((fail+1)); continue; }
    /tmp/got; actual=$?
    if [ "$expected" = "$actual" ]; then pass=$((pass+1))
    else echo "FAIL $f: expected $expected, got $actual"; fail=$((fail+1)); fi
done
echo "passed $pass, failed $fail"
[ "$fail" -eq 0 ]