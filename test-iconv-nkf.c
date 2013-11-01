#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "iconv.h"
#include "iconv-nkf.h"

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

void hexdump(const char *str, size_t len) {
  size_t i;

  for (i = 0; i < len; i++) {
    printf("%02x%s", (unsigned char)str[i], ((i+1) % 10 ? " " : "|"));
  }
  puts("");
}

int i, test_num = 0, test_ok = 0, test_warning = 0, test_error = 0;
char *i_strs[] = {
  "A",
  "あ",
  "\x1B",
  "Aあ",
  "あA",
  "\x1B""Aあ",
  "A\x1Bあ",
  "Aあ\x1B",
  "ABあ",
  "あAB",
  "Aあい",
  "あいB",
  "ABあい",
  "あいBB",
  "AあB",
  "あAい",
  "0\x1B(B1\x1B$B3",
  "あ\x1B(Bい\x1B$Bう",
  "ABCDEFGあ0123456",
  "あいうえおA１２３４５",
  "ABCDEFG""\x1B\x1B""0123456",
  "ABCDEFG""\x1B\x1B\x1B""0123456",
  "ABCDEFG""\x1B \x1B""0123456",
  "あいうえお""\x1B\x1B""１２３４５",
  "あいうえお""\x1B\x1B\x1B""１２３４５",
  "あいうえお""\x1B \x1B""１２３４５",
  NULL
};

void test_assert(const char *title, int flag, int warn) {
  printf("Test %4d: ", ++test_num);
  if (flag) {
    printf("OK");
    test_ok++;
  }
  else if (warn) {
    printf("WARNING");
    test_warning++;
  }
  else {
    printf("ERROR");
    test_error++;
  }
  if (title) {
    printf(": %s", title);
  }
  printf("\n");
}

void test(int, char *);
void test2(const char *, size_t, const char *, const char *);

int main(void) {
  int i;

  for (i = 0; i_strs[i]; i++) {
    puts("======================================================================");
    test(i, i_strs[i]);
  }

  printf("Test result: %s (OK=%d ERROR=%d WARNING=%d)\n",
      test_error ? "ERROR" : "OK", test_ok, test_error, test_warning);

  return test_error ? 1 : 0;
}

void test(
  int i_index,
  char *i_str
) {
  size_t i_strlen = strlen(i_str);
  char *encodings[] = {
    "UTF-8", "EUC-JP", "Shift_JIS", "ISO-2022-JP", NULL
  };

  int from;
  for (from = 0; encodings[from]; from++) {
    char i_buf[8192];
    char *i_strptr, *i_ptr;
    size_t i_strleft, i_left;

    iconv_t cd = iconv_open(encodings[from], "UTF-8");
    i_strptr = i_str;
    i_strleft = i_strlen;
    i_ptr = i_buf;
    i_left = sizeof(i_buf);
    iconv(cd, &i_strptr, &i_strleft, &i_ptr, &i_left);

    size_t i_len = i_ptr - i_buf;
    int to;
    for (to = 0; encodings[to]; to++) {
      if (!strcmp(encodings[from], "ISO-2022-JP") &&
	  !strcmp(encodings[to], "ISO-2022-JP")) {
	continue;
      }
      puts("----------------------------------------------------------------------");
      printf("String in UTF-8: %d: %s\n", i_index, i_str);
      printf("String in UTF-8 hex: ");
      hexdump(i_str, i_strlen);
      test2(i_buf, i_len, encodings[from], encodings[to]);
    }
  }
}

void test2(
  const char *i_buf,
  size_t i_len,
  const char *from,
  const char *to
) {
  const char *i_ptr;
  size_t i_left;
  size_t i_step;

  iconv_t org_cd;
  int org_errno;
  char org_i_buf[8192], org_o_buf[8192];
  char *org_i_ptr, *org_o_ptr;
  size_t org_i_len, org_ret;
  size_t org_i_left, org_i_eaten;
  size_t org_o_left, org_o_eaten;

  iconv_nkf_t nkf_cd;
  int nkf_errno;
  char nkf_i_buf[8192], nkf_o_buf[8192];
  char *nkf_i_ptr, *nkf_o_ptr;
  size_t nkf_i_len, nkf_ret;
  size_t nkf_i_left, nkf_i_eaten;
  size_t nkf_o_left, nkf_o_eaten;

  for (i_step = i_len; i_step > 0; i_step--) {
    org_cd = iconv_open(to, from);
    if (org_cd == (iconv_t)-1) {
      printf("iconv_open failed: %s -> %s: %s\n", from, to, strerror(errno));
      exit(1);
    }
    org_i_ptr = org_i_buf;
    org_i_left = 0;
    org_o_ptr = org_o_buf;
    org_o_left = sizeof(org_o_buf);

    nkf_cd = iconv_nkf_open(to, from);
    if (nkf_cd == (iconv_nkf_t)-1) {
      printf("iconv_nkf_open failed: %s -> %s: %s\n", from, to, strerror(errno));
      exit(1);
    }
    nkf_i_ptr = nkf_i_buf;
    nkf_i_left = 0;
    nkf_o_ptr = nkf_o_buf;
    nkf_o_left = sizeof(nkf_o_buf);

    i_ptr = i_buf;
    i_left = i_len;
    for (; i_ptr <= i_buf + i_len; i_ptr += i_step, i_left -= min(i_step, i_left)) {
      printf("From=%s To=%s i_len=%ld i_step=%ld\n", from, to, i_len, i_step);

      memcpy(org_i_ptr + org_i_left, i_ptr, min(i_step, i_left));
      org_i_len = org_i_left += min(i_step, i_left);
      printf("String org in: ");
      hexdump(org_i_ptr, org_i_left);
      errno = 0;
      org_ret = iconv(org_cd, org_i_len ? &org_i_ptr : NULL, &org_i_left, &org_o_ptr, &org_o_left);
      org_errno = errno;
      org_i_eaten = org_i_ptr - org_i_buf;
      org_o_eaten = org_o_ptr - org_o_buf;

      memcpy(nkf_i_ptr + nkf_i_left, i_ptr, min(i_step, i_left));
      nkf_i_len = nkf_i_left += min(i_step, i_left);
      printf("String nkf in: ");
      hexdump(nkf_i_ptr, nkf_i_left);
      errno = 0;
      nkf_ret = iconv_nkf(nkf_cd, nkf_i_len ? &nkf_i_ptr : NULL, &nkf_i_left, &nkf_o_ptr, &nkf_o_left);
      nkf_errno = errno;
      nkf_i_eaten = nkf_i_ptr - nkf_i_buf;
      nkf_o_eaten = nkf_o_ptr - nkf_o_buf;

      printf("Result org: "
	"戻値=%2ld, 入力元消費=%ld, 出力先消費=%ld, errno=%d (%s)\n",
	org_ret, org_i_eaten, org_o_eaten, org_errno, strerror(org_errno)
      );
      printf("Result nkf: "
	"戻値=%2ld, 入力元消費=%ld, 出力先消費=%ld, errno=%d (%s)\n",
	nkf_ret, nkf_i_eaten, nkf_o_eaten, nkf_errno, strerror(nkf_errno)
      );
      test_assert(
	"iconv() result: Values",
	(org_ret == nkf_ret && org_errno == nkf_errno &&
	  org_i_eaten == nkf_i_eaten &&
	  org_o_eaten == nkf_o_eaten),
	nkf_i_len > 0
      );

      printf("String org out: ");
      hexdump(org_o_buf, org_o_eaten);
      printf("String nkf out: ");
      hexdump(nkf_o_buf, nkf_o_eaten);

      test_assert(
	"iconv() result: Output string",
	!memcmp(org_o_buf, nkf_o_buf, min(org_o_eaten, nkf_o_eaten)),
	0
      );
      test_assert(
	"iconv() result: Output string length",
	(org_o_eaten == nkf_o_eaten),
	1
      );
    }

    iconv_close(org_cd);
    iconv_nkf_close(nkf_cd);

    size_t org_o_len = org_o_ptr - org_o_ptr;
    size_t nkf_o_len = nkf_o_ptr - nkf_o_ptr;
    printf("From=%s To=%s i_len=%ld i_step=%ld\n", from, to, i_len, i_step);
    test_assert(
      "iconv() last result: Output string",
      !memcmp(org_o_buf, nkf_o_buf, min(org_o_len, nkf_o_len)),
      0
    );
    test_assert(
      "iconv() last result: Output string length",
      org_o_len == nkf_o_len,
      0
    );
  }
}

