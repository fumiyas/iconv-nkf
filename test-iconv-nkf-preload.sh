#!/bin/sh

set -u
LD_PRELOAD=`pwd`/iconv-nkf.so
export LD_PRELOAD

echo "Test: iconv-nkf.so: iconv(1) で柔軟なエンコーディング変換"
str_in='123 あ ①'
str_out=`
  echo "$str_in" \
  |iconv --from 'UTF-8' --to 'Shift_JIS' \
  |iconv --from 'Shift_JIS' --to 'EUC-JP' \
  |iconv --from 'EUC-JP' --to 'ISO-2022-JP' \
  |iconv --from 'ISO-2022-JP' --to 'UTF-8' \
  ;
`
echo "期待値: $str_in (`echo $str_in |od -tx1 |sed -n '1s/^[^ ]* //p'`)"
echo "結果値: $str_out (`echo $str_out |od -tx1 |sed -n '1s/^[^ ]* //p'`)"
echo "判定: "`[ "$str_in" = "$str_out" ] && echo "OK" || echo "NG"`
echo

