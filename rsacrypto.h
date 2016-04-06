#ifndef _RSACRYPTO_H_
#define _RSACRYPTO_H_

// rsa encrypt and decrypt
//
// Author: Ping Zeng(zengping10212044@gmail.com)
//
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <stdio.h>

static int padding = RSA_NO_PADDING;

static RSA * createRSAFromFile(unsigned char *filename, int public)
{
  RSA *rsa= NULL;
  BIO *keybio ;
  keybio = BIO_new(BIO_s_file());
  BIO_read_filename(keybio, filename);
  if (keybio==NULL)
  {
    printf( "Failed to create key BIO");
    return 0;
  }
  if(public)
  {
    rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);
  }
  else
  {
    rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa,NULL, NULL);
  }
  if(rsa == NULL)
  {
    printf( "Failed to create RSA");
  }

  return rsa;

}

static RSA * createRSA(unsigned char * key,int public)
{
  RSA *rsa= NULL;
  BIO *keybio ;
  keybio = BIO_new_mem_buf(key, -1);
  if (keybio==NULL)
  {
    printf( "Failed to create key BIO");
    return 0;
  }
  if(public)
  {
    rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);
  }
  else
  {
    rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa,NULL, NULL);
  }
  if(rsa == NULL)
  {
    printf( "Failed to create RSA");
  }

  return rsa;
}

static int public_encrypt(unsigned char * data,int data_len, RSA* rsa, unsigned char *encrypted)
{
  int result = RSA_public_encrypt(data_len,data,encrypted,rsa,padding);
  return result;
}

static int private_decrypt(unsigned char * enc_data,int data_len, RSA* rsa, unsigned char *decrypted)
{
  int  result = RSA_private_decrypt(data_len,enc_data,decrypted,rsa,padding);
  return result;
}

static void printLastError(char *msg)
{
  char * err = malloc(130);;
  ERR_load_crypto_strings();
  ERR_error_string(ERR_get_error(), err);
  printf("%s ERROR: %s\n",msg, err);
  free(err);
}

#endif
