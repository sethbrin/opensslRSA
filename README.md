opensslRSA
--------------
using openssl rsa to encrypt and decrypt file, current only support key size of 2048,
and only support pem key

1. rsa only support text file
2. rsa_base64 can support binary file encrypt/decrypt.

  * The encrypt procedure: file->base64->rsa
  * The decrypt procedure: file->rsa->base64

Dependency
--------------
The openssl rsa depende on base64, if the file is unicode/binary file, use base64 to encode the string

Usage
--------------
You can use `python pycrypto.py` to generate pem key, and for the detail usage use `rsa -h`.

```
 cd base64 && make
 make
 rsa -h
 rsa -i input -o cypher -k pubkey // encrypt a text file
 rsa -d -i cypher -o output -k prikey // decrypt a text file
 python pycrypto.py

 rsa_base64 -i input -o cypher -k pubkey // encrypt a text file
 rsa_base64 -d -i cypher -o output -k prikey // decrypt a text file
```
