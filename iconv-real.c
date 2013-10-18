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

#include <link.h>
#include <string.h>
#include <pthread.h>

#include "iconv-real.h"

#ifdef __mips__
#define OFFSET(map, x) ((x) + ((map)->l_addr))
#else
#define OFFSET(map, x) (x)
#endif

int
iconv_real_init(
  void
) {
  static pthread_mutex_t iconv_real_lock = PTHREAD_MUTEX_INITIALIZER;
  static int iconv_real_initialized = 0;
  static int iconv_real_status = -1;
  struct link_map *map = _r_debug.r_map;

  if (iconv_real_initialized) {
    return iconv_real_status;
  }

  pthread_mutex_lock(&iconv_real_lock);

  if (!map) {
    goto out;
  }

  for (; map; map = map->l_next) {
    if (!strstr(map->l_name, "/libc.so")) {
      continue;
    }

    ElfW(Dyn) *dyn;
    ElfW(Sym) *symtab = NULL;
    char const *strtab = NULL;
    unsigned int symnum = 0;

    for (dyn = map->l_ld; dyn->d_tag != DT_NULL; ++dyn) {
      if (dyn->d_tag == DT_SYMTAB) {
	symtab = (ElfW(Sym) *)OFFSET(map, dyn->d_un.d_ptr);
      }
      else if (dyn->d_tag == DT_STRTAB) {
	strtab = (char const *)OFFSET(map, dyn->d_un.d_ptr);
      }
      else if (dyn->d_tag == DT_HASH) {
	symnum = ((unsigned int *)OFFSET(map, dyn->d_un.d_ptr))[1];
      }
    }

    if (!symtab || !strtab || !symnum) {
      goto out;
    }

    for (; symnum; symtab++, symnum--) {
      char const *name = strtab + symtab->st_name;
      void *value = (void *)symtab->st_value + map->l_addr;

      if (!strcmp(name, "iconv_open")) {
	iconv_real_open = value;
      }
      else if (!strcmp(name, "iconv_close")) {
	iconv_real_close = value;
      }
      else if (!strcmp(name, "iconv")) {
	iconv_real = value;
      }
    }
  }

  if (iconv_real_open && iconv_real_close && iconv_real) {
    iconv_real_status = 0;
  }

out:
  iconv_real_initialized = 1;
  pthread_mutex_unlock(&iconv_real_lock);

  return iconv_real_status;
}

