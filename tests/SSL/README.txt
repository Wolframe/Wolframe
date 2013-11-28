SSL test files for Wolframe

Generate a CA certificate:
  openssl req -config openssl.cnf -days 1825 -x509 -newkey rsa:2048 \
              -out CA.cert.pem -keyout CA.key.pem -outform PEM

Generate a certificate request (for wolframe):
  openssl req -config openssl.cnf -new -nodes -keyout wolframed.key \
              -out wolframed.req -days 365

Sign a certificate (for wolframe):
  openssl ca -config openssl.cnf -policy policy_anything -out wolframed.crt \
             -infiles wolframed.req
NOTE: If you want a certificate valid for multiple hostnames, IPs ...,
create a file having subjectAltName=.... and sign the certificate 
with -extfile filename. The openssl.cnf must have req_extensions = v3_req uncommented
See http://www.openssl.org/docs/apps/x509v3_config.html#Subject_Alternative_Name_
for more information.

Delete the unnecessary files: xx.pem (xx means numeric), *.old *.req


All passwords are xxyy.

wolframed, wolframedpwd and WolfClient are signed with CA
client is signed with CAclient

To build the CA directory copy / link CA certificates to the CA directory
and then run:
  c_rehash <dir> on that directory

-------------------------------------------------------------------------------

List of files:

README.txt		this file

CA.cert.pem		Wolframe CA certificate (valid till Feb 28 23:59:47 2016 GMT)
CA.key.pem		Wolframe CA key
openssl.cnf		configuration file used in the commands above

ClientCA.cert.pem	another CA certificate (valid till Feb 29 00:21:29 2016 GMT)
ClientCA.key.pem	the key for the other certificate
ClientCA.cnf		similar to openssl.cnf, used with the Client CA

index.txt
index.txt.attr
serial			files used for certicate generation

CAchain.pem		CA chain file
CAdir			CA directory

client.crt		client certificate, signed with CAclient.cert
client.key		key for client.crt

WolfClient.crt		client certificate signed with CA.cert
WolfClient.key		key for Wolfclient

wolframed.crt		server certificate (signed with CA.cert)
wolframed.key		key for wolframed.crt

wolframedpwd.crt	server certificate (signed with CA.cert)
wolframedpwd.key	key for wolframedpwd.crt, password protected
