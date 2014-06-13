Install test SSL client certificate on Windows

1) Create a Windows SSL client certificate:
      openssl pkcs12 -inkey wolframed.key -in wolframed.crt -name wolframe -passout pass:wolframe -export -out wolframed.pfx

2) Install the certificate: Filemanager right mouse click on wolframed.pfc -> "Install certificate"





