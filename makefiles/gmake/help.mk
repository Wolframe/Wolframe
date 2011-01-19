
Available targets:

make [all]	       create all artifacts
make test	       create test binaries and execute tests
make doc               build the documentation
make clean             clean up build artifacts
make distclean         clean up all generated artifacts
make install           install (set 'DESTDIR' and 'prefix' at will)
make uninstall         uninstall (set 'DESTDIR' and 'prefix' at will)
make dist[-Z|-gz|-bz2] create tarball containing all sources
make help              show this very help page
make init-po           create initial version of the gettext files
make merge-po          merge the gettext files after changes
make check-po          check sanity of gettext files

Available optional features:

WITH_SSL=1             use OpenSSL additionally for communication encryption
WITH_LUA=1             build code depending on Lua
WITH_QT=1              build code depending on Qt

Avaliable optional features during testing only:

WITH_GTEST=1           use Google Unit testing library

Example:
make WITH_SSL=1 WITH_GTEST=1 WITH_LUA=1 WITH_QT=1
