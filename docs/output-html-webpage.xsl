<?xml version='1.0'?>
<!DOCTYPE doc [
<!ENTITY nbsp "&#xa0;">
<!ENTITY copy "&#169;">
]>
<xsl:stylesheet
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	version="1.0">

<xsl:import href="http://docbook.sourceforge.net/release/xsl/current/xhtml/chunk.xsl"/>

	<xsl:param name="html.stylesheet" select="'css/style.css'"/>
	<xsl:param name="section.autolabel" select="1"/>
	<xsl:param name="section.label.includes.component.label" select="1"/>
	<xsl:param name="section.autolabel.max.depth" select="2"/>

	<xsl:param name="titlepage.pubdate" select="0"/>
	
	<xsl:template name="user.footer.content">
		<!-- footer -->
		<div  id="footer">
			<span class="shadow-bottom"></span>
			<div class="footer-bottom">
				<div class="shell">
					<nav class="footer-nav">
						<p class="copy">&copy; Copyright 2013 Project Wolframe<span>|</span>Based on a <a href="http://chocotemplates.com" target="_blank">ChocoTemplates.com</a> design</p>
					</nav>
				</div>
			</div>
		</div>
	</xsl:template>

</xsl:stylesheet>
<!--	stringparam user.header.content "<a href=\"/\">Back to main page</a>" \ -->
