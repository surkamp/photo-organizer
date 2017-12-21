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

#include "args.h"
#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

static void args_usage( const char* fmt, ... )
  __attribute( ( format( printf, 1, 2 ) ) );

static void args_usage( const char* fmt, ... )
{
  if( fmt != NULL )
  {
    va_list ap;
    va_start( ap, fmt );
    vfprintf( stderr, fmt, ap );
    va_end( ap );
    fputc( '\n', stderr );
  }
  fprintf( stderr, "usage: photoalbum [-h|--help] [-v|--verbose] [-n|--dry-run] <glob-pattern> [glob-pattern] [...]\n" );
}

static void args_init_config( void )
{
  config.dry_run = false;
  config.verbose = false;
  config.patterns = 0;
  config.first_pattern = NULL;
}

bool args_parse( int argc, char* const argv[] )
{
  int arg = 1;

  args_init_config( );

  if( argc == 0 )
  {
    args_usage( "missing patterns" );
    return false;
  }

  for( ; arg < argc; ++arg )
  {
    const char* argstr = argv[arg];

    if( argstr[0] == '-' )
    {
      if( strcmp( "-n", argstr ) == 0
        || strcmp( "--dry-run", argstr ) == 0 )
      {
        config.dry_run = true;
        config.verbose = true;
      }
      else if( strcmp( "-v", argstr ) == 0
        || strcmp( "--verbose", argstr ) == 0 )
      {
        config.verbose = true;
      }
      else if( strcmp( "-h", argstr ) == 0
        || strcmp( "--help", argstr ) == 0 )
      {
        args_usage( NULL );
        return false;
      }
      else
      {
        args_usage( "invalid argument %s", argstr );
        return false;
      }
    }
    else
    {
      break;
    }
  }

  if( arg == argc )
  {
    args_usage( "missing patterns" );
    return false;
  }

  if( ( arg - argc ) > UINT8_MAX )
  {
    args_usage( "patterns limit is %u", UINT8_MAX );
    return false;
  }

  config.patterns = argc - arg;
  config.first_pattern = argv + arg;

  return true;
}
