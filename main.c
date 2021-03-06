//
// use rsa and base64 to encrypt and decrypt text FILE
//
// Author: Ping Zeng(zengping1021244@gmail.com)
//
//
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <getopt.h>
#include "rsacrypto.h"
#include "base64/include/libbase64.h"

// pipeline fd
static int pipedes[2];

typedef struct _args_t {
  FILE* in_file;
  FILE* out_file;
  RSA* rsa;
  int is_decrypt;
} args_t;

#define KEY_SIZE 2048
#define ENCRYPT_BUF_SIZE (KEY_SIZE/8)


static void usage(args_t *args)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "About:  encrypt and decrypt vcf file\n");
    fprintf(stderr, "Usage: rsa [option]\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "File format options:\n");
    fprintf(stderr, "  -d, --decrypt(optional)     decrypt file, default encrypt vcf\n");
    fprintf(stderr, "  -i, --input(optional)       set the input file, default is stdin\n");
    fprintf(stderr, "  -o, --output(optional)      set the out file, default is stdout\n");
    fprintf(stderr, "  -k, --key(required)         the key used to decrypt or encrypt file\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Example:\n");
    fprintf(stderr, "  rsa -k key.pub -i a.vcf -o b.vcf \n");
    fprintf(stderr, "  rsa -d -k key.pub -i a.vcf -o b.vcf \n");
    fprintf(stderr, "  cat a.vcf | rsa -k key.pub -o b.vcf \n");
    fprintf(stderr, "  cat a.vcf | rsa -k key.pub \n");
    fprintf(stderr, "\n");
    exit(-1);
}

static void encrypt_rsa(args_t* args)
{
  // each time we encrypt ENCRYPT_BUF_SIZE bytes
  //char buf[ENCRYPT_BUF_SIZE] = {};
  char writebuf[KEY_SIZE] = {};
  char buf[ENCRYPT_BUF_SIZE] = {}; //key length : 2048
  int result;

  while (1)
  {
    result = read(pipedes[0], buf, ENCRYPT_BUF_SIZE);
    if (result <= 0)
    {
      break;
    }
    if ((result = public_encrypt(buf, ENCRYPT_BUF_SIZE, args->rsa, writebuf)) != -1)
    {
      fwrite(writebuf, sizeof(char), result, args->out_file);
    }
    else
    {
      printLastError("Public Encrypt failed ");
      exit(0);
    }
    memset(writebuf, 0, sizeof(writebuf));
    memset(buf, 0, sizeof(buf));
  }
}

static void decrypt_rsa(args_t* args)
{
  // each time we decrypt KEY_SIZE bytes
  char buf[ENCRYPT_BUF_SIZE] = {};
  char writebuf[KEY_SIZE] = {};

  int result;
  int ind;
  while (1)
  {
    result = fread(buf, sizeof(char), ENCRYPT_BUF_SIZE, args->in_file);
    if (result <= 0)
    {
      break;
    }
    if ((result = private_decrypt(buf, sizeof(buf), args->rsa, writebuf)) != -1)
    {
      //fwrite(writebuf, sizeof(char), result, args->out_file);
      ind = 0;
      // remove the preceding \0's
      while (writebuf[ind++] == '\0');
      write(pipedes[1], writebuf+ind-1, strlen(writebuf)-ind+1);
    }
    else
    {
      printLastError("Private decrypt failed ");
      exit(0);
    }
    memset(writebuf, 0, sizeof(writebuf));
    memset(buf, 0, sizeof(buf));
  }

}

#define BASE64_BUFSIZE 1024 * 1024

static char base64_buf[BASE64_BUFSIZE];
static char base64_out[(BASE64_BUFSIZE * 5) / 3];	// Technically 4/3 of input, but take some margin

static int enc_base64 (FILE *fp)
{
  size_t nread;
  size_t nout;
	int ret = 1;
	struct base64_state state;

	base64_stream_encode_init(&state, 0);

	while ((nread = fread(base64_buf, 1, BASE64_BUFSIZE, fp)) > 0) {
		base64_stream_encode(&state, base64_buf, nread, base64_out, &nout);
		if (nout) {
			write(pipedes[1], base64_out, nout);
		}
		if (feof(fp)) {
			break;
		}
	}
	if (ferror(fp)) {
		fprintf(stderr, "read error\n");
		ret = 0;
		goto out;
	}
	base64_stream_encode_final(&state, base64_out, &nout);

	if (nout) {
		write(pipedes[1], base64_out, nout);
	}
out:	fclose(fp);
	return ret;
}

static int dec_base64 (FILE *fp)
{
  size_t nread;
  size_t nout;
	int ret = 1;
	struct base64_state state;

	base64_stream_decode_init(&state, 0);

	while ((nread = read(pipedes[0], base64_buf, BASE64_BUFSIZE)) > 0) {
		if (!base64_stream_decode(&state, base64_buf, nread, base64_out, &nout)) {
			fprintf(stderr, "decoding error\n");
			ret = 0;
			goto out;
		}
		if (nout) {
			fwrite(base64_out, nout, 1, fp);
		}
		if (feof(fp)) {
			break;
		}
	}
	if (ferror(fp)) {
		fprintf(stderr, "read error\n");
		ret = 0;
	}
out:	fclose(fp);
	return ret;
}


int main(int argc, char *argv[])
{
  args_t args;
  args.in_file = stdin;
  args.out_file = stdout;
  args.rsa = NULL;
  args.is_decrypt = 0;
  char* key_file = NULL;

  int c;
  static struct option loptions[] =
  {
    {"help",no_argument, NULL,'h'},
    {"decrypt", required_argument, NULL, 'd'},
    {"output", required_argument, NULL, 'o'},
    {"input", required_argument, NULL, 'i'},
    {"key", required_argument, NULL, 'k'},
    {NULL,0,NULL,0}
  };
  while((c = getopt_long(argc, argv, "h?d?i:?o:?k:", loptions, NULL)) != -1)
  {
    switch(c)
    {
    case 'i':
      args.in_file = fopen(optarg, "r");
      if (NULL == args.in_file)
      {
        fprintf(stderr, "Can not open input file %s\n", optarg);
        exit(1);
      }
      break;
    case 'o':
      args.out_file = fopen(optarg, "w");
      if (NULL == args.out_file)
      {
        fprintf(stderr, "Can not create output file %s\n", optarg);
        exit(1);
      }
      break;
    case 'k':
      key_file = optarg;
      break;
    case 'd':
      args.is_decrypt = 1;
      break;
    default:
      usage(&args);
    }
  }
  // check if key is set
  if (NULL == key_file)
  {
    fprintf(stderr, "Error, You should set the key file\n");
    usage(&args);
  }
  if (args.is_decrypt)
  {
    args.rsa = createRSAFromFile(key_file, 0);
  }
  else
  {
    args.rsa = createRSAFromFile(key_file, 1);
  }
  if(NULL == args.rsa)
  {
    printLastError("Invalid key file");
    exit(1);
  }

  pid_t pid;
  pipe(pipedes);

  if (args.is_decrypt)
  {
    pid = fork();
    if (pid > 0)
    {
      close(pipedes[0]); // close read
      decrypt_rsa(&args);
      close(pipedes[1]);
    }
    else
    {
      close(pipedes[1]); //close write
      dec_base64(args.out_file);
      close(pipedes[0]);
    }
  }
  else
  {
    pid = fork();
    if (pid > 0)
    {
      close(pipedes[0]); // close read
      enc_base64(args.in_file);
      close(pipedes[1]);
    }
    else
    {
      close(pipedes[1]); //close write
      encrypt_rsa(&args);
      close(pipedes[0]);
    }
  }

  waitpid(pid, NULL, 0);

  return 0;
}
