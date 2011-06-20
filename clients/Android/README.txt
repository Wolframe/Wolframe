Create a keystore using:
keytool -genkey -v -keystore Wolframe.keystore -alias WolframeKey -keyalg RSA -keysize 2048 -validity 10000
The keystore is necessary only for building a release. Debug keyes are generated automatically.

Compile with:
ant debug
    or
ant release

Clean with:
ant clean

