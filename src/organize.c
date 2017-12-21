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

#include "organize.h"
#include "config.h"

#include <libexif/exif-data.h>
#include <libexif/exif-utils.h>
#include <libexif/exif-loader.h>

#include <stdio.h>
#include <glob.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

typedef unsigned char buffer_type;

struct exif_lookup
{
  ExifTag tag;
  ExifIfd ifd;
};

static void organize_format_date( const buffer_type* value, buffer_type* buff, size_t buff_size )
{
  size_t chr = 0;
  buffer_type* buff_ptr = buff;

  do
  {
    if( *value == ':' )
    {
      *buff_ptr = '-';
    }
    else
    {
      *buff_ptr = *value;
    }

    ++chr;
    ++value;
    ++buff_ptr;
  } while( *value != '\0' && *value != ' ' && chr < buff_size );

  *buff_ptr = '\0';
}

static bool organize_read_date_from_exif( const char* filename, buffer_type* buff, size_t buff_size )
{
  // lookup table sorted by preference
  static const struct exif_lookup lookup_table[] =
  {
      { EXIF_TAG_DATE_TIME_ORIGINAL, EXIF_IFD_EXIF }
    , { EXIF_TAG_DATE_TIME_DIGITIZED, EXIF_IFD_EXIF }
    , { EXIF_TAG_DATE_TIME, EXIF_IFD_0 }
    , { 0, EXIF_IFD_COUNT }
  };

  const struct exif_lookup* info = lookup_table;
  ExifLoader* loader = NULL;
  ExifData* data = NULL;
  ExifEntry* entry = NULL;

  loader = exif_loader_new( );
  exif_loader_write_file( loader, filename );
  data = exif_loader_get_data( loader );
  exif_loader_unref( loader );

  if( data == NULL )
  {
    fprintf( stderr, "Unable to read EXIF data from file %s\n", filename );
    return false;
  }

  do
  {
    entry = exif_content_get_entry( data->ifd[ info->ifd ], info->tag );

    // only ASCII format is acceptable...
    if( entry != NULL
      && entry->format == EXIF_FORMAT_ASCII
      && entry->size < ( buff_size - 1 ) )
    {
      // exif_entry_dump( entry, 0 );

      organize_format_date( entry->data, buff, buff_size );
      exif_data_unref( data );
      return true;
    }

    exif_entry_unref( entry );
    ++info;
  } while( info->ifd != EXIF_IFD_COUNT );

  exif_data_unref( data );

  return false;
}

static bool organize_create_directory( const char* directory )
{
  struct stat statbuff;

  if( lstat( directory, &statbuff ) == 0 )
  {
    if( ! S_ISDIR( statbuff.st_mode ) )
    {
      fprintf( stderr, "%s exists and is not a directory\n", directory );
      return false;
    }

    // directory already exists, great!
    return true;
  }
  else if( errno != ENOENT )
  {
    fprintf( stderr, "lstat(2) failed: (%d) %s\n", errno, strerror( errno ) );
    return false;
  }

  if( config.dry_run )
  {
    return true;
  }

  if( config.verbose )
  {
    printf( "mkdir %s\n", directory );
  }

  if( mkdir( directory, 0755 ) < 0 )
  {
    fprintf( stderr, "mkdir(2) failed: (%d) %s\n", errno, strerror( errno ) );
    return false;
  }

  return true;
}

static bool organize_move( const char* filename, const char* directory )
{
  static const size_t buff_size = 1024;
  char dst[ buff_size ];
  size_t buff_p_size = 0;

  buff_p_size = snprintf( dst, buff_size, "%s/%s", directory, filename );
  if( buff_p_size < 0 )
  {
    fprintf( stderr, "snprintf(3) failed: (%d) %s\n", errno, strerror( errno ) );
    return false;
  }
  else if( buff_p_size >= buff_size )
  {
    fprintf( stderr, "path overflow" );
    return false;
  }

  if( config.verbose )
  {
    printf( "'%s' -> '%s'\n", filename, dst );
  }

  if( config.dry_run )
  {
    return true;
  }

  if( rename( filename, dst ) != 0 )
  {
    fprintf( stderr, "rename(2) failed: (%d) %s\n", errno, strerror( errno ) );
    return false;
  }

  return true;
}

bool organize( const char* pattern )
{
  glob_t myglob;

  if( glob( pattern, GLOB_NOSORT | GLOB_TILDE_CHECK, NULL, &myglob ) != 0 )
  {
    globfree( &myglob );
    return false;
  }

  static const size_t date_buff_size = 64;
  char date_buff[ date_buff_size ];
  const char* curr = NULL;

  for( size_t match = 0; match < myglob.gl_pathc; ++match )
  {
    curr = *( myglob.gl_pathv + match );
    if(
      ! organize_read_date_from_exif( curr, (buffer_type*)date_buff, date_buff_size )
      || ! organize_create_directory( date_buff )
      || ! organize_move( curr, date_buff ) )
    {
      globfree( &myglob );
      return false;
    }
  }

  globfree( &myglob );
  return true;
}
