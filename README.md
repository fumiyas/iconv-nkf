iconv(3)-compatible API and $LD_PRELOAD-able library by NKF
======================================================================

  * Copyright (c) 2013 SATOH Fumiyasu @ OSS Technology Corp., Japan
  * License: BSD (3 clause)
  * URL: <https://github.com/fumiyas/iconv-nkf>
  * Blog: <http://fumiyas.github.io/>
  * Twitter: <https://twitter.com/satoh_fumiyasu>


``` console
$ echo '123 あ ①' |iconv -f UTF-8 -t Shift_JIS |iconv -f Shift_JIS -t UTF-8
iconv: illegal input sequence at position 8
$ export LD_PRELOAD=/usr/local/lib/iconv-nkf.so
$ echo '123 あ ①' |iconv -f UTF-8 -t Shift_JIS |iconv -f Shift_JIS -t UTF-8
123 あ ①
```
