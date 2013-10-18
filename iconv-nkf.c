/*
  iconv(3)-compatible API and $LD_PRELOAD-able library by NKF
  Copyright (c) 2013 SATOH Fumiyasu @ OSS Technology Corp., Japan

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
  claim that you wrote the original software. If you use this software
  in a product, an acknowledgment in the product documentation would be
  appreciated but is not required.

  2. Altered source versions must be plainly marked as such, and must not be
  misrepresented as being the original software.

  3. This notice may not be removed or altered from any source distribution.
 */

#include <stdio.h>
#include <pthread.h>
#include <errno.h>

#include "iconv-nkf.h"
#include "iconv-real.h"

#define CONST_DISCARD(type, ptr)	((type) ((void *) (ptr)))

#undef getc
#define getc(f)         iconv_nkf_getc(f)

#undef putchar
#define putchar(c)      iconv_nkf_putchar(c)

#undef TRUE
#undef FALSE

static char *iconv_nkf_inbuf, *iconv_nkf_outbuf;
static char *iconv_nkf_inptr, *iconv_nkf_outptr;
static size_t iconv_nkf_inbytesleft,iconv_nkf_outbytesleft;
static int iconv_nkf_guess_flag;
static int iconv_nkf_errno;

static int
iconv_nkf_getc(FILE *f)
{
  unsigned char c;

  if (iconv_nkf_inbytesleft) {
    c = *iconv_nkf_inptr++;
    iconv_nkf_inbytesleft--;
    return (int)c;
  }

  return EOF;
}

static void
iconv_nkf_putchar(int c)
{
  if (iconv_nkf_guess_flag) {
    return;
  }

  if (iconv_nkf_outbytesleft) {
    *iconv_nkf_outptr++ = c;
    iconv_nkf_outbytesleft--;
  }
  else {
    /* FIXME: Set error flag */
  }
}

#define PERL_XS 1
#define iconv iconv_x
#include "nkf-dist/utf8tbl.c"
#include "nkf-dist/nkf.c"
#undef iconv

typedef struct {
  const char *name;
  const char *in_option;
  const char *out_option;
} iconv_nkf_encoding_options_struct;

static iconv_nkf_encoding_options_struct
nkf_encoding_options_by_name[] = {
  {"UTF-8",		"-W",			"-w"},
  {"UTF8",		"-W",			"-w"},
  {"UTF-8-MAC",		"--ic=UTF-8-MAC",	NULL},
  {"UTF8-MAC",		"--ic=UTF-8-MAC",	NULL},
  {"ISO-2022-JP",	"-J",			"-j"},
  {"ISO2022JP",		"-J",			"-j"},
  {"EUC-JP",		"-E",			"-e"},
  {"EUCJP",		"-E",			"-e"},
  {"EUC-JP-MS",		"--ic=EUC-JP-MS",	"--oc=EUCJP-MS"},
  {"EUC-JP-NKF",	"--ic=EUC-JP-NKF",	"--oc=EUCJP-NKF"},
  {"EUCJP-NKF",		"--ic=EUC-JP-NKF",	"--oc=EUCJP-NKF"},
  {"Shift_JIS",		"-S",			"-s"},
  {"Shift-JIS",		"-S",			"-s"},
  {"SJIS",		"-S",			"-s"},
  {"CP932",		"--ic=CP932",		"--oc=CP932"},
  {NULL,		NULL,			NULL}
};

static iconv_nkf_encoding_options_struct *
iconv_nkf_encoding_options(
  const char *name
) {
  int i;
 
  for (i = 0; nkf_encoding_options_by_name[i].name; i++) {
    if (strcasecmp(nkf_encoding_options_by_name[i].name, name) == 0) {
      return &nkf_encoding_options_by_name[i];
    }
  }

  return NULL;
}

iconv_nkf_t
iconv_nkf_open(
  const char *to,
  const char *from
) {
  iconv_nkf_t cd;
  iconv_nkf_encoding_options_struct *nkf_encoding_options;

  cd = malloc(sizeof(*cd));
  if (!cd) {
    return (iconv_nkf_t)-1;
  }

  cd->iconv_cd = (iconv_real_t)-1;

  nkf_encoding_options = iconv_nkf_encoding_options(from);
  if (!nkf_encoding_options || !nkf_encoding_options->in_option) {
    goto err;
  }
  cd->nkf_in_option = nkf_encoding_options->in_option;

  nkf_encoding_options = iconv_nkf_encoding_options(to);
  if (!nkf_encoding_options || !nkf_encoding_options->out_option) {
    goto err;
  }
  cd->nkf_out_option = nkf_encoding_options->out_option;

  return cd;

err:
#ifdef ICONV_NKF_PRELOADLIB
  if (iconv_real_init() != 0) {
    free(cd);
    errno = EINVAL;
    return (iconv_nkf_t)-1;
  }

  cd->iconv_cd = (iconv_real_t)iconv_real_open(to, from);
  if (cd->iconv_cd == (iconv_real_t)-1) {
    free(cd);
    return (iconv_nkf_t)-1;
  }

  return cd;
#else
  errno = EINVAL;

  return (iconv_nkf_t)-1;
#endif
}

int iconv_nkf_close(
  iconv_nkf_t cd
) {
  if (cd->iconv_cd != (iconv_real_t)-1) {
    iconv_real_close(cd->iconv_cd);
  }

  free(cd);

  return 0;
}


size_t iconv_nkf(
  iconv_nkf_t cd,
  char **inbuf, size_t *inbytesleft,
  char **outbuf, size_t *outbytesleft
) {
  static pthread_mutex_t iconv_nkf_lock = PTHREAD_MUTEX_INITIALIZER;

  if (cd->iconv_cd != (iconv_real_t)-1) {
    return iconv_real(cd->iconv_cd, inbuf, inbytesleft, outbuf, outbytesleft);
  }

  if (inbuf == NULL || *inbuf == NULL || outbuf == NULL || *outbuf == NULL) {
    /* FIXME */
    return 0;
  }

  pthread_mutex_lock(&iconv_nkf_lock);

  iconv_nkf_inbuf = iconv_nkf_inptr = *inbuf;
  iconv_nkf_inbytesleft = *inbytesleft;
  iconv_nkf_outbuf = iconv_nkf_outptr = *outbuf;
  iconv_nkf_outbytesleft = *outbytesleft;
  *iconv_nkf_outptr = '\0';
  iconv_nkf_guess_flag = 0;
  iconv_nkf_errno = 0;

  reinit();
  /* Disable JIS X 0201->0208 conversion and MIME decoding */
  options((unsigned char *)"-xm0");
  options(CONST_DISCARD(unsigned char *, cd->nkf_in_option));
  options(CONST_DISCARD(unsigned char *, cd->nkf_out_option));
  kanji_convert(NULL);

  *iconv_nkf_outptr = '\0';

  *inbuf = iconv_nkf_inptr;
  *inbytesleft -= iconv_nkf_inptr - iconv_nkf_inbuf;
  *outbuf = iconv_nkf_outptr;
  *outbytesleft -= iconv_nkf_outptr - iconv_nkf_outbuf;

  if (iconv_nkf_errno) {
    /* FIXME */
  }

  pthread_mutex_unlock(&iconv_nkf_lock);

  return 0;
}

