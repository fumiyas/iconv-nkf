#include <stdio.h>
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

int main(void) {
  int i, test = 0;
  const char *from = "UTF-8", *to;
  const char * const encodings[] = {
    "Shift_JIS", "EUC-JP", "ISO-2022-JP", "UTF-8", NULL
  };

  char i_buf[8192] = "ABCDEFG あいうえおかきくけこ \x1B XYZ わをん";
  char *i_ptr;
  size_t i_len = strlen(i_buf);
  size_t i_left;
  size_t i_step;

  iconv_t org_cd;
  int org_errno;
  char org_i_buf[8192], org_o_buf[8192];
  char *org_i_ptr, *org_o_ptr;
  size_t org_ret;
  size_t org_i_left, org_i_eaten;
  size_t org_o_left, org_o_eaten;

  iconv_nkf_t nkf_cd;
  int nkf_errno;
  char nkf_i_buf[8192], nkf_o_buf[8192];
  char *nkf_i_ptr, *nkf_o_ptr;
  size_t nkf_ret;
  size_t nkf_i_left, nkf_i_eaten;
  size_t nkf_o_left, nkf_o_eaten;

  printf("str: ");
  hexdump(i_buf, i_len);

  for (i = 0; encodings[i]; i++) {
    puts("======================================================================");
    to = encodings[i];

    for (i_step = i_len; i_step > 0; i_step--) {
      puts("----------------------------------------------------------------------");
      org_cd = iconv_open(to, from);
      org_i_ptr = org_i_buf;
      org_i_left = 0;
      org_o_ptr = org_o_buf;
      org_o_left = sizeof(org_o_buf);

      nkf_cd = iconv_nkf_open(to, from);
      nkf_i_ptr = nkf_i_buf;
      nkf_i_left = 0;
      nkf_o_ptr = nkf_o_buf;
      nkf_o_left = sizeof(nkf_o_buf);

      i_ptr = i_buf;
      i_left = i_len;
      for (; i_ptr <= i_buf + i_len; i_ptr += i_step, i_left -= min(i_step, i_left)) {
	printf("i_len=%ld i_step=%ld from=%s to=%s\n", i_len, i_step, from, to);

	memcpy(org_i_ptr + org_i_left, i_ptr, min(i_step, i_left));
	org_i_left += min(i_step, i_left);
	printf("str org in: ");
	hexdump(org_i_ptr, org_i_left);
	errno = 0;
	org_ret = iconv(org_cd, org_i_left ? &org_i_ptr : NULL, &org_i_left, &org_o_ptr, &org_o_left);
	org_errno = errno;
	org_i_eaten = org_i_ptr - org_i_buf;
	org_o_eaten = org_o_ptr - org_o_buf;

	memcpy(nkf_i_ptr + nkf_i_left, i_ptr, min(i_step, i_left));
	nkf_i_left += min(i_step, i_left);
	printf("str nkf in: ");
	hexdump(nkf_i_ptr, nkf_i_left);
	errno = 0;
	nkf_ret = iconv_nkf(nkf_cd, nkf_i_left ? &nkf_i_ptr : NULL, &nkf_i_left, &nkf_o_ptr, &nkf_o_left);
	nkf_errno = errno;
	nkf_i_eaten = nkf_i_ptr - nkf_i_buf;
	nkf_o_eaten = nkf_o_ptr - nkf_o_buf;

	printf("ret org out: 戻値=%2ld, 入力元消費=%ld, 出力先消費=%ld, errno=%d (%s)\n",
	  org_ret, org_i_eaten, org_o_eaten, org_errno, strerror(org_errno)
	);
	printf("ret nkf out: 戻値=%2ld, 入力元消費=%ld, 出力先消費=%ld, errno=%d (%s)\n",
	  nkf_ret, nkf_i_eaten, nkf_o_eaten, nkf_errno, strerror(nkf_errno)
	);
	printf("Test %4d: ", test++);
	if (org_ret == nkf_ret && org_i_eaten == nkf_i_eaten && org_o_eaten == nkf_o_eaten && org_errno == nkf_errno) {
	  puts("OK");
	}
	else {
	  puts("NG");
	}

	printf("str org: ");
	hexdump(org_o_buf, org_o_eaten);
	printf("str nkf: ");
	hexdump(nkf_o_buf, nkf_o_eaten);
	printf("Test %4d: ", test++);
	if (org_o_eaten == nkf_o_eaten && !memcmp(org_o_buf, nkf_o_buf, min(org_o_eaten, nkf_o_eaten))) {
	  puts("OK");
	}
	else {
	  puts("NG");
	}

	fflush(stdout);
      }

      iconv_close(org_cd);
      iconv_nkf_close(nkf_cd);
    }

    memcpy(i_buf, org_o_buf, org_o_eaten);
    i_len = org_o_eaten;
    from = to;
  }

  return 0;
}

