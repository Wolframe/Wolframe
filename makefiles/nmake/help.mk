
Available targets:

nmake [all]	       create all artifacts
nmake test	       create test binaries and execute tests and execute
                       fast tests
nmake longtest         execute all tests, including long lasting ones
nmake doc              build the documentation
nmake clean            clean up build artifacts
nmake distclean        clean up all generated artifacts
nmake help             show this very help page

Available optional features:

WITH_SSL=1             use OpenSSL additionally for communication encryption
WITH_LUA=1             build code depending on Lua
WITH_QT=1              build code depending on Qt
WITH_SQLITE3=1         build AAAA and DB connectors for Sqlite3
WITH_PGSQL=1           build AAAA and DB connectors for PostgreSql
WITH_LIBXML2=1         build code using libxml2
WITH_LIBXSLT=1         build code using libxslt
WITH_MSXML=1           build with Microsoft XML Core Services support
WITH_XMLLITE=1         build with Microsoft XmlLite library support
WITH_EXAMPLES=1        build and test the examples

Avaliable optional features during testing only:

DEBUG=1                build using debug compiler and linker flags
WITH_EXPECT=1          use Expect/Tcl for system testing

Example:
nmake /nologo /f Makefile.W32 WITH_SSL=1 WITH_EXPECT=1 WITH_LUA=1
                              WITH_QT=1 WITH_SQLITE3=1 WITH_PGSQL=1
			      WITH_MSXML=1 WITH_XMLLITE=1 WITH_EXAMPLES=1
