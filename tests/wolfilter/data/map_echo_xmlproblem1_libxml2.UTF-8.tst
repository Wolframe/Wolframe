**
**requires:LIBXML2
**input
<?xml version="1.0" encoding="UTF-8"?><!DOCTYPE category SYSTEM 'CategoryHierarchyRequest.simpleform'><category state="" id="1"/>**config
--input-filter libxml2 --output-filter libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2 - 
**requires:TEXTWOLF
**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<category state="" id="1"/>
**end
