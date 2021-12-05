#include <argp.h>
#include <error.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <inttypes.h>

#include "serpent.h"

static char doc[] = "Serpent block cipher encrypter and decrypter\
\vReads data block by block from STDIN and writes processed blocks\
 to STDOUT. CBC mode is default, but ECB may be specified as well.";

static struct argp_option options[] = {
	{ "encrypt", 'e', "MODE", OPTION_ARG_OPTIONAL, "Perform encryption (*CBC, ECB)" },
	{ "decrypt", 'd', "MODE", OPTION_ARG_OPTIONAL, "Perform decryption (*CBC, ECB)" },
	{ 0,0,0,0, "Key options:" },
	{ "key",     'k', "KEY",  0, "Use KEY (hex-encoded) as secret key" },
	{ "keyfile", 'f', "FILE", 0, "Use up to 256 bits of FILE as secret key"},
  	{ 0 }
};

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

static char parse_mode(char *modestr) {
	if (!strcasecmp(modestr, "ecb"))
		return MODE_ECB;
	if (!strcasecmp(modestr, "cbc"))
		return MODE_CBC;
	
	fprintf(stderr, "warning: mode `%s' unrecognized. defaulting to CBC.\n", modestr);
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

static struct argp argp = { options, parse_option, NULL, doc };

union key_material {
	uint32_t words[8];
	uint8_t  bytes[32];
};

void process_key(char key_type, char *key_data, uint32_t full_key[8]) {
	switch(key_type) {
	case KEY_TYPE_HEX:		
		size_t len = strlen(key_data);
		if (len % 8 || len == 0)
			error(1, 0, "please specify a positive multiple of 8 hex digits for KEY");

		size_t i = 0;
		for (char *p = key_data + len - 8; i < 8 && p >= key_data; ++i, p -= 8)
			sscanf(p, "%8" PRIx32, full_key + i);
		if (i < 8) full_key[i] |= 1;
	}
}

int main(int argc, char **argv) {
	struct arguments args = {
		.operation = OP_NONE,
		.mode      = MODE_CBC,
		.key_type  = KEY_TYPE_NONE
	};

	argp_parse(&argp, argc, argv, 0, 0, &args);
	if (args.operation == OP_NONE)
		error(1, 0, "you must select either encryption or decryption");
	if (args.key_type == KEY_TYPE_NONE)
		error(1, 0, "you must pass a key to the program");


	uint32_t full_key[8] = { 0 };
	process_key(args.key_type, args.key_data, full_key);

	

	return 0;
}



// int main() {
// 	uint32_t kkey[8] = {
// 		0x9bd95acd, 0xfb668394, 0xeb4c9210, 0xed976378,
// 		0x47c45751, 0xf20de6f1, 0x2e98b4c5, 0x59d13854
// 	};

// 	uint32_t input[4] = { 0xfc994152, 0xb42fce88, 0x9df67c5a, 0xd260d4e9 };
// 	uint32_t output[4];

// 	uint32_t round_keys[33][4];
// 	generate_subkeys(kkey, round_keys);

// 	encrypt_block(input, output, round_keys);
// 	for (size_t i = 0; i < 4; ++i)
// 		printf("%08x ", output[i]);
// 	printf("\n");

// 	uint32_t decrypted[4];
// 	decrypt_block(output, decrypted, round_keys);
// 	for (size_t i = 0; i < 4; ++i)
// 		printf("%08x ", decrypted[i]);
// 	printf("\n");

// 	return 0;
// }