#!/bin/bash
assert() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s #コンパイラが出力するアセンブラ
    cc -static -o tmp tmp.s c_func.c #出力したアセンブラを機械語に変換
    ./tmp                  #実行
    actual="$?"

    if [ "$actual" = "$expected" ]; then
      echo "$input => $actual"
    else
      echo "$input => $expected expected but got $actual"
      exit 1
    fi
}

assert 0 "./test/basic_1.c"
assert 42 "./test/basic_2.c"
assert 21 "./test/basic_3.c"
assert 21 "./test/basic_4.c"
assert 47 "./test/basic_5.c"
assert 15 "./test/basic_6.c"
assert 4 "./test/basic_7.c"
assert 15 "./test/basic_8.c"
assert 10 "./test/basic_9.c"
assert 30 "./test/basic_10.c"
assert 6 "./test/basic_11.c"

assert 0 "./test/equality_1.c"
assert 1 "./test/equality_2.c"
assert 1 "./test/equality_3.c"
assert 0 "./test/equality_4.c"

assert 1 "./test/compare_1.c"
assert 0 "./test/compare_2.c"
assert 0 "./test/compare_3.c"
assert 1 "./test/compare_4.c"
assert 1 "./test/compare_5.c"
assert 0 "./test/compare_6.c"
assert 1 "./test/compare_7.c"
assert 0 "./test/compare_8.c"
assert 0 "./test/compare_9.c"
assert 1 "./test/compare_10.c"
assert 1 "./test/compare_11.c"
assert 0 "./test/compare_12.c"

assert 138 "./test/int_local_var_1.c"
assert 1 "./test/int_local_var_2.c"
assert 5 "./test/int_local_var_3.c"
assert 4 "./test/int_local_var_4.c"
assert 6 "./test/int_local_var_5.c"
assert 6 "./test/int_local_var_6.c"
assert 6 "./test/int_local_var_7.c"
assert 2 "./test/int_local_var_8.c"
assert 3 "./test/int_local_var_9.c"
assert 6 "./test/int_local_var_10.c"

# if
assert 0 "./test/if_1.c"
assert 1 "./test/if_2.c"
assert 0 "./test/if_3.c"
assert 1 "./test/if_4.c"
assert 0 "./test/if_5.c"
assert 1 "./test/if_6.c"
assert 1 "./test/if_7.c"
assert 1 "./test/if_8.c"
assert 1 "./test/if_9.c"
assert 10 "./test/if_10.c"
assert 11 "./test/if_11.c"
assert 11 "./test/if_12.c"

# if-else
assert 2 "./test/if_else_1.c"
assert 5 "./test/if_else_2.c"

# while
assert 0 "./test/while_1.c"
assert 0 "./test/while_2.c"
assert 3 "./test/while_3.c"

# for
assert 3 "./test/for_1.c"
assert 10 "./test/for_2.c"
assert 10 "./test/for_3.c"
assert 3 "./test/for_4.c"

# while - if - for 複数ある場合やネスト
assert 0 "./test/while_if_for_nest_1.c"
assert 8 "./test/while_if_for_nest_2.c"
assert 20 "./test/while_if_for_nest_3.c"
assert 30 "./test/while_if_for_nest_4.c"
assert 5 "./test/while_if_for_nest_5.c"
assert 9 "./test/while_if_for_nest_6.c"
assert 50 "./test/while_if_for_nest_7.c"
assert 3 "./test/while_if_for_nest_8.c"

# compound statement
assert 4 "./test/compound_statement_1.c"
assert 16 "./test/compound_statement_2.c"
assert 19 "./test/compound_statement_3.c"
assert 10 "./test/compound_statement_4.c"
assert 32 "./test/compound_statement_5.c"

# function call (c_func.c)
assert 0 "./test/function_call_1.c"
assert 1 "./test/function_call_2.c"
assert 3 "./test/function_call_3.c"
assert 6 "./test/function_call_4.c"
assert 10 "./test/function_call_5.c"
assert 15 "./test/function_call_6.c"
assert 21 "./test/function_call_7.c"

# function define
assert 1 "./test/function_define_1.c"
assert 230 "./test/function_define_2.c"
assert 230 "./test/function_define_3.c"
assert 55 "./test/function_define_4.c"
assert 55 "./test/function_define_5.c"

# 関数
assert 1 "./test/function_1.c"
assert 3 "./test/function_2.c"
assert 6 "./test/function_3.c"
assert 10 "./test/function_4.c"
assert 15 "./test/function_5.c"
assert 21 "./test/function_6.c"

# ポインタ、アドレス
assert 3 "./test/pointer_address_1.c"
assert 10 "./test/pointer_address_2.c"
assert 57 "./test/pointer_address_3.c"
assert 167 "./test/pointer_address_4.c"
assert 3 "./test/pointer_address_5.c"

# ポインタの加算/減算
assert 5 "./test/pointer_add_sub_1.c"
assert 1 "./test/pointer_add_sub_2.c"
assert 1 "./test/pointer_add_sub_3.c"


# sizeof
assert 4 "./test/sizeof_1.c"
assert 4 "./test/sizeof_2.c"
assert 4 "./test/sizeof_3.c"
assert 4 "./test/sizeof_4.c"
assert 8 "./test/sizeof_5.c"

# 配列定義
assert 4 "./test/array_define_1.c"
assert 4 "./test/array_define_2.c"
assert 40 "./test/array_define_3.c"
assert 80 "./test/array_define_4.c"

# 配列の代入
assert 3 "./test/array_assign_1.c"

# 配列の添字
assert 12 "./test/array_index_1.c"
assert 13 "./test/array_index_2.c"
assert 14 "./test/array_index_3.c"
assert 15 "./test/array_index_4.c"

# グローバル変数
assert 130 "./test/global_var_1.c"
assert 131 "./test/global_var_2.c"
assert 132 "./test/global_var_3.c"
assert 133 "./test/global_var_4.c"
assert 134 "./test/global_var_5.c"

# char型導入
assert 3 "./test/char_1.c"
assert 4 "./test/char_2.c"
assert 5 "./test/char_3.c"

# 文字列リテラル導入
assert 246 "./test/string_1.c"
assert 10 './test/string_2.c'
assert 10 './test/string_3.c'


# comment
assert 161 "./test/comment_1.c"
assert 162 "./test/comment_2.c"

# assert 'int main() { int a; int b; a = -1; b = -1; char *c; c = "%d\n"; int d; printf(c, -4); }'

# 第二引数の-が動かない
# assert 254 'int foo(int a, int b) { return a + b; } int main() { return foo(-1, -1) }'

# 現在の問題
# printfを呼ぶために関数のsignature登録をやめた

# 残課題
# マイナスの結果、256以上の数値
# callの呼び出し時にBSPが8の倍数の場合落ちることがある問題(発生未確認)

# TODO TODO
# 関数の戻り値と引数の型チェック
# 関数の戻り値と引数の型にポインタを対応させる
# 同じ関数名で異なるsignatureは対応していない
# =にも型情報を持たせて異なる型を代入させない


echo OK