
Available targets:

make [all]	       create all artifacts
make test	       create test binaries and execute tests and execute
                       fast tests
make longtest          execute all tests, including long lasting ones
make doc               build the documentation
make clean             clean up build artifacts
make distclean         clean up all generated artifacts
make help              show this very help page

Available optional features:

WITH_SSL=1             use OpenSSL additionally for communication encryption
WITH_LUA=1             build code depending on Lua
WITH_QT=1              build code depending on Qt
WITH_SQLITE3=1         build AAAA and DB connectors for Sqlite3
WITH_PGSQL=1           build AAAA and DB connectors for PostgreSql

Avaliable optional features during testing only:

WITH_EXPECT=1          use Expect/Tcl for system testing

Example:
nmake /nologo /f Makefile.W32 WITH_SSL=1 WITH_GTEST=1 WITH_EXPECT=1 WITH_LUA=1
                              WITH_QT=1 WITH_SQLITE3=1 WITH_PGSQL=1
