# photo-organizer
Photo organization tool using EXIF

This software is a command line tool to organize photos by reading the EXIF
shooting date and time, create appropriate directory (current name format is
Year-Month-Day) and move the photos to it's respective directory.

## Known issues
* Current directory files processing only (files must be on current directory)
* No fallback to filesystem object creation date time when no EXIF information
  is available
* Unable to move files between different mount points

## Dependencies
* C compiler that is c99 standard complaint
* [libexif](https://github.com/libexif/libexif)
