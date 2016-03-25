#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Author: Ping Zeng(zengping@ncic.ac.cn)
#
# some helper function to generate rsa keys and encrypt or decrypt files
# TODO current only support text format encrypt

from Crypto.PublicKey import RSA
from Crypto import Random
import random
import sys

def generateRSAPublicPrivateKeys(key_len=2048, privkey='privkey.rsa', pubkey='publickey.rsa'):
  """Generate RSA keys

  @param key_len: the rsa password length
  @param privkey: the path which save the private key
  @param pubkey: the path which save the public key
  """
  key = RSA.generate(key_len, Random.new().read)

  with open(privkey, 'w') as fp:
    fp.write(key.exportKey())
  with open(pubkey, 'w') as fp:
    fp.write(key.publickey().exportKey())

def encrypt(pubkey_path, file_path, output_path, mode="r"):
  """Encrypt file with PublicKey

  @param pubkey_path: the publickey path
  @param file_path: the file need to be encrypted
  @param mode: the file open mode
  @param output_path: the output file path
  """
  key = RSA.importKey(open(pubkey_path).read())
  with open(file_path, mode) as fp:
    message = "".join(fp.readlines())

  ciphertext = key.encrypt(message, random.randint(1,100))[0]

  with open(output_path, 'wb') as fp:
    fp.write(ciphertext)

def decrypt(prikey_path, file_path, output_path, mode='w'):
  """Decrypt file with Private Key

  @param prikey_path: the private key path
  @param file_path: the file need to be encrypted
  @param output_path: the output file path
  @param mode: the output file mode
  """
  key = RSA.importKey(open(prikey_path).read())
  with open(file_path, "rb") as fp:
    ciphertext = "".join(fp.readlines())

  message = key.decrypt(ciphertext)

  with open(output_path, mode) as fp:
    fp.write(message)

if __name__ == "__main__":
  #generateRSAPublicPrivateKeys(key_len=2048)
  encrypt("./publickey.rsa", "./input", "ciphertext")
  decrypt("./privkey.rsa", "./ciphertext", "output")
