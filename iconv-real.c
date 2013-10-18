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

#include <pthread.h>

#include "iconv-real.h"

static pthread_mutex_t iconv_real_lock = PTHREAD_MUTEX_INITIALIZER;

int
iconv_real_init(
  void
) {
  static volatile int iconv_real_initialized = 0;

  if (iconv_real_initialized) {
    return 0;
  }

  pthread_mutex_lock(&iconv_real_lock);

  /* FIXME */

  iconv_real_initialized = 1;
  pthread_mutex_unlock(&iconv_real_lock);

  return 0;
}

