#include <stdio.h>
#include <string.h>

#include "iconv-nkf.h"

int main(void) {
  iconv_nkf_t cd;
  size_t inleft, outleft, ret;
  char inbuf[8192];
  char outbuf[8192];
  char *inptr, *outptr;

  cd = iconv_nkf_open("Shift_JIS", "UTF-8");

  strcpy(inbuf, "\xE3\x81");
  inptr = inbuf;
  inleft = 2;
  outptr = outbuf;
  outleft = sizeof(outbuf);
  ret = iconv_nkf(cd, &inptr, &inleft, &outptr, &outleft);
  printf("status:       %ld\n", ret);
  printf("in consumed:  %ld\n", inptr - inbuf);
  printf("out consumed: %ld\n", outptr - outbuf);
  strcat(inbuf, "\x82");
  inleft += 1;
  ret = iconv_nkf(cd, &inptr, &inleft, &outptr, &outleft);
  printf("status:       %ld\n", ret);
  printf("in consumed:  %ld\n", inptr - inbuf);
  printf("out consumed: %ld\n", outptr - outbuf);
  *outptr = '\0';
  printf("[%s]\n", outbuf);

  return 0;
}
