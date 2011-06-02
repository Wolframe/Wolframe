
Available targets:

make [all]	       create all artifacts
make test	       create test binaries and execute tests and execute
                       fast tests
make longtest          execute all tests, including long lasting ones
make clean             clean up build artifacts
make distclean         clean up all generated artifacts
make install           install (set 'DESTDIR' and 'prefix' at will)
make uninstall         uninstall (set 'DESTDIR' and 'prefix' at will)
make dist[-Z|-gz|-bz2] create tarball containing all sources
make help              show this very help page
make config            show the configuration used during build
make init-po           create initial version of the gettext files
make merge-po          merge the gettext files after changes
make check-po          check sanity of gettext files

Available optional features:

WITH_SSL=1             use OpenSSL additionally for communication encryption
WITH_LUA=1             build code using the Lua interpreter
WITH_QT=1              build code depending on Qt
WITH_SASL=1            build authentication code with Cyrus SASL2
WITH_PAM=1             build authentication code with PAM
WITH_SQLITE3=1         build AAAA and DB connectors for Sqlite3
WITH_PGSQL=1           build AAAA and DB connectors for PostgreSql
WITH_EXAMPLES=0        do not build and test the examples (default is on)

Avaliable optional features during testing only:

WITH_EXPECT=1          use Expect/Tcl for system testing

Example:
make WITH_SSL=1 WITH_EXPECT=1 WITH_QT=1 WITH_PAM=1 WITH_SASL=1 \
     WITH_SQLITE3=1 WITH_PGSQL=1 WITH_LUA=1

To build the documentation call:

cd docs; make help
