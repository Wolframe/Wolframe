**
**input
<?xml version="1.0" encoding="ISO-8859-1" standalone="no"?>
<!DOCTYPE CATALOG SYSTEM 'CDCatalog'>
<CATALOG><CD><TITLE>Empire Burlesque</TITLE><ARTIST>Bob Dylan</ARTIST><RELEASE><COUNTRY>USA</COUNTRY><COMPANY>Columbia</COMPANY><PRICE>10.90</PRICE><YEAR>1985</YEAR></RELEASE></CD><CD><TITLE>El Rei el Cantinero</TITLE><ARTIST>Jose Alfredo Jimenez</ARTIST><RELEASE><COUNTRY>Mexico</COUNTRY><COMPANY>RCA</COMPANY><PRICE>15.90</PRICE><YEAR>1958</YEAR></RELEASE></CD><CD><TITLE>Still got the blues</TITLE><ARTIST>Gary Moore</ARTIST><RELEASE><COUNTRY>UK</COUNTRY><COMPANY>Virgin records</COMPANY><PRICE>10.20</PRICE><YEAR>1990</YEAR></RELEASE></CD><CD><TITLE>Eros</TITLE><ARTIST>Eros Ramazzotti</ARTIST><RELEASE><COUNTRY>EU</COUNTRY><COMPANY>BMG</COMPANY><PRICE>9.90</PRICE><YEAR>1997</YEAR></RELEASE></CD><CD><TITLE>One night only</TITLE><ARTIST>Bee Gees</ARTIST><RELEASE><COUNTRY>UK</COUNTRY><COMPANY>Polydor</COMPANY><PRICE>10.90</PRICE><YEAR>1998</YEAR></RELEASE></CD><CD><TITLE>Sylvias Mother</TITLE><ARTIST>Dr.Hook</ARTIST><RELEASE><COUNTRY>UK</COUNTRY><COMPANY>CBS</COMPANY><PRICE>8.10</PRICE><YEAR>1973</YEAR></RELEASE></CD><CD><TITLE>Romanza</TITLE><ARTIST>Andrea Bocelli</ARTIST><RELEASE><COUNTRY>EU</COUNTRY><COMPANY>Polydor</COMPANY><PRICE>10.80</PRICE><YEAR>1996</YEAR></RELEASE></CD></CATALOG>**config
--input-filter textwolf --output-filter textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf  --module ../../src/modules/cmdbind/directmap/mod_command_directmap --module ../../src/modules/normalize/number/mod_normalize_number --module ../../src/modules/normalize/string/mod_normalize_string --module ../../src/modules/ddlcompiler/simpleform/mod_ddlcompiler_simpleform --module ../wolfilter/modules/cmdbind/mylang/mod_command_mylang --cmdprogram=example.dmap --program=example.sfrm --program=example.wnmp --program=example.mlg getCDCatalog
**requires:TEXTWOLF
**requires:DISABLED SUSE_LINUX
**file:example.sfrm
FORM CDCatalog
{
	CATALOG
	{
		CD []
		{
			TITLE string
			ARTIST string
			RELEASE
			{
				COUNTRY string
				COMPANY string
				PRICE string
				YEAR int
			}
		}
	}
}
**file:example.wnmp
int=number:integer;
uint=number:unsigned;
float=number:float;
normname=string:convdia,lcname;
**file:example.dmap
COMMAND(get CDCatalog) CALL(run) RETURN(CDCatalog);
**file:example.mlg
run
**output
<?xml version="1.0" encoding="ISO-8859-1" standalone="no"?>
<!DOCTYPE CATALOG SYSTEM "CDCatalog.simpleform"><CATALOG><CD><TITLE>EMPIRE BURLESQUE</TITLE><ARTIST>BOB DYLAN</ARTIST><RELEASE><COUNTRY>USA</COUNTRY><COMPANY>COLUMBIA</COMPANY><PRICE>10.90</PRICE><YEAR>1985</YEAR></RELEASE></CD><CD><TITLE>EL REI EL CANTINERO</TITLE><ARTIST>JOSE ALFREDO JIMENEZ</ARTIST><RELEASE><COUNTRY>MEXICO</COUNTRY><COMPANY>RCA</COMPANY><PRICE>15.90</PRICE><YEAR>1958</YEAR></RELEASE></CD><CD><TITLE>STILL GOT THE BLUES</TITLE><ARTIST>GARY MOORE</ARTIST><RELEASE><COUNTRY>UK</COUNTRY><COMPANY>VIRGIN RECORDS</COMPANY><PRICE>10.20</PRICE><YEAR>1990</YEAR></RELEASE></CD><CD><TITLE>EROS</TITLE><ARTIST>EROS RAMAZZOTTI</ARTIST><RELEASE><COUNTRY>EU</COUNTRY><COMPANY>BMG</COMPANY><PRICE>9.90</PRICE><YEAR>1997</YEAR></RELEASE></CD><CD><TITLE>ONE NIGHT ONLY</TITLE><ARTIST>BEE GEES</ARTIST><RELEASE><COUNTRY>UK</COUNTRY><COMPANY>POLYDOR</COMPANY><PRICE>10.90</PRICE><YEAR>1998</YEAR></RELEASE></CD><CD><TITLE>SYLVIAS MOTHER</TITLE><ARTIST>DR.HOOK</ARTIST><RELEASE><COUNTRY>UK</COUNTRY><COMPANY>CBS</COMPANY><PRICE>8.10</PRICE><YEAR>1973</YEAR></RELEASE></CD><CD><TITLE>ROMANZA</TITLE><ARTIST>ANDREA BOCELLI</ARTIST><RELEASE><COUNTRY>EU</COUNTRY><COMPANY>POLYDOR</COMPANY><PRICE>10.80</PRICE><YEAR>1996</YEAR></RELEASE></CD></CATALOG>
**end
