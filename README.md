NKF の iconv(3) 互換 API と $LD_PRELOAD ライブラリー
======================================================================

  * Copyright (c) 2013 SATOH Fumiyasu @ OSS Technology Corp., Japan
  * ライセンス: BSD (3 条項)
  * URL: <https://github.com/fumiyas/iconv-nkf>
  * Blog: <http://fumiyas.github.io/>
  * Twitter: <https://twitter.com/satoh_fumiyasu>

使用例
----------------------------------------------------------------------

``` console
$ echo '123 あ ①' |iconv -f UTF-8 -t Shift_JIS |iconv -f Shift_JIS -t UTF-8
iconv: illegal input sequence at position 8
$ export LD_PRELOAD=/usr/local/lib/iconv-nkf.so
$ echo '123 あ ①' |iconv -f UTF-8 -t Shift_JIS |iconv -f Shift_JIS -t UTF-8
123 あ ①
```

TODO (制限)
----------------------------------------------------------------------

  * `EINVAL` 対応
    * 入力でマルチバイド文字を分割して渡された場合の対応。
  * `E2BIG` 対応
    * 出力で出力バッファーが不足していた場合の対応。
  * マルチスレッド対応

