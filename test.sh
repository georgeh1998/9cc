#!/bin/bash
assert() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s #コンパイラが出力するアセンブラ
    cc -o tmp tmp.s c_func.c #出力したアセンブラを機械語に変換
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

# if
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

# if-else
assert 2 "x = 0; if (x) x = 4; else x = 2;"
assert 5 "x = 1; if (x) x = 5; else x = 2;"

# while
assert 0 "a = 0; while (a) 1;"
assert 0 "a = 1; while (a) a = a - 1;"
assert 3 "a = 2; while (a) a = a - 1; a + 3;"

# if
assert 3 "x = 123; for (a = 1; a < 3; a = a + 1) x;"
assert 10 "x = 0; for (a = 0; a < 10; a = a + 1) x = x + 1;"
assert 10 "a = 0; x = 0; for (a = 0; a < 10; a = a + 1) x = x + 1; x;"
assert 3 "x = 0; for (a = 2; a < 100; a = (a * a)) x = x + 1; x;"

# while - if - for 複数ある場合やネスト
assert 0 "a = 1; if (a) a = 0; if (a) a = 2; a;"
assert 8 "a = 0; for (x = 0; x < 3; x = x + 1) a = a + 1; for (x = 0; x < 5; x = x + 1) a = a + 1; a;"
assert 20 "a = 0; while (a < 10) a = a + 1; while (a < 20) a = a + 1; a;"
assert 30 "a = 1; if (a) a = 0; while (a < 30) a = a + 1; a;"
assert 5 "a = 0; for (j = 1; j < 10; j = j + 1) a = a + 1; if (a == 9) a = 5; a;"
assert 9 "a = 0; for (x = 0; x < 3; x = x + 1) for (y = 0; y < 3; y = y + 1) a = a + 1; a;"
assert 50 "a = 0; for (x = 0; x < 5; x = x + 1) for (y = 0; y < 10; y = y + 1) a = a + 1; a;"
assert 3 "a = 1; b = 1; c = 1; if (a) if (b) c = 3; c;"

# compound statement
assert 4 "a = 1; if (a) { a = a + 1; a = a + 2; }"
assert 16 "a = 1; if (a) { a = a + 1; a = a + 2; a = a * a; }"
assert 19 "x = 10; for (a = 1; a < 10; a = a + 1) x = x + 1; x;"
assert 10 "a = 3; x = 5; if (a < 10) { a = a + 1; x = x * 2; } x;"
assert 32 "a = 0; x = 2; while (a < 4) { a = a + 1; x = x * 2; } x;"


# function call
assert 0 "foo0();"
assert 1 "foo1(1);"
assert 3 "foo2(1, 2);"
assert 6 "foo3(1, 2, 3);"
assert 10 "foo4(1, 2, 3, 4);"
assert 15 "foo5(1, 2, 3, 4, 5);"
assert 21 "foo6(1, 2, 3, 4, 5, 6);"

# function define
assert 1 "foo() { return 1; } foo();"
assert 230 "foo() { return 230; } foo();"


# ポインタ、アドレス
assert 3 "x = 3; y = &x; return *y;"
assert 5 "x = 5; y = 5; z = &y + 8; return *z;"

# 残課題
# マイナスの結果、256以上の数値
# callの呼び出し時にBSPが8の倍数の場合落ちることがある問題(発生未確認)

echo OK