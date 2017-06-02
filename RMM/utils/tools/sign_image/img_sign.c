#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <openssl/bio.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#define HASH_STRING_LEN	(65)
#define SIG_LEN			(256)
#define PRIV_KEY_LEN	(2048)

unsigned char calc_hash[HASH_STRING_LEN];
FILE *raw_file, *new_file;

char *priv_key = "-----BEGIN PRIVATE KEY-----\nMIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQCuuV0HpPEtflGa\nEsuosRXstIYhbjw2q7U/jEmVsTQqiGAhXchrrEO6PQmAwIoBi0VEIcHkeYwCWCrp\nvFtu4ENTbP++z3wZcQfVnPQz3hkMmkWd32x4YMc45Eo8xR0ozcBete9oue+AGFhp\nYG7m79J5pL0l+UeHecKHUdJBj+2Xsqp3oCTV29pUhyGhNSRrF2FMeJqifWQNTOmv\nyT/ogzAlQkJiKNcl4QZVgCiABGwrJ4yjQT1b2N1J0Q1pvLZxjusNZIP2R+KeeCdx\ngCvdDAM8yC1O+GnONbCWQa6hHlwo6xhqC8CaepzJgnvnXphAYXi4+GDYtY5sljqB\nCbD33Tk7AgMBAAECggEACbDoUws4AVD3T7dpKmMgFy/wHJyTpGijtfGfBzl77jwA\nMPmVqzszynmmNEj5nppN7jh9G6IOPPcM8kfrPlwb/92U1c2GUZbuW0/IT7Qq6YMJ\nB3ej+Y1Mn4AyCerY8jFvURGAROvDN/Shur6JiC+XGf4kK54G9KGxQ3YrVk2YXQiJ\nUfzCaOOLEBNOE/mBcfUX1K+U1Zq7F6fl88ohN2CilBIRpS6GQvPxlWuvjv2CwyH1\ncMKMGHMwnRey8Or8ryw/wpqkRHWGzr24tjCPLk1DW6neLe8Ag4aCOwebBQ/8FlNh\nYLZMa2rh5nOHOqP168YgnKduicpRFT2ijNjtT8SpuQKBgQDoEZIGLWIb03z9nVpw\nVtswRqwUhlMwq1WJtlbdVD0/M+VSSDUGUroYGFEsv4hXYqeR6m4CKgTCdSGzKEsk\nPnPL3PtUhgm8GNrzjCrXyN5pTmWIA2+GuStpUZ+9YKHMgl24cUpp8NHLspMOv3yo\nl0yGbUpxinQIUSbcP0QX42Mi3QKBgQDAvfETfC5AHZJVpV8TVVd2IGQdg4LRiH1+\nj7simKZtsFBaAUqMwmBKVMHk9OCG1CPSPyEQBgDSM4b+8IdcYfG3QARqBrAm7PZ4\ntWydDOSX0tUrirHF9XzPFS0+CkIGuERLDG1Pf84MyaW2YysR1gKtyT8j+BByERoq\nvcJY32CO9wKBgHHRxEvRUHATXxdIaMX2Y5Se5OFALGcPcWmUi7AME+n+uveLQK50\nPp94tb97xRQO597yGrhnAWSabEfheFEzbDnNjVscToP5sNGIqq6g7yy47aUzAi0S\nfo8uIGmSjmiue+btcbDaExuT3vp5wlsxsa+wxLVBUb2Oh7+4K96HgOTlAoGBAITT\nYQu4jHcK6gmzyW2dZoS1lcjxFHopwbIygxg4cpks5HLiaOnQGmOfPW/3gbPpBjah\n4rsUKAS0FdgjEmWL6fQLXf9jVIm+y6wr+wnQFMi8NsOm9RQjNTvQsDkNAKt42TeO\n/NXIkcsmO9kRRmEoRvhUs/qpvUuelZXW59QvPG/JAoGBAMm0LSVvDGLcZI9yw9KQ\nT56UwxEh2r6NL2u3NC5+5/PjlHKF02jA+qDSQmA5M30efBAJNhaA2+MZ8PZXdR54\n0JNrLWWRz8cbOqLhGmTMZ4lrJJ5311gTzYzSqYe8Gosx69qKMAtKe9BVAXI0WvsT\nvO/vZ6CNNGg3WIeGrB1lxoGX\n-----END PRIVATE KEY-----\n";

void sha256_hash_string (unsigned char hash[SHA256_DIGEST_LENGTH], unsigned char outputBuffer[HASH_STRING_LEN])
{
	int i = 0;

	for(i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
	}

	outputBuffer[HASH_STRING_LEN - 1] = 0;
}

int calc_sha256 (void)
{
	const int bufSize = 256;
	int bytesRead = 0;
	unsigned char hash[SHA256_DIGEST_LENGTH];
	unsigned char* buffer;

	if (!raw_file) return -1;

	buffer = malloc(bufSize);
	if(!buffer) return -1;

	SHA256_CTX sha256;
	SHA256_Init(&sha256);


	while((bytesRead = fread(buffer, 1, bufSize, raw_file)))
	{
		SHA256_Update(&sha256, buffer, bytesRead);
	}
	SHA256_Final(hash, &sha256);

	sha256_hash_string(hash, calc_hash);
	free(buffer);
	fseek(raw_file, 0L, SEEK_SET);
	return 0;
}

int main(int argc, char *argv[])
{
	unsigned char* signature;
	unsigned int slen;
	RSA *private_key = NULL;
	BIO *priv_bio;
	unsigned char buffer[SIG_LEN];

	if (argc < 3)
	{
		printf("Usage: %s image_file signed_file\n", argv[0]);
		exit(-1);
	}

	raw_file = fopen(argv[1], "r");
	new_file = fopen(argv[2], "w");
	if (!raw_file || !new_file)
	{
		fprintf(stderr, "Open file error\n");
		exit(-1);
	}

	priv_bio = BIO_new_mem_buf(priv_key, -1);
	if(priv_bio == NULL)
	{
		fprintf(stderr, "priv_bio is null\n");
		exit(-1);
	}

	private_key = PEM_read_bio_RSAPrivateKey(priv_bio, NULL, NULL, NULL);
	if(private_key == NULL)
	{
		fprintf(stderr, "private_key is null\n");
		exit(-1);
	}

	signature = (unsigned char*) malloc(RSA_size(private_key));
	if (!signature)
	{
		fprintf(stderr, "memory allocation error\n");
		exit(-1);
	}

	if (calc_sha256() != 0)
	{
		fprintf(stderr, "calc_sha256 error\n");
		exit(-1);
	}
	if(RSA_sign(NID_sha256, (unsigned char*) calc_hash, strlen(calc_hash),
				signature, &slen, private_key) != 1)
	{
		fprintf(stderr, "RSA_sign error\n");
	}

	fwrite(signature, SIG_LEN, 1, new_file);

	int bytesRead = 0;
	while((bytesRead = fread(buffer, 1, SIG_LEN, raw_file)))
	{
		fwrite(buffer, bytesRead, 1, new_file);
	}

	/* free resouces */
	return 0;
}
