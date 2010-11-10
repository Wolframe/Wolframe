SSL test files for SMERP

Generate a CA certificate:
  openssl req -config openssl.cnf -days 1825 -x509 -newkey rsa:2048 \
              -out CA.cert.pem -keyout CA.key.pem -outform PEM

Generate a certificate request:
  openssl req -config openssl.cnf -new -nodes -keyout smerpd.key \
              -out smerpd.req -days 365

Sign a certificate
  openssl ca -config openssl.cnf -policy policy_anything -out smerpd.crt \
             -infiles smerpd.req

Delete the unnecessary files: xx.pem (xx means numeric), *.old *.req


All passwords are xxyy.

smerpd, smerpdpwd and SMERPclient are signed with CA
client is signed with CAclient

To build the CA directory copy / link CA certificates to the CA directory
and then run:
  c_rehash <dir> on that directory

-------------------------------------------------------------------------------

List of files:

README.txt		this file

CA.cert.pem		SMERP CA certificate
CA.key.pem		SMERP CA key
openssl.cnf		configuration file used in the commands above

CAclient.cert.pem	another CA certificate
CAclient.key.pem	the key for the other certificate
ClientCA.cnf		similar to openssl.cnf, used with the Client CA

index.txt
index.txt.attr
serial			files used for certicate generation

CAchain.pem		CA chain file
CAdir			CA directory

client.crt		client certificate, signed with CAclient.cert
client.key		key for client.crt

SMERPclient.crt		client certificate signed with CA.cert
SMERPclient.key		key for SMERPclient

smerpd.crt		server certificate (signed with CA.cert)
smerpd.key		key for smerpd.crt

smerpdpwd.crt		same as smerpd but password protected
smerpdpwd.key		key for smerpdpwd.crt

