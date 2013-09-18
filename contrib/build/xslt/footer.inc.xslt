<?xml version="1.0" encoding="ISO-8859-1" ?>
<xsl:stylesheet xmlns="http://www.w3.org/1999/xhtml"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:template name="footer">
  <xsl:param name="base" select="/page/@base"/>
    
    <div id="footer">
      <div id="footersections">
        <div class="half">
          <h2>Footer area #1</h2>
          <p>This area uses the div class="half". You can replace it with two div:s if you give them class="quarter" to get a 4-column footer, or replace the two following div:s with one div with class="lasthalf" to get a 2-column footer. In this file, I have combined the "half", "quarter" and "lastquarter" classes.</p>
        </div>

        <div class="quarter">
          <h2>Footer area #2</h2>
          <p>This area uses the div class="quarter".</p>
          <p>Paragraphs and <a href="#">links</a> work here too.</p>
        </div>

        <div class="lastquarter">
          <h2>Footer menu</h2>
          <ul>
            <li><a href="index.html">Link #1</a></li>
            <li><a href="index.html">Link #2</a></li>
          </ul>
        </div>
        <div class="clear">&#160;</div> 
      </div>
    </div>

    <div id="credits">
      <p>&#xa9; 2011 - 2013 <a href="http://wolframe.org">Project Wolframe</a><br/>
      <span class="small">Template design by <a href="http://andreasviklund.com/">Andreas Viklund</a></span></p>
    </div>

</xsl:template>
  
</xsl:stylesheet>
