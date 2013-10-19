#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "iconv-nkf.h"

int main(void) {
  iconv_nkf_t cd;
  char inbuf[8192], outbuf[8192];
  char *inptr, *outptr;
  size_t inleft, outleft, ret;

  cd = iconv_nkf_open("Shift_JIS", "UTF-8");

  puts("Test: iconv_nkf(): マルチバイト文字の分割渡し Part 1");
  strcpy(inbuf, "\xE3\x81");
  inptr = inbuf;
  inleft = 2;
  outptr = outbuf;
  outleft = sizeof(outbuf);
  errno = 0;
  ret = iconv_nkf(cd, &inptr, &inleft, &outptr, &outleft);
  printf("期待値: 戻値=-1, 入力元消費=0, 出力先消費=0, errno=%d (%s)\n",
    EINVAL, strerror(EINVAL)
  );
  printf("結果値: 戻値=%2ld, 入力元消費=%ld, 出力先消費=%ld, errno=%d (%s)\n",
    ret, inptr - inbuf, outptr - outbuf, errno, strerror(errno)
  );

  puts("Test: iconv_nkf(): マルチバイト文字の分割渡し Part 2");
  strcat(inbuf, "\x82");
  inleft += 1;
  errno = 0;
  ret = iconv_nkf(cd, &inptr, &inleft, &outptr, &outleft);
  printf("期待値: 戻値= 0, 入力元消費=3, 出力先消費=2, errno=%d (%s)\n",
    0, strerror(0)
  );
  printf("結果値: 戻値=%2ld, 入力元消費=%ld, 出力先消費=%ld, errno=%d (%s)\n",
    ret, inptr - inbuf, outptr - outbuf, errno, strerror(errno)
  );
  *outptr = '\0';
  printf("[%s]\n", outbuf);

  return 0;
}

