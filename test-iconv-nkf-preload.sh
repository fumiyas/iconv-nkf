#!/bin/sh

set -u
LD_PRELOAD=`pwd`/iconv-nkf.so
export LD_PRELOAD

str_in='123 あ ①'
str_out=`
  echo "$str_in" \
  |iconv --from 'UTF-8' --to 'Shift_JIS' \
  |iconv --from 'Shift_JIS' --to 'EUC-JP' \
  |iconv --from 'EUC-JP' --to 'ISO-2022-JP' \
  |iconv --from 'ISO-2022-JP' --to 'UTF-8' \
  ;
`

echo "Expected: $str_in (`echo $str_in |od -tx1 |sed -n '1s/^[^ ]* //p'`)"
echo "Got:      $str_out (`echo $str_out |od -tx1 |sed -n '1s/^[^ ]* //p'`)"
echo "Result:   "`[ "$str_in" = "$str_out" ] && echo "OK" || echo "NG"`

