# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [0.1.0] - 2018-02-16
### Added
- CHANGELOG.md: Created;
- version.c: Created, added version string;
- args.c: -V and --version options;
- organize.c: Fallback using stat(3) st_mtime when EXIF read fail or EXIF does
  not exits;

### Changed
- args.c: Fixed software name;
- organize.c: Try stat(3) if EXIF fail;
