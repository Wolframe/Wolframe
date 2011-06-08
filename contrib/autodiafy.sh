#!/bin/sh

AUTODIA='autodia.pl -l c++'
$AUTODIA -z -o database.png -i \
	"include/database/DBreference.hpp include/database/PostgreSQL.hpp include/database/SQLite.hpp include/database/database.hpp"
