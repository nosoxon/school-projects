#include <stdio.h>
#include <stdint.h>

#include "serpent.h"

int main() {
	uint32_t kkey[8] = {
		0x9bd95acd, 0xfb668394, 0xeb4c9210, 0xed976378,
		0x47c45751, 0xf20de6f1, 0x2e98b4c5, 0x59d13854
	};

	uint32_t input[4] = { 0xfc994152, 0xb42fce88, 0x9df67c5a, 0xd260d4e9 };
	uint32_t output[4];

	uint32_t round_keys[33][4];
	generate_subkeys(kkey, round_keys);

	encrypt_block(input, output, round_keys);
	for (size_t i = 0; i < 4; ++i)
		printf("%08x ", output[i]);
	printf("\n");

	uint32_t decrypted[4];
	decrypt_block(output, decrypted, round_keys);
	for (size_t i = 0; i < 4; ++i)
		printf("%08x ", decrypted[i]);
	printf("\n");

	return 0;
}