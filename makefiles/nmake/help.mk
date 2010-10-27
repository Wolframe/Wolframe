
Available targets:

make [all]	       create all artifacts
make test	       create test binaries and execute tests
make doc               build the documentation
make clean             clean up build artifacts
make distclean         clean up all generated artifacts
make help              show this very help page

Available optional features:

WITH_SSL=1             use OpenSSL additionally for communication encryption
WITH_BOOST_LOG=1       use embedded Boost Log library instead of a fake logger
WITH_LUA=1             generate LUA handler

Example:
nmake /nologo /f Makefile.W32 WITH_SSL=1 WITH_BOOST_LOG=1 WITH_LUA=1
