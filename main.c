/* 20211205 Oliver Emery (University of Iowa) CS:4980
 * 
 * Cryptographic frontend/CLI for my Serpent implementation. I used GNU argp to
 * help with argument parsing.
 * 
 * Compile on any recent Linux distro with
 *  $ gcc -o serpent main.c serpent.c
 * 
 * Run
 *  $ ./serpent --help
 * to see available options.
 * 
 * Example:
 *  $ dd if=/dev/random of=keyfile bs=32 count=1
 *  $ dd if=/dev/random of=input bs=512 count=4
 *  $ ./serpent --encrypt=CBC --keyfile=keyfile < input > input.enc
 *  $ ./serpent --decrypt=CBC --keyfile=keyfile < input.enc > input.dec
 *  $ cmp input input.dec
 * 
 * This demo supports two modes: CBC and ECB. I cheaped out on the IV and made it
 * constant 0s. As this is a frontend for a raw block cipher, it only processes
 * 128-bit blocks. I have it set to pad the last block with 0s if it isn't aligned
 * to a 16-byte boundary. However, decrypting will leave the added padding in the
 * plaintext. To change this behavior, I would most likely need to implement some sort
 * of header with metadata including length.
 */

#include <argp.h>
#include <error.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "serpent.h"

#define OP_NONE		0
#define OP_ENCRYPT	1
#define OP_DECRYPT	2

#define MODE_CBC	1
#define MODE_ECB	2

#define KEY_TYPE_NONE	0
#define KEY_TYPE_HEX	1
#define KEY_TYPE_FILE	2

struct arguments {
	char operation, mode, key_type;
	char *key_data;
};

static char help_text[] = "Serpent block cipher encrypter and decrypter"
	"\vReads data block by block from STDIN and writes processed blocks "
 	"to STDOUT. CBC mode is default; alternatively, ECB may be specified.";

static struct argp_option options[] = {
	{ "encrypt", 'e', "MODE", OPTION_ARG_OPTIONAL, "Perform encryption (CBC, ECB)" },
	{ "decrypt", 'd', "MODE", OPTION_ARG_OPTIONAL, "Perform decryption (CBC, ECB)" },
	{ 0,0,0,0, "Key options:" },
	{ "key",     'k', "KEY",  0, "Use KEY (hex literal) as secret key" },
	{ "keyfile", 'f', "FILE", 0, "Use up to 256 bits of FILE as secret key"},
  	{ 0 }
};

static char parse_mode(char *modestr) {
	if (!strcasecmp(modestr, "ecb"))
		return MODE_ECB;
	if (!strcasecmp(modestr, "cbc"))
		return MODE_CBC;
	
	fprintf(stderr, "Warning: Mode `%s' unrecognized. Defaulting to CBC.\n", modestr);
	return MODE_CBC;
}

static error_t parse_option(int key, char *arg, struct argp_state *state) {
	struct arguments *arguments = state->input;

	switch (key) {
	case 'e':
		arguments->operation = OP_ENCRYPT;
		arguments->mode = arg ? parse_mode(arg) : MODE_CBC;
		break;
	case 'd':
		arguments->operation = OP_DECRYPT;
		arguments->mode = arg ? parse_mode(arg) : MODE_CBC;
		break;
	case 'k':
		arguments->key_type = KEY_TYPE_HEX;
		arguments->key_data = arg;
		break;
	case 'f':
		arguments->key_type = KEY_TYPE_FILE;
		arguments->key_data = arg;
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}

	return 0;
}

/* Load key from command line argument: hex literal or keyfile */
void process_key(char key_type, char *key_data, uint32_t full_key[8]) {
	switch (key_type) {
	case KEY_TYPE_HEX:		
		size_t len = strlen(key_data);
		if (len % 8 || len == 0)
			error(1, 0, "KEY must have a positive multiple of 8 hex digits");

		size_t i = 0;
		for (char *p = key_data + len - 8; i < 8 && p >= key_data; ++i, p -= 8)
			sscanf(p, "%8" PRIx32, full_key + i);
		if (i < 8) full_key[i] |= 1;
		break;

	case KEY_TYPE_FILE:
		int fd = open(key_data, O_RDONLY);
		if (fd < 0)
			error(1, errno, "Unable to open `%s'", key_data);

		struct stat kf_stat;
		if (fstat(fd, &kf_stat) < 0)
			error(1, errno, "Unable to stat `%s'", key_data);

		if (kf_stat.st_size < 32)
			error(1, 0, "Keyfile must contain at least 32 bytes of data");
		
		if (read(fd, full_key, 32) < 32)
			error(1, errno, "Unable to read `%s'", key_data);
		
		close(fd);
		break;
	}
}

#define xor_blocks(l, r) \
	l[0] ^= r[0]; l[1] ^= r[1]; l[2] ^= r[2]; l[3] ^= r[3];

/* performs the target operation continuously on STDIN */
void process_input(char operation, char mode, uint32_t user_key[8]) {
	subkey_t subkeys;
	generate_subkeys(user_key, subkeys);

	ssize_t nr;
	uint32_t input[4], output[4] = { 0 }, chain[4] = { 0 };

	// Read 16 bytes from STDIN at a time. Could increase speed better buffering
	while ((nr = read(STDIN_FILENO, input, 16)) > 0) {
		// poor man's padding
		if (nr < 16) memset(((char*)input) + nr, 0, 16 - nr);

		if (operation == OP_ENCRYPT) {
			if (mode == MODE_CBC) { xor_blocks(input, output) }
			encrypt_block(input, output, subkeys);
		} else {
			decrypt_block(input, output, subkeys);
			if (mode == MODE_CBC) {
				xor_blocks(output, chain)
				memcpy(chain, input, 16);
			}
		}
		write(STDOUT_FILENO, output, 16);
	}
}

int main(int argc, char **argv) {
	struct argp argp = { options, parse_option, NULL, help_text };
	struct arguments args = {
		.operation = OP_NONE,
		.mode      = MODE_CBC,
		.key_type  = KEY_TYPE_NONE
	};

	argp_parse(&argp, argc, argv, 0, 0, &args);
	if (args.operation == OP_NONE)
		error(1, 0, "encryption or decryption must be selected");
	if (args.key_type == KEY_TYPE_NONE)
		error(1, 0, "a key is reuired to operate");


	uint32_t full_key[8] = { 0 };
	process_key(args.key_type, args.key_data, full_key);
	process_input(args.operation, args.mode, full_key);

	return 0;
}
