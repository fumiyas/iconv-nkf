#include <stdio.h>
#include <string.h>
#include <errno.h>

typedef unsigned char uchar;

#include "iconv-nkf.h"

int main(void) {
  iconv_nkf_t cd;
  char inbuf[8192], outbuf[8192];
  char *inptr, *outptr;
  size_t inleft, outleft, ret;

  /* 「あ」(E3 81 82) を分割して渡すテスト */

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
  printf("判定: ");
  if (ret == (size_t)-1 && inptr == inbuf && outptr == outbuf && errno == EINVAL) {
    puts("OK");
  }
  else {
    puts("NG");
  }

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
  printf("判定: ");
  if (ret == (size_t)0 && inptr-inbuf == 3 && outptr-outbuf == 2 && errno == 0) {
    puts("OK");
  }
  else {
    puts("NG");
  }

  puts("Test: iconv_nkf(): マルチバイト文字の分割渡し変換結果");
  printf("期待値: %02x %02x\n", 0x82, 0xA0);
  printf("結果値: %02x %02x\n", (uchar)outbuf[0], (uchar)outbuf[1]);
  printf("判定: ");
  if ((uchar)outbuf[0] == 0x82 && (uchar)outbuf[1] == 0xA0) {
    puts("OK");
  }
  else {
    puts("NG");
  }

  return 0;
}

