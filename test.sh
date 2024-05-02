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

assert 0 "main() { 0; }"
assert 42 "main() { 42; }"
assert 21 "main() { 5+20-4; }"
assert 21 "main() { 5 + 20 - 4; }"
assert 47 'main() { 5+6*7; }'
assert 15 'main () { 5*(9-6); }'
assert 4 'main() { (3+5)/2; }'
assert 15 'main() { (2+3)*(1+2); }'
assert 10 'main() { -10+20; }'
assert 30 'main() { 10-(-40)-(+20); }'
assert 6 'main() { 10-(-4*2)/((-4)/2); }'

assert 0 'main () { 0==1; }'
assert 1 'main () { 42==42; }'
assert 1 'main () { 0!=1; }'
assert 0 'main () { 42!=42; }'

assert 1 'main () { 0<1; }'
assert 0 'main () { 1<1; }'
assert 0 'main () { 2<1; }'
assert 1 'main () { 0<=1; }'
assert 1 'main () { 1<=1; }'
assert 0 'main () { 2<=1; }'

assert 1 'main () { 1>0; }'
assert 0 'main () { 1>1; }'
assert 0 'main () { 1>2; }'
assert 1 'main () { 1>=0; }'
assert 1 'main () { 1>=1; }'
assert 0 'main () { 1>=2; }'

assert 138 "main () { 2+1;3+131+(2*2); }"
assert 1 "main () { a=1; }"
assert 5 "main () { a=2+3; }"
assert 4 "main () { a=b=4; }"
assert 6 "main () { a=1;b=2;c=a+b+3; }"

assert 6 "main () { ab=1;abc=2;abcd=ab+abc+3; }"
assert 6 "main () { aB=1;AbC=2;aBcD=aB+AbC+3; }"

assert 2 "main () { return 2; }"
assert 3 "main () { a = 3; return a;   a = 2; }"
assert 6 "main () { returnAbc = 3; a = 3; returnAbc + a; }"

# if
assert 0 "main () { x = 0; if (x) 1; x; }"
assert 1 "main () { x = 1; if (x-1) 1; x; }"
assert 0 "main () { a = 3; x = 0; if (x) a = 10; x; }"
assert 1 "main () { a = 1; x = 1; if (x-1) a = 10; x; }"
assert 0 "main () { a = 1; x = 0; if (x) a = 10; x; }"
assert 1 "main () { a = 1; x = 0; if (x) a = 10; a; }"
assert 1 "main () { a = 1; x = 0; if (x) a = 10; x+a; }"
assert 1 "main () { a = 1; x = 0; if (x) a = 10; a+x; }"
assert 1 "main () { a = 1; x = 1; if (x) a = 10; x; }"
assert 10 "main () { a = 1; x = 1; if (x) a = 10; a; }"
assert 11 "main () { a = 1; x = 1; if (x) a = 10; x+a; }"
assert 11 "main () { a = 1; x = 1; if (x) a = 10; a+x; }"

# if-else
assert 2 "main () { x = 0; if (x) x = 4; else x = 2; }"
assert 5 "main () { x = 1; if (x) x = 5; else x = 2; }"

# while
assert 0 "main () { a = 0; while (a) 1; }"
assert 0 "main () { a = 1; while (a) a = a - 1; }"
assert 3 "main () { a = 2; while (a) a = a - 1; a + 3; }"

# if
assert 3 "main () { x = 123; for (a = 1; a < 3; a = a + 1) x; }"
assert 10 "main () { x = 0; for (a = 0; a < 10; a = a + 1) x = x + 1; }"
assert 10 "main () { a = 0; x = 0; for (a = 0; a < 10; a = a + 1) x = x + 1; x; }"
assert 3 "main () { x = 0; for (a = 2; a < 100; a = (a * a)) x = x + 1; x; }"

# while - if - for 複数ある場合やネスト
assert 0 "main () { a = 1; if (a) a = 0; if (a) a = 2; a; }"
assert 8 "main () { a = 0; for (x = 0; x < 3; x = x + 1) a = a + 1; for (x = 0; x < 5; x = x + 1) a = a + 1; a; }"
assert 20 "main () { a = 0; while (a < 10) a = a + 1; while (a < 20) a = a + 1; a; }"
assert 30 "main () { a = 1; if (a) a = 0; while (a < 30) a = a + 1; a; }"
assert 5 "main () { a = 0; for (j = 1; j < 10; j = j + 1) a = a + 1; if (a == 9) a = 5; a; }"
assert 9 "main () { a = 0; for (x = 0; x < 3; x = x + 1) for (y = 0; y < 3; y = y + 1) a = a + 1; a; }"
assert 50 "main () { a = 0; for (x = 0; x < 5; x = x + 1) for (y = 0; y < 10; y = y + 1) a = a + 1; a; }"
assert 3 "main () { a = 1; b = 1; c = 1; if (a) if (b) c = 3; c; }"

# compound statement
assert 4 "main () { a = 1; if (a) { a = a + 1; a = a + 2; } }"
assert 16 "main () { a = 1; if (a) { a = a + 1; a = a + 2; a = a * a; } }"
assert 19 "main () { x = 10; for (a = 1; a < 10; a = a + 1) x = x + 1; x; }"
assert 10 "main () { a = 3; x = 5; if (a < 10) { a = a + 1; x = x * 2; } x; }"
assert 32 "main () { a = 0; x = 2; while (a < 4) { a = a + 1; x = x * 2; } x; }"


# function call
assert 0 "main () { foo0(); }"
assert 1 "main () { foo1(1); }"
assert 3 "main () { foo2(1, 2); }"
assert 6 "main () { foo3(1, 2, 3); }"
assert 10 "main () { foo4(1, 2, 3, 4); }"
assert 15 "main () { foo5(1, 2, 3, 4, 5); }"
assert 21 "main () { foo6(1, 2, 3, 4, 5, 6); }"

# function define
assert 1 "foo() { return 1; } main () { foo(); }"
assert 230 "foo() { return 230; } main () { foo(); }"
assert 230 "foo() { return 230; } main() { return foo(); }"


# ポインタ、アドレス
assert 3 "main () { x = 3; y = &x; return *y; }"
assert 5 "main () { x = 5; y = 5; z = &y + 8; return *z; }"
assert 1 "foo(a) { return a; } main() { foo(1); }"
assert 3 "foo(a, b) { return a + b; } main() { foo(1, 2); }"
assert 6 "foo(a, b, c) { return a + b + c; } main() { foo(1, 2, 3); }"
assert 10 "foo(a, b, c, d) { return a + b + c + d; } main() { foo(1, 2, 3, 4); }"
assert 15 "foo(a, b, c, d, e) { return a + b + c + d + e; } main() { foo(1, 2, 3, 4, 5); }"
assert 21 "foo(a, b, c, d, e, f) { return a + b + c + d + e + f; } main() { foo(1, 2, 3, 4, 5, 6); }"

# 残課題
# マイナスの結果、256以上の数値
# callの呼び出し時にBSPが8の倍数の場合落ちることがある問題(発生未確認)

# 関数呼び出しの引数に演算がある場合

echo OK