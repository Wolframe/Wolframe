
Available targets:

make [all]                 create all artifacts
make test                  create test binaries and execute fast tests
make longtest              execute all tests, including long lasting ones
make testreport            execute tests as with 'test', then create a report
make clean                 clean up build artifacts
make distclean             clean up all generated artifacts
make install               install (set 'DESTDIR' and 'prefix' at will)
make uninstall             uninstall (set 'DESTDIR' and 'prefix' at will)
make dist[-Z|-gz|-bz2|-xz] create tarball containing all sources
make help                  show this very help page
make config                show the configuration used during build
make depend                build dependency files (extension .d)
make init-po               create initial version of the gettext files
make merge-po              merge the gettext files after changes
make check-po              check sanity of gettext files

Available optional features:

WITH_SSL=1              use OpenSSL additionally for communication encryption
WITH_LUA=1              build bindings and modules for the Lua interpreter
WITH_PYTHON=1           build bindings and modules for Python
WITH_SASL=1             build authentication code with Cyrus SASL2
WITH_PAM=1              build authentication code with PAM
WITH_SYSTEM_SQLITE3=1   build AAAA and DB connectors for Sqlite3 (from system) 
WITH_LOCAL_SQLITE3=1    force building of local amalgataion version of Sqlite3
WITH_PGSQL=1            build AAAA and DB connectors for PostgreSql
WITH_ORACLE=1           build AAAA and DB connectors for Oracle
WITH_TEXTWOLF=1         build code using textwolf
WITH_LIBXML2=1          build code using libxml2
WITH_LIBXSLT=1          build code using libxslt (requires also libxml2)
WITH_SYSTEM_LIBHPDF=1   build code for creating PDFs with LibHpdf (from system)
WITH_LOCAL_LIBHPDF=1    force building of local version of LibHpdf
WITH_ICU=1              build code using International Comp. for Unicode
WITH_SYSTEM_FREEIMAGE=1 build modules using FreeImage, for image manipulation
WITH_LOCAL_FREEIMAGE=1  build modules using FreeImage (using local library)
WITH_CJSON=1            build filtering module for JSON with cjson
DEFAULT_MODULE_LOAD_DIR=<dir> set module load directory for loadable modules

Some more obscure options:

WITH_EXAMPLES=0         do not build and test the examples (default is on)
ENABLE_NLS=0            Don't build gettext NLS support (default is on)

Avaliable optional features during testing only:

RELEASE=1               build using release compiler and linker flags
RUN_TESTS=0             don't automatially run tests (default is run them)
WITH_EXPECT=1           use Expect/Tcl for system testing

Example:
make WITH_SSL=1 WITH_EXPECT=1 WITH_PAM=1 WITH_SASL=1 \
     WITH_SYSTEM_SQLITE3=1 WITH_PGSQL=1 WITH_LUA=1 WITH_LIBXML2=1 \
     WITH_LIBXSLT=1 WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 WITH_LOCAL_FREEIMAGE=1 \
     WITH_PYTHON=1 WITH_CJSON=1 WITH_TEXTWOLF=1

To build the documentation call:

cd docs; make help
