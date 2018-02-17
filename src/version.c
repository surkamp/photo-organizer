/*-
 * BSD 2-Clause License
 *
 * Copyright (c) 2017, Sérgio Surkamp
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "version.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

const char* version( void )
{
#define VERSION_BUFF_SIZE 64
  static char version_buff[VERSION_BUFF_SIZE];
  static const bool version_cached = false;

  if( ! version_cached )
  {
    int version_buff_print_size = 0;
    memset( version_buff, 0, VERSION_BUFF_SIZE );
    version_buff_print_size = snprintf( version_buff, VERSION_BUFF_SIZE,
      "%" PRIu8 ".%" PRIu8 ".%" PRIu16, VERSION_MAJOR, VERSION_MINOR,
      VERSION_PATCH );

    if( version_buff_print_size < 0 )
    {
      fprintf( stderr, "snprintf(3) failed: (%d) %s", errno, strerror( errno ) );
    }
  }
  return version_buff;
#undef VERSION_BUFF_SIZE
}
