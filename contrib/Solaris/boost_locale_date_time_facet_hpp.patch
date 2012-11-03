--- boost/locale/date_time_facet.hpp.orig	2012-11-02 13:16:50.000000000 +0100
+++ boost/locale/date_time_facet.hpp	2012-11-02 13:16:57.000000000 +0100
@@ -51,7 +51,7 @@
                     second,                     ///< second [0..59]
                     week_of_year,               ///< The week number in the year
                     week_of_month,              ///< The week number within current month
-                    first_day_of_week,          ///< First day of week, constant, for example Sunday in US = 1, Monday in France = 2
+                    first_day_of_week           ///< First day of week, constant, for example Sunday in US = 1, Monday in France = 2
                 };
 
             } // marks
@@ -133,7 +133,7 @@
                 least_maximum,      ///< The last maximal value for this period, For example for Gregorian calendar
                                     ///< day it is 28
                 actual_maximum,     ///< Actual maximum, for it can be 28, 29, 30, 31 for day according to current month
-                absolute_maximum,   ///< Maximal value, for Gregorian day it would be 31.
+                absolute_maximum    ///< Maximal value, for Gregorian day it would be 31.
             } value_type;
 
             ///
@@ -141,7 +141,7 @@
             ///
             typedef enum {
                 move,   ///< Change the value up or down effecting others for example 1990-12-31 + 1 day = 1991-01-01
-                roll,   ///< Change the value up or down not effecting others for example 1990-12-31 + 1 day = 1990-12-01
+                roll    ///< Change the value up or down not effecting others for example 1990-12-31 + 1 day = 1990-12-01
             } update_type;
 
             ///
