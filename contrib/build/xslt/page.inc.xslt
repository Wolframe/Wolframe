<?xml version="1.0" encoding="ISO-8859-1" ?>
<xsl:stylesheet xmlns="http://www.w3.org/1999/xhtml"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<!-- debug only: find unmappd XML, diable in production
     TODO: react to page/@debug flag or similar
  -->
<!--
<xsl:include href="default.inc.xslt"/>
-->

<xsl:include href="header.inc.xslt"/>
<xsl:include href="footer.inc.xslt"/>

<xsl:template name="page">
  <xsl:param name="base" select="@base"/>
  <xsl:param name="title"/>

  <html>

    <xsl:call-template name="header">
      <xsl:with-param name="base" select="$base"/>
      <xsl:with-param name="title" select="$title"/>
    </xsl:call-template>

    <div id="containerfull"> 

      <div id="header">
        <h1><a href="index.html">Project Wolframe</a></h1>
        <h2>Bugreporting...</h2>
      </div>

      <div id="menu">
        <ul>   
          <li><a class="current" href="index-main.html">Home</a></li>
          <li><a href="download.html">Downloads</a></li>
          <li><a href="documentation.html">Documentation</a></li>
          <li><a href="news.html">News</a></li>
          <li><a href="support.html">Support</a></li>
        </ul>
      </div>

      <div id="feature">
        <div class="left">
          <h2>Wolframe Application Server</h2>
          <p>This area can be used to put something special in focus. Products, a presentation or anything you may want to highlight. If not needed, this area can be removed completely.</p>
        </div>

        <div class="right">
          <h2>Wolframe Server Development Kit</h2>
          <p>Use div class="left" and "right" for two columns, or remove the two extra div:s to use a single column here.</p>
        </div>
        <div class="clear">&#160;</div>
      </div>

      <div id="main">
        <div id="sidebar">
          <div class="sidebarbox">
            <h2>Sidebar menu</h2>
            <ul class="sidemenu">
              <li><a href="index.html">Sidebar link</a></li>
              <li><a href="#">Subpages supported</a>
                <ul>
                  <li><a href="index.html">Subpage one</a></li>
                  <li><a href="examples.html">Subpage two</a></li>
                </ul></li>
              <li><a href="#">Last page</a></li>
            </ul>
          </div>

          <div class="sidebarbox">
            <h2>Text box</h2>
            <p>This is a sidebar text box. It can be used for regular links:</p>
            <ul>
              <li><a href="http://andreasviklund.com/templates/">More free templates</a></li>
              <li><a href="http://andreasviklund.com/blog/">Andreas' blog</a></li>
            </ul>
          </div>
        </div>

        <div id="content">
          <xsl:apply-templates/>
          <div class="clear">&#160;</div>
        </div>
            
        <div class="clear">&#160;</div>

      </div>
    </div>

    <body>
      <xsl:call-template name="footer">
        <xsl:with-param name="base" select="$base"/>
      </xsl:call-template>
    </body>

  </html>

</xsl:template>

</xsl:stylesheet>
