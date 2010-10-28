void ControlConnection::handle_ssl_handshake
		(const boost::system::error_code& error)
	{
	if(!error) {
		syslog(LOG_INFO,
			   "SSL handshake for new incoming connection was successful");

		// Extract the common name from the client cert
		SSL* ssl = _ssl_sock.impl()->ssl;
		X509* peer_cert;
		char buf[2048];
		memset(buf, 0, 2048);
		peer_cert = SSL_get_peer_certificate(ssl);
		int x = X509_NAME_get_text_by_NID(
					X509_get_subject_name(peer_cert),
					NID_commonName,
					buf,
					2047);
		if(x != -1)	{
			std::string cn = buf;
			if(cn == "<hardcoded-hostname>") {
				_writebuffer = "Greetings\r\n";
				boost::asio::async_write(
					_ssl_sock, 
					boost::asio::buffer(m_writebuffer),
					boost::bind(
						&ControlConnection::handle_write_greeting,
						shared_from_this(),
						boost::asio::placeholders::error));
				}
			else {
				syslog(LOG_ERR,
					   "New connection rejected: "
					   "CommonName in client certificate is not"
					   " \"<hardcoded-hostname>\", but \"%s-\"",
					   cn.c_str());
				_ssl_sock.lowest_layer().close();
				}
			}
		else {
			syslog(LOG_ERR,
				   "New connection rejected: "
				   "No CommonName in peer certificate");
			_ssl_sock.lowest_layer().close();
			}

		}
	else {
		syslog(LOG_ERR,
			   "New connection failed, error on SSL handshake: %s",
			   error.message().c_str());
		_ssl_sock.lowest_layer().close();
		}
	}
=================


