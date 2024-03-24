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

assert 0 0
assert 42 42
assert 21 "5+20-4"
assert 21 "5 + 20 - 4"
assert 47 '5+6*7'
assert 15 '5*(9-6)'
assert 4 '(3+5)/2'
assert 15 '(2+3)*(1+2)'

echo OK