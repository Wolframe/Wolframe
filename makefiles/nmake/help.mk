
Available targets:

nmake [all]            create all artifacts
nmake test             create test binaries and execute tests and execute
                       fast tests
nmake longtest         execute all tests, including long lasting ones
nmake doc              build the documentation
nmake clean            clean up build artifacts
nmake distclean        clean up all generated artifacts
nmake help             show this very help page

Available optional features:

WITH_SSL=1             use OpenSSL additionally for communication encryption
WITH_LUA=1             build code depending on Lua
WITH_PYTHON=1          build bindings and modules for Python
WITH_SQLITE3=1         build AAAA and DB connectors for Sqlite3
WITH_PGSQL=1           build AAAA and DB connectors for PostgreSql
WITH_TEXTWOLF=1        build code using textwolf
WITH_LIBXML2=1         build code using libxml2
WITH_LIBXSLT=1         build code using libxslt (requires also libxml2)
WITH_EXAMPLES=1        build and test the examples
WITH_LIBHPDF=1         build code for creating PDFs with LibHpdf
WITH_ICU=1             build code using ICU (icu4c)
WITH_FREEIMAGE=1       build modules using FreeImage
WITH_CJSON =1          build filtering module for JSON with cjson

Avaliable optional features during testing only:

DEBUG=1                build using debug compiler and linker flags
WITH_EXPECT=1          use Expect/Tcl for system testing

Example:
nmake /nologo /f Makefile.W32 WITH_SSL=1 WITH_EXPECT=1 WITH_LUA=1
                              WITH_SQLITE3=1 WITH_PGSQL=1 WITH_LIBXML2=1
                              WITH_LIBXSLT=1 WITH_LIBHPDF=1 WITH_EXAMPLES=1
                              WITH_ICU=1 WITH_FREEIMAGE=1 WITH_PYTHON=1
                              WITH_CJSON=1 WITH_TEXTWOLF=1
                              
