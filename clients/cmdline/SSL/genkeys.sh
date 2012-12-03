openssl req -config openssl.cnf -days 1825 -x509 -newkey rsa:2048 -out CA.cert.pem -keyout CA.key.pem -outform PEM
openssl req -config openssl.cnf -new -nodes -keyout wolframed.key -out wolframed.req -days 365
openssl ca -config openssl.cnf -policy policy_anything -out wolframed.crt -infiles wolframed.req
