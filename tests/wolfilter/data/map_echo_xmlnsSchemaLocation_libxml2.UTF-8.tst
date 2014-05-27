**
**requires:LIBXML2
**input
<?xml version="1.0" encoding="UTF-8"?>
<!-- this example has been copied from http://www.w3schools.com/schema/schema_schema.asp -->
<note xmlns="http://www.w3schools.com" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.w3schools.com note.xsd"><to>Tove</to><from>Jani</from><heading>Reminder</heading><body>Don't forget me this weekend!</body></note>**config
--input-filter libxml2 --output-filter libxml2 --module ../../src/modules/filter/libxml2/mod_filter_libxml2 - 
**output
<?xml version="1.0" encoding="UTF-8"?>
<note xmlns="http://www.w3schools.com" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.w3schools.com note.xsd"><to>Tove</to><from>Jani</from><heading>Reminder</heading><body>Don't forget me this weekend!</body></note>
**end
