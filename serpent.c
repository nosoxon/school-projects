#include <stdio.h>

#include "constants.h"

typedef uint32_t block_t[4]; // LSW..MSW

#define get_bit(blk, bit) \
	((blk[(bit) / 32] & (1 << ((bit) % 32))) != 0)

#define set_bit(blk, bit, val) \
	if ((val)) blk[(bit) / 32] |=   (val) << ((bit) % 32); \
	else       blk[(bit) / 32] &= ~((val) << ((bit) % 32))

#define clear_block(blk) \
	blk[0] = 0; blk[1] = 0; blk[2] = 0; blk[3] = 0;

#define lrot(word, n) \
	((word) << (n)) | ((word) >> (32 - (n)))

void permutation(permutation_t table, block_t in, block_t out) {
	for (uint8_t b = 0; b < 128; ++b) {
		set_bit(out, b, get_bit(in, table[b]));
	}
}

void ltxform(ltxform_t table, block_t in, block_t out) {
	for (uint8_t b = 0; b < 128; ++b) {
		uint8_t v = get_bit(in, table[b][0]);
		for (uint8_t i = 1; table[b][i] != 0xff; ++i)
			v ^= get_bit(in, table[b][i]);
		set_bit(out, b, v);
	}
}

uint8_t sbox(uint8_t i, uint8_t val) {
	return sbox_tab[i % 8][val];
}

#define FRAC_PHI	0x9e3779b9
void generate_keys(uint32_t in[8], block_t subkeys[33]) {
	uint32_t w[140] = { 0 };
	for (uint32_t i = 0; i < 8; ++i)
		w[i] = in[i];
	for (uint32_t i = 8; i < 140; ++i) {
		w[i] = w[i-8] ^ w[i-5] ^ w[i-3] ^ w[i-1] ^ FRAC_PHI ^ (i - 8);
		w[i] = lrot(w[i], 11);
	}

	uint32_t *pk = &w[8];
	uint32_t keys[132] = {0};

	for (size_t i = 0; i < 33; ++i) {
		for (size_t j = 0; j < 32; ++j) {
			
		}
	}
	
}

// void encrypt_block(block_t io) {
// 	initial_permutation(io);
// 	for (uint8_t i = 0; i < 32; ++i)
// 		round(i, io);
// 	final_permutation(io);
// }

#define BBP "%c%c%c%c%c%c%c%c"
#define BNBP "%c%c%c%c\n%c%c%c%c\n"
#define B2B(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 


void print_block_bits(block_t blk) {
	for (int i = 3; i >= 0; --i)
		printf(BBP BBP BBP BBP "\n", B2B(blk[i] >> 24), B2B(blk[i] >> 16), B2B(blk[i] >> 8), B2B(blk[i]));
}

int main() {
	//block_t test = { 0xffff0000, 0xff00ff00, 0xf0f0f0f0, 0xcccccccc };
	block_t test = { 0xcccccccc, 0xf0f0f0f0, 0xff00ff00, 0xffff0000 };
	block_t result = {0};

	print_block_bits(test);
	printf("\n");

	permutation(ip_tab, test, result);
	for (int i = 3; i >= 0; --i) printf(BNBP BNBP BNBP BNBP "\n", B2B(result[i] >> 24), B2B(result[i] >> 16), B2B(result[i] >> 8), B2B(result[i]));
	printf("\n");

	block_t result2 = {0};
	permutation(fp_tab, result, result2);
	print_block_bits(result2);

	return 0;
}