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

assert 0 "int main() { 0; }"
assert 42 "int main() { 42; }"
assert 21 "int main() { 5+20-4; }"
assert 21 "int main() { 5 + 20 - 4; }"
assert 47 'int main() { 5+6*7; }'
assert 15 'int main () { 5*(9-6); }'
assert 4 'int main() { (3+5)/2; }'
assert 15 'int main() { (2+3)*(1+2); }'
assert 10 'int main() { -10+20; }'
assert 30 'int main() { 10-(-40)-(+20); }'
assert 6 'int main() { 10-(-4*2)/((-4)/2); }'

assert 0 'int main () { 0==1; }'
assert 1 'int main () { 42==42; }'
assert 1 'int main () { 0!=1; }'
assert 0 'int main () { 42!=42; }'

assert 1 'int main () { 0<1; }'
assert 0 'int main () { 1<1; }'
assert 0 'int main () { 2<1; }'
assert 1 'int main () { 0<=1; }'
assert 1 'int main () { 1<=1; }'
assert 0 'int main () { 2<=1; }'

assert 1 'int main () { 1>0; }'
assert 0 'int main () { 1>1; }'
assert 0 'int main () { 1>2; }'
assert 1 'int main () { 1>=0; }'
assert 1 'int main () { 1>=1; }'
assert 0 'int main () { 1>=2; }'

assert 138 "int main () { 2+1;3+131+(2*2); }"
assert 1 "int main () { int a; a=1; }"
assert 5 "int main () { int a; a=2+3; }"
assert 4 "int main () { int a; int b; a=b=4; }"
assert 6 "int main () { int a; int b; int c; a=1;b=2;c=a+b+3; }"

assert 6 "int main () { int ab; int abc; int abcd; ab=1;abc=2;abcd=ab+abc+3; }"
assert 6 "int main () { int ab; int abc; int abcd; ab=1;abc=2;abcd=ab+abc+3; }"
assert 6 "int main () { int aB; int AbC; int aBcD; aB=1;AbC=2;aBcD=aB+AbC+3; }"

assert 2 "int main () { return 2; }"
assert 3 "int main () { int a; a = 3; return a; a = 2; }"
assert 6 "int main () { int returnAbc; int a; returnAbc = 3; a = 3; returnAbc + a; }"

# if
assert 0 "int main () { int x; x = 0; if (x) 1; x; }"
assert 1 "int main () { int x; x = 1; if (x-1) 1; x; }"
assert 0 "int main () { int a; int x; a = 3; x = 0; if (x) a = 10; x; }"
assert 1 "int main () { int a; int x; a = 1; x = 1; if (x-1) a = 10; x; }"
assert 0 "int main () { int a; int x; a = 1; x = 0; if (x) a = 10; x; }"
assert 1 "int main () { int a; int x; a = 1; x = 0; if (x) a = 10; a; }"
assert 1 "int main () { int a; int x; a = 1; x = 0; if (x) a = 10; x+a; }"
assert 1 "int main () { int a; int x; a = 1; x = 0; if (x) a = 10; a+x; }"
assert 1 "int main () { int a; int x; a = 1; x = 1; if (x) a = 10; x; }"
assert 10 "int main () { int a; int x; a = 1; x = 1; if (x) a = 10; a; }"
assert 11 "int main () { int a; int x; a = 1; x = 1; if (x) a = 10; x+a; }"
assert 11 "int main () { int a; int x; a = 1; x = 1; if (x) a = 10; a+x; }"

# if-else
assert 2 "int main () { int x; x = 0; if (x) x = 4; else x = 2; }"
assert 5 "int main () { int x; x = 1; if (x) x = 5; else x = 2; }"

# while
assert 0 "int main () { int a; a = 0; while (a) 1; }"
assert 0 "int main () { int a; a = 1; while (a) a = a - 1; }"
assert 3 "int main () { int a; a = 2; while (a) a = a - 1; a + 3; }"

# if
assert 3 "int main () { int x; int a; x = 123; for (a = 1; a < 3; a = a + 1) x; }"
assert 10 "int main () { int x; int a; x = 0; for (a = 0; a < 10; a = a + 1) x = x + 1; }"
assert 10 "int main () { int a; int x; a = 0; x = 0; for (a = 0; a < 10; a = a + 1) x = x + 1; x; }"
assert 3 "int main () { int x; int a; x = 0; for (a = 2; a < 100; a = (a * a)) x = x + 1; x; }"

# while - if - for 複数ある場合やネスト
assert 0 "int main () { int a; a = 1; if (a) a = 0; if (a) a = 2; a; }"
assert 8 "int main () { int a; int x; a = 0; for (x = 0; x < 3; x = x + 1) a = a + 1; for (x = 0; x < 5; x = x + 1) a = a + 1; a; }"
assert 20 "int main () { int a; a = 0; while (a < 10) a = a + 1; while (a < 20) a = a + 1; a; }"
assert 30 "int main () { int a; a = 1; if (a) a = 0; while (a < 30) a = a + 1; a; }"
assert 5 "int main () { int a; int j; a = 0; for (j = 1; j < 10; j = j + 1) a = a + 1; if (a == 9) a = 5; a; }"
assert 9 "int main () { int a; int x; int y; a = 0; for (x = 0; x < 3; x = x + 1) for (y = 0; y < 3; y = y + 1) a = a + 1; a; }"
assert 50 "int main () { int a; int x; int y; a = 0; for (x = 0; x < 5; x = x + 1) for (y = 0; y < 10; y = y + 1) a = a + 1; a; }"
assert 3 "int main () { int a; int b; int c; a = 1; b = 1; c = 1; if (a) if (b) c = 3; c; }"

# compound statement
assert 4 "int main () { int a; a = 1; if (a) { a = a + 1; a = a + 2; } }"
assert 16 "int main () { int a; a = 1; if (a) { a = a + 1; a = a + 2; a = a * a; } }"
assert 19 "int main () { int a; int x; x = 10; for (a = 1; a < 10; a = a + 1) x = x + 1; x; }"
assert 10 "int main () { int a; int x; a = 3; x = 5; if (a < 10) { a = a + 1; x = x * 2; } x; }"
assert 32 "int main () { int a; int x; a = 0; x = 2; while (a < 4) { a = a + 1; x = x * 2; } x; }"


# function call
assert 0 "int main () { foo0(); }"
assert 1 "int main () { foo1(1); }"
assert 3 "int main () { foo2(1, 2); }"
assert 6 "int main () { foo3(1, 2, 3); }"
assert 10 "int main () { foo4(1, 2, 3, 4); }"
assert 15 "int main () { foo5(1, 2, 3, 4, 5); }"
assert 21 "int main () { foo6(1, 2, 3, 4, 5, 6); }"

# function define
assert 1 "int foo() { return 1; } int main () { foo(); }"
assert 230 "int foo() { return 230; } int main () { foo(); }"
assert 230 "int foo() { return 230; } int main() { return foo(); }"
assert 55 "int fibonacci(int n) { int a; int b; int tmp; int i; a = 0; b = 1; if (n == 0) { return a; } if (n == 1) { return b; } for (i = 0; i < (n-1); i = i + 1) { tmp = a; a = b; b = tmp + b; } return b; } int main() { return fibonacci(10); }"
assert 55 "int fibonacci(int n) { if (n == 0) { return 0; } if (n == 1) { return 1; } return fibonacci(n-1) + fibonacci(n-2); } int main() { return fibonacci(10); }"

# ポインタ、アドレス
assert 3 "int main () { int x; int y; x = 3; y = &x; return *y; }"
assert 5 "int main () { int x; int y; int z; x = 5; y = 5; z = &y + 8; return *z; }"
assert 1 "int foo(int a) { return a; } int main() { foo(1); }"
assert 3 "int foo(int a, int b) { return a + b; } int main() { foo(1, 2); }"
assert 6 "int foo(int a, int b, int c) { return a + b + c; } int main() { foo(1, 2, 3); }"
assert 10 "int foo(int a, int b, int c, int d) { return a + b + c + d; } int main() { foo(1, 2, 3, 4); }"
assert 15 "int foo(int a, int b, int c, int d, int e) { return a + b + c + d + e; } int main() { foo(1, 2, 3, 4, 5); }"
assert 21 "int foo(int a, int b, int c, int d, int e, int f) { return a + b + c + d + e + f; } int main() { foo(1, 2, 3, 4, 5, 6); }"

# ポインタに代入
assert 10 "int main () { int x; int *y; y=&x; *y = 10; return x; }"
assert 167 "int main () { int x; int *y; int **z; y=&x; z = &y;  **z = 167; return x; }"
assert 3 "int main () { int a; int *b; int **c; int ***d; b=&a; c = &b; d = &c; ***d = 3; return a; }"

# 残課題
# マイナスの結果、256以上の数値
# callの呼び出し時にBSPが8の倍数の場合落ちることがある問題(発生未確認)


echo OK