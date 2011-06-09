#!/bin/sh

AUTODIA='autodia -l c++'
$AUTODIA -z -o database.png -i \
	"include/database/DBreference.hpp include/database/PostgreSQL.hpp include/database/SQLite.hpp include/database/database.hpp"

$AUTODIA -z -o logger.png -i \
	"include/singleton.hpp include/logger.hpp include/logger/logError.hpp include/logger/logSyslogFacility.hpp include/logger/logBackend.hpp include/logger/logComponent.hpp include/logger/logLevel.hpp"
