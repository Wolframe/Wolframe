**
**requires:LIBXML2
**input
<?xml version="1.0" encoding="UTF-8"?><!DOCTYPE doc SYSTEM "CategoryHierarchyRequest.simpleform"><doc><category state="" id="1"/></doc>**config
--input-filter libxml2 --output-filter libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2 --module ../../src/modules/doctype/xml/mod_doctype_xml - 
**output
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE doc SYSTEM "CategoryHierarchyRequest.simpleform"><doc><category state="" id="1"/></doc>
**end
