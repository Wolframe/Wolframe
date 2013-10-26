<?xml version='1.0'?>
<xsl:stylesheet
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	version="1.0">

<xsl:import href="http://docbook.sourceforge.net/release/xsl/current/xhtml/chunk.xsl"/>

	<xsl:param name="html.stylesheet" select="'/layout/styles/main.css'"/>
	<xsl:param name="section.autolabel" select="1"/>
	<xsl:param name="section.label.includes.component.label" select="1"/>
	<xsl:param name="section.autolabel.max.depth" select="2"/>

	<xsl:param name="titlepage.pubdate" select="1"/>

	<xsl:template name="user.head.content">
		<xsl:variable name="codefile" select="document('head-webpage.html',/)"/>
		<xsl:copy-of select="$codefile/htmlcode/node()"/>
	</xsl:template>

	<xsl:template name="user.header.navigation">
		<xsl:variable name="codefile" select="document('header-webpage.html',/)"/>
		<xsl:copy-of select="$codefile/htmlcode/node()"/>
		<xsl:text disable-output-escaping='yes'>&lt;div class="wrapper row3"></xsl:text>
		<xsl:text disable-output-escaping='yes'>&lt;div id="container"></xsl:text>
	</xsl:template>

	<xsl:template name="user.footer.navigation">
		<xsl:text disable-output-escaping='yes'>&lt;/div></xsl:text>
		<xsl:text disable-output-escaping='yes'>&lt;/div></xsl:text>
		<xsl:variable name="codefile" select="document('footer-webpage.html',/)"/>
		<xsl:copy-of select="$codefile/htmlcode/node()"/>
	</xsl:template>
	
</xsl:stylesheet>
