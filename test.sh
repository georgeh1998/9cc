#!/bin/bash
assert() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s #コンパイラが出力するアセンブラ
    cc -o tmp tmp.s        #出力したアセンブラを機械語に変換
    ./tmp                  #実行
    actual="$?"

    if [ "$actual" = "$expected" ]; then
      echo "$input => $actual"
    else
      echo "$input => $expected expected but got $actual"
      exit 1
    fi
}

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 21 "5 + 20 - 4;"
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 15 '(2+3)*(1+2);'
assert 10 '-10+20;'
assert 30 '10-(-40)-(+20);'
assert 6 '10-(-4*2)/((-4)/2);'

assert 0 '0==1;'
assert 1 '42==42;'
assert 1 '0!=1;'
assert 0 '42!=42;'

assert 1 '0<1;'
assert 0 '1<1;'
assert 0 '2<1;'
assert 1 '0<=1;'
assert 1 '1<=1;'
assert 0 '2<=1;'

assert 1 '1>0;'
assert 0 '1>1;'
assert 0 '1>2;'
assert 1 '1>=0;'
assert 1 '1>=1;'
assert 0 '1>=2;'

assert 138 "2+1;3+131+(2*2);"
assert 1 "a=1;"
assert 5 "a=2+3;"
assert 4 "a=b=4;"
assert 6 "a=1;b=2;c=a+b+3;"

assert 6 "ab=1;abc=2;abcd=ab+abc+3;"
assert 6 "aB=1;AbC=2;aBcD=aB+AbC+3;"

assert 2 "return 2;"
assert 3 "a = 3; return a;   a = 2;"
assert 6 "returnAbc = 3; a = 3; returnAbc + a;"

assert 0 "x = 0; if (x) 1; x;"
assert 1 "x = 1; if (x-1) 1; x;"
assert 0 "a = 3; x = 0; if (x) a = 10; x;"
assert 1 "a = 1; x = 1; if (x-1) a = 10; x;"

assert 0 "a = 1; x = 0; if (x) a = 10; x;"
assert 1 "a = 1; x = 0; if (x) a = 10; a;"
assert 1 "a = 1; x = 0; if (x) a = 10; x+a;"
assert 1 "a = 1; x = 0; if (x) a = 10; a+x;"

assert 1 "a = 1; x = 1; if (x) a = 10; x;"
assert 10 "a = 1; x = 1; if (x) a = 10; a;"
assert 11 "a = 1; x = 1; if (x) a = 10; x+a;"
assert 11 "a = 1; x = 1; if (x) a = 10; a+x;"

# while
assert 0 "a = 0; while (a) 1;"
assert 0 "a = 1; while (a) a = a - 1;"
assert 3 "a = 2; while (a) a = a - 1; a + 3;"

echo OK