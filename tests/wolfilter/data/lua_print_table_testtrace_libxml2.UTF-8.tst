**
**requires:LUA
**requires:LIBXML2
**input
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE invoice SYSTEM 'http://www.wolframe.org/example-doc/invoice.dtd'>
<invoice>
<order>
<number>INS03-62</number>
<reference>PO#6123</reference>
<representative>happyHamster1</representative>
<terms>45</terms>
<issuedate>2013/03/21</issuedate>
<duedate>2013/05/05</duedate>
<description>swimming pool supplies and accessoires</description>
</order>
<item>
<name>Bermuda TF 3 Pool Cleaner</name>
<description>Advanced Suction Pool Cleaning Robot</description>
<quantity>1</quantity>
<discount>0</discount>
<price>
<unit>789.00</unit>
<total>789.00</total>
<tax description="MWST 3.5%">27.60</tax>
<gross>816.60</gross>
</price>
</item>
<item>
<name>Blue Ocean Pool Lounge - Luxury edition</name>
<description>Swimming Seat</description>
<quantity>2</quantity>
<discount>10</discount>
<price>
<unit>79.30</unit>
<total>142.70</total>
<tax description="MWST 3.5%">5.00</tax>
<gross>147.70</gross>
</price>
</item>
<item>
<name>Pooltoyland Happy Duckburg Family</name>
<description>Disney Rubber Ducks</description>
<quantity>4</quantity>
<discount>0</discount>
<price>
<unit>3.50</unit>
<total>14.00</total>
<tax description="MWST 3.5%">0.50</tax>
<gross>14.50</gross>
</price>
</item>
<item>
<name>Beach Ball 24"</name>
<description>Inflatable Ball</description>
<quantity>2</quantity>
<discount>0</discount>
<price>
<unit>1.95</unit>
<total>3.90</total>
<tax description="MWST 3.5%">0.15</tax>
<gross>4.05</gross>
</price>
</item>
<item>
<name>Drink Holder Tahiti Sunset</name>
<description>Drink holder</description>
<quantity>6</quantity>
<discount>0</discount>
<price>
<unit>3.80</unit>
<total>22.80</total>
<tax description="MWST 3.5%">0.80</tax>
<gross>23.60</gross>
</price>
</item>
<bill>
<price>
<total>972.40</total>
<tax>34.05</tax>
<gross>1006.45</gross>
</price>
<payed>0.00</payed>
<open>1006.45</open>
</bill>
<address id='supplier'>
<name>The Consumer Company</name>
<street>Kreuzstrasse 34</street>
<postalcode>8008</postalcode>
<city>&#252;rich</city>
<country>Switzerland</country>
</address>
<address id='customer'>
<name>The Consumer Company</name>
<street>Kreuzstrasse 34</street>
<postalcode>8008</postalcode>
<city>&#252;rich</city>
<country>Switzerland</country>
</address>
<address id='shipping'>
<name>The Manufacturer Company</name>
<street>Bahnhofstrasse 2</street>
<postalcode>8001</postalcode>
<city>&#252;rich</city>
<country>Switzerland</country>
</address>
</invoice>**config
--input-filter xml:libxml2 --output-filter xml:libxml2 --printlayout print_invoice.tracepdf:invoice.simplepdf --form invoice.simpleform --script print_table.lua run
**file: print_table.lua

function run()
	f = filter( "xml:textwolf")
	f.empty = false
	input:as( f)
	output:as( filter( "blob"))
	t = input:table()
	f = formfunction( "print_invoice")
	output:print( f( t))
end

**file: invoice.simpleform
!DOCTYPE "invoice 'http://www.wolframe.org/example-doc/invoice.dtd'"
invoice
{
	order
	{
		number string
		reference string
		representative string
		terms uint
		issuedate string
		duedate string
		description string
	}
	item []
	{
		name string
		description string
		quantity uint
		discount float
		price
		{
			unit float
			total float
			tax
			{
				description @string
				_ float
			}
			gross float
		}
	}
	bill
	{
		price
		{
			total float
			tax float
			gross float
		}
		payed float
		open float
	}
	address []
	{
		id @string
		name string
		street string
		postalcode uint
		city string
		country string
	}
}

**file: invoice.simplepdf
/invoice: {Text="invoice"} PrintText()
//name: {Text ?= "text"; [Index] ?= -1; [Index] = Index + 1} PrintText()
**output
ENTER PrintText:
1$ Text = 'invoice'
ENTER PrintText:
2$ Text = 'invoice', Index = '2.00000'
LEAVE PrintText 2
ENTER PrintText:
3$ Text = 'invoice', Index = '3.00000'
LEAVE PrintText 3
ENTER PrintText:
4$ Text = 'invoice', Index = '4.00000'
LEAVE PrintText 4
ENTER PrintText:
5$ Text = 'invoice', Index = '2.00000'
LEAVE PrintText 5
ENTER PrintText:
6$ Text = 'invoice', Index = '3.00000'
LEAVE PrintText 6
ENTER PrintText:
7$ Text = 'invoice', Index = '4.00000'
LEAVE PrintText 7
ENTER PrintText:
8$ Text = 'invoice', Index = '5.00000'
LEAVE PrintText 8
ENTER PrintText:
9$ Text = 'invoice', Index = '6.00000'
LEAVE PrintText 9
LEAVE PrintText 1
**end
