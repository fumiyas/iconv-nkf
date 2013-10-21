#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "iconv-nkf.h"

#define max(a, b) ((a) > (b) ? (a) : (b))

void hexdump(const char *str, size_t len) {
  size_t i;

  for (i = 0; i < len; i++) {
    printf("%s%02x", (i == 0 ? "" : " "), (unsigned char)str[i]);
  }
  puts("");
}

int main(void) {
  iconv_nkf_t cd;
  char inbuf[8192], outbuf[8192];
  char *inptr, *outptr;
  size_t inleft, inlen, outleft, outlen, ret;

  /* 「あ」(E3 81 82) を分割して渡すテスト */

  cd = iconv_nkf_open("Shift_JIS", "UTF-8");

  puts("Test: iconv_nkf(): マルチバイト文字の分割渡し Part 1");
  strcpy(inbuf, "\xE3\x81");
  inptr = inbuf;
  inleft = strlen(inptr);
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
  inleft = strlen(inptr);
  errno = 0;
  ret = iconv_nkf(cd, &inptr, &inleft, &outptr, &outleft);
  inlen = inptr - inbuf;
  outlen = outptr - outbuf;
  printf("期待値: 戻値= 0, 入力元消費=3, 出力先消費=2, errno=%d (%s)\n",
    0, strerror(0)
  );
  printf("結果値: 戻値=%2ld, 入力元消費=%ld, 出力先消費=%ld, errno=%d (%s)\n",
    ret, inptr - inbuf, outptr - outbuf, errno, strerror(errno)
  );
  printf("判定: ");
  if (ret == (size_t)0 && inlen == 3 && outlen == 2 && errno == 0) {
    puts("OK");
  }
  else {
    puts("NG");
  }

  puts("Test: iconv_nkf(): マルチバイト文字の分割渡し変換結果");
  printf("期待値: ");
  hexdump("\x82\xA0", 2);
  printf("結果値: ");
  hexdump(outbuf, outlen);
  printf("判定: ");
  if (!memcmp(outbuf, "\x82\xA0", max(2, outlen))) {
    puts("OK");
  }
  else {
    puts("NG");

  }

  iconv_nkf_close(cd);

  /* 「あい」(ISO-2022-JP 表現) を分割して渡すテスト */

  cd = iconv_nkf_open("UTF-8", "ISO-2022-JP");

  puts("Test: iconv_nkf(): ISO-2022-JP 文字列の分割渡し Part 1");
  strcpy(inbuf, "\x1B\x24\x42\x24\x22");
  inptr = inbuf;
  inleft = strlen(inptr);
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

  puts("Test: iconv_nkf(): ISO-2022-JP 文字列の分割渡し Part 2");
  strcat(inbuf, "\x24\x24\x1B\x28\x42");
  inleft = strlen(inptr);
  errno = 0;
  ret = iconv_nkf(cd, &inptr, &inleft, &outptr, &outleft);
  inlen = inptr - inbuf;
  outlen = outptr - outbuf;
  printf("期待値: 戻値= 0, 入力元消費=10, 出力先消費=6, errno=%d (%s)\n",
    0, strerror(0)
  );
  printf("結果値: 戻値=%2ld, 入力元消費=%ld, 出力先消費=%ld, errno=%d (%s)\n",
    ret, inptr - inbuf, outptr - outbuf, errno, strerror(errno)
  );
  printf("判定: ");
  if (ret == (size_t)0 && inlen == 10 && outlen == 6 && errno == 0) {
    puts("OK");
  }
  else {
    puts("NG");
  }

  puts("Test: iconv_nkf(): ISO-2022-JP 文字列の分割渡し変換結果");
  printf("期待値: ");
  hexdump("あい", sizeof("あい") - 1);
  printf("結果値: ");
  hexdump(outbuf, outlen);
  printf("判定: ");
  if (!memcmp(outbuf, "あい", max(sizeof("あい") - 1, outlen))) {
    puts("OK");
  }
  else {
    puts("NG");

  }

  iconv_nkf_close(cd);

  return 0;
}

