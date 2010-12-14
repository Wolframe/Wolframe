
Available targets:

make [all]	       create all artifacts
make test	       create test binaries and execute tests
make doc               build the documentation
make clean             clean up build artifacts
make distclean         clean up all generated artifacts
make help              show this very help page

Available optional features:

WITH_SSL=1             use OpenSSL additionally for communication encryption

Avaliable optional features during testing only:

WITH_GTEST=1           use Google Unit testing library

Example:
nmake /nologo /f Makefile.W32 WITH_SSL=1 WITH_GTEST=1
