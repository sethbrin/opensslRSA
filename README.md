opensslRSA
--------------
using openssl rsa to encrypt and decrypt text file, current only support key size of 2048,
and only support pem key

Usage
--------------
You can use `python pycrypto.py` to generate pem key, and for the detail usage use `rsa -h`.

```
 make
 rsa -h
 rsa -i input -o cypher -k pubkey // encrypt a text file
 rsa -d -i cypher -o output -k prikey // decrypt a text file
 python pycrypto.py
```
