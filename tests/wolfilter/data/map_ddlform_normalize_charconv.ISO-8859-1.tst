**
**requires:DISABLED
**input
<?xml version="1.0" encoding="ISO-8859-1" standalone="no"?>
<!DOCTYPE invoice SYSTEM 'typed_invoice.simpleform'>
<invoice>
<order>
<number>INS03-62</number>
<reference>PO#6123</reference>
<representative> happyHamster1 </representative>
<terms> 45 </terms>
<issuedate>2013/03/21</issuedate>
<duedate>2013/05/05</duedate>
<description>Übliche Zusätze und Gerätschaften für öffentliche Schwimmbäder</description>
</order>
<item>
<name>äÄöÖüÜïÏëËÿŸ� éè</name>
<description></description>
<quantity>1</quantity>
<discount>0</discount>
<price>
<unit>789.00</unit>
<total>789.00</total>
<tax description="MWST 3.5%">27.60</tax>
<gross>816.60</gross>
</price>
</item>
<bill>
<price>
<total>789.00</total>
<tax>27.60</tax>
<gross>781.40</gross>
</price>
<payed>0.00</payed>
<open>781.40</open>
</bill>
<address id='supplier'>
<name> Züliefärär </name>
<street>Überlandwäg</street>
<postalcode>8008</postalcode>
<city>Zürich</city>
<country>Switzerland</country>
</address>
<address id='customer'>
<name>Abnähmer</name>
<street>Öttestrasse zwölf</street>
<postalcode>8008</postalcode>
<city>Zürich</city>
<country>Switzerland</country>
</address>
<address id='shipping'>
<name>Hersteller</name>
<street>Hölderlinwäg 13</street>
<postalcode>8001</postalcode>
<city>Zürich</city>
<country>Switzerland</country>
</address>
</invoice>**config
--input-filter textwolf --output-filter textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf -c wolframe.conf typed_invoice
**requires:TEXTWOLF
**file:wolframe.conf
LoadModules
{
	module ./../../src/modules/ddlcompiler/simpleform/mod_ddlcompiler_simpleform
	module ./../../src/modules/normalize/number/mod_normalize_number
	module NOTFOUND:normalize_locale
	module ./../../src/modules/normalize/string/mod_normalize_string
}
Processor
{
	program ../wolfilter/scripts/typed_invoice.sfrm
	program ../wolfilter/template/program/simpleform_complex.wnmp
}
**output
<?xml version="1.0" encoding="ISO-8859-1" standalone="no"?>
<!DOCTYPE invoice SYSTEM 'http://www.wolframe.org/example-doc/typed_invoice.dtd'>
<invoice>
<order>
<number>INS03-62</number>
<reference>PO#6123</reference>
<representative> happyHamster1 </representative>
<terms> 45 </terms>
<issuedate>2013/03/21</issuedate>
<duedate>2013/05/05</duedate>
<description>Uebliche Zusaetze und Geraetschaften fuer oeffentliche Schwimmbaeder</description>
</order>
<item>
<name>aAoOuUiIeEyYaee</name>
<description></description>
<quantity>1</quantity>
<discount>0</discount>
<price>
<unit>789.00</unit>
<total>789.00</total>
<tax description="MWST 3.5%">27.60</tax>
<gross>816.60</gross>
</price>
</item>
<bill>
<price>
<total>789.00</total>
<tax>27.60</tax>
<gross>781.40</gross>
</price>
<payed>0.00</payed>
<open>781.40</open>
</bill>
<address id='supplier'>
<name> Zueliefaeraer </name>
<street>Ueberlandwaeg</street>
<postalcode>8008</postalcode>
<city>Zurich</city>
<country>Switzerland</country>
</address>
<address id='customer'>
<name>Abnaehmer</name>
<street>Aettestrasse zwoelf</street>
<postalcode>8008</postalcode>
<city>Zurich</city>
<country>Switzerland</country>
</address>
<address id='shipping'>
<name>Hersteller</name>
<street>Hoelderlinwaeg 13</street>
<postalcode>8001</postalcode>
<city>Zuerich</city>
<country>Switzerland</country>
</address>
</invoice>
**end
