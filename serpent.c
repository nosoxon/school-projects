#include <stdio.h>
#include <stdint.h>

#include "operations.h"


#define key_transform(n, pk, kw, off) \
	s##n(pk[off+3], pk[off+2], pk[off+1], pk[off], \
	     kw[off+3], kw[off+2], kw[off+1], kw[off])

#define FRAC_PHI	0x9e3779b9UL
void gen_keys(uint32_t key[8], uint32_t rk[33][4]) {
	uint32_t w[140] = { 0 };
	for (uint32_t i = 0; i < 8; ++i)
		w[i] = key[i];

	for (uint32_t i = 8; i < 140; ++i)
		w[i] = rol(w[i-8] ^ w[i-5] ^ w[i-3] ^ w[i-1] ^ FRAC_PHI ^ (i - 8), 11);

	uint32_t *pk = &w[8];
	uint32_t kw[132] = { 0 };
	key_transform(3, pk, kw, 0);
	for (size_t i = 0; i < 128; i += 32) {
		key_transform(2, pk, kw, i+4);
		key_transform(1, pk, kw, i+8);
		key_transform(0, pk, kw, i+12);
		key_transform(7, pk, kw, i+16);
		key_transform(6, pk, kw, i+20);
		key_transform(5, pk, kw, i+24);
		key_transform(4, pk, kw, i+28);
		key_transform(3, pk, kw, i+32);
	}

	for (int i = 0; i < 33; ++i)
		for (size_t j = 0; j < 4; ++j)
			rk[i][j] = kw[4*i+j];
}

#define linear_transform(a,b,c,d, w,x,y,z) { \
	a = rol(a, 13); \
	c = rol(c, 3); \
	b ^= a ^ c; \
	d ^= c ^ (a << 3); \
	b = rol(b, 1); \
	d = rol(d, 7); \
	a ^= b ^ d; \
	c ^= d ^ (b << 7); \
	a = rol(a, 5); \
	c = rol(c, 22); \
	w = a; x = b; y = c; z = d; }

#define inverse_linear_transform(a,b,c,d, w,x,y,z) { \
	c = ror(c, 22); \
	a = ror(a, 5); \
	c ^= d ^ (b << 7); \
	a ^= b ^ d; \
	d = ror(d, 7); \
	b = ror(b, 1); \
	d ^= c ^ (a << 3); \
	b ^= a ^ c; \
	c = ror(c, 3); \
	a = ror(a, 13); \
	w = a; x = b; y = c; z = d; }

#define mix_key(a,b,c,d, sk) \
	a ^= sk[0]; b ^= sk[1]; c ^= sk[2]; d ^= sk[3];


#define single_round(n, a,b,c,d, w,x,y,z, rk) \
	mix_key(a,b,c,d, rk); \
	s##n(d,c,b,a, z,y,x,w); \
	linear_transform(w,x,y,z, a,b,c,d);

void encrypt_block(uint32_t in[4], uint32_t out[4], uint32_t rk[33][4]) {
	register uint32_t w, x, y, z,
		a = in[0], b = in[1], c = in[2], d = in[3];

	single_round(0, a,b,c,d, w,x,y,z, rk[0])
	single_round(1, a,b,c,d, w,x,y,z, rk[1])
	single_round(2, a,b,c,d, w,x,y,z, rk[2])
	single_round(3, a,b,c,d, w,x,y,z, rk[3])
	single_round(4, a,b,c,d, w,x,y,z, rk[4])
	single_round(5, a,b,c,d, w,x,y,z, rk[5])
	single_round(6, a,b,c,d, w,x,y,z, rk[6])
	single_round(7, a,b,c,d, w,x,y,z, rk[7])
	single_round(0, a,b,c,d, w,x,y,z, rk[8])
	single_round(1, a,b,c,d, w,x,y,z, rk[9])
	single_round(2, a,b,c,d, w,x,y,z, rk[10])
	single_round(3, a,b,c,d, w,x,y,z, rk[11])
	single_round(4, a,b,c,d, w,x,y,z, rk[12])
	single_round(5, a,b,c,d, w,x,y,z, rk[13])
	single_round(6, a,b,c,d, w,x,y,z, rk[14])
	single_round(7, a,b,c,d, w,x,y,z, rk[15])
	single_round(0, a,b,c,d, w,x,y,z, rk[16])
	single_round(1, a,b,c,d, w,x,y,z, rk[17])
	single_round(2, a,b,c,d, w,x,y,z, rk[18])
	single_round(3, a,b,c,d, w,x,y,z, rk[19])
	single_round(4, a,b,c,d, w,x,y,z, rk[20])
	single_round(5, a,b,c,d, w,x,y,z, rk[21])
	single_round(6, a,b,c,d, w,x,y,z, rk[22])
	single_round(7, a,b,c,d, w,x,y,z, rk[23])
	single_round(0, a,b,c,d, w,x,y,z, rk[24])
	single_round(1, a,b,c,d, w,x,y,z, rk[25])
	single_round(2, a,b,c,d, w,x,y,z, rk[26])
	single_round(3, a,b,c,d, w,x,y,z, rk[27])
	single_round(4, a,b,c,d, w,x,y,z, rk[28])
	single_round(5, a,b,c,d, w,x,y,z, rk[29])
	single_round(6, a,b,c,d, w,x,y,z, rk[30])

	mix_key(a,b,c,d, rk[31]);
	s7(d,c,b,a, z,y,x,w);
	mix_key(w,x,y,z, rk[32]);

	out[0] = w;
	out[1] = x;
	out[2] = y;
	out[3] = z;
}

#define decrypt_round(n, a,b,c,d, w,x,y,z, rk) \
	inverse_linear_transform(w,x,y,z, a,b,c,d); \
	is##n(d,c,b,a, z,y,x,w); \
	mix_key(w,x,y,z, rk);


void decrypt_block(uint32_t in[4], uint32_t out[4], uint32_t rk[33][4]) {
	register uint32_t w, x, y, z,
		a = in[0], b = in[1], c = in[2], d = in[3];

	mix_key(a,b,c,d, rk[32]);
	is7(d,c,b,a, z,y,x,w);
	mix_key(w,x,y,z, rk[31]);

	decrypt_round(6, a,b,c,d, w,x,y,z, rk[30])
	decrypt_round(5, a,b,c,d, w,x,y,z, rk[29])
	decrypt_round(4, a,b,c,d, w,x,y,z, rk[28])
	decrypt_round(3, a,b,c,d, w,x,y,z, rk[27])
	decrypt_round(2, a,b,c,d, w,x,y,z, rk[26])
	decrypt_round(1, a,b,c,d, w,x,y,z, rk[25])
	decrypt_round(0, a,b,c,d, w,x,y,z, rk[24])
	decrypt_round(7, a,b,c,d, w,x,y,z, rk[23])
	decrypt_round(6, a,b,c,d, w,x,y,z, rk[22])
	decrypt_round(5, a,b,c,d, w,x,y,z, rk[21])
	decrypt_round(4, a,b,c,d, w,x,y,z, rk[20])
	decrypt_round(3, a,b,c,d, w,x,y,z, rk[19])
	decrypt_round(2, a,b,c,d, w,x,y,z, rk[18])
	decrypt_round(1, a,b,c,d, w,x,y,z, rk[17])
	decrypt_round(0, a,b,c,d, w,x,y,z, rk[16])
	decrypt_round(7, a,b,c,d, w,x,y,z, rk[15])
	decrypt_round(6, a,b,c,d, w,x,y,z, rk[14])
	decrypt_round(5, a,b,c,d, w,x,y,z, rk[13])
	decrypt_round(4, a,b,c,d, w,x,y,z, rk[12])
	decrypt_round(3, a,b,c,d, w,x,y,z, rk[11])
	decrypt_round(2, a,b,c,d, w,x,y,z, rk[10])
	decrypt_round(1, a,b,c,d, w,x,y,z, rk[9])
	decrypt_round(0, a,b,c,d, w,x,y,z, rk[8])
	decrypt_round(7, a,b,c,d, w,x,y,z, rk[7])
	decrypt_round(6, a,b,c,d, w,x,y,z, rk[6])
	decrypt_round(5, a,b,c,d, w,x,y,z, rk[5])
	decrypt_round(4, a,b,c,d, w,x,y,z, rk[4])
	decrypt_round(3, a,b,c,d, w,x,y,z, rk[3])
	decrypt_round(2, a,b,c,d, w,x,y,z, rk[2])
	decrypt_round(1, a,b,c,d, w,x,y,z, rk[1])
	decrypt_round(0, a,b,c,d, w,x,y,z, rk[0])

	out[0] = w;
	out[1] = x;
	out[2] = y;
	out[3] = z;
}

int main() {
	uint32_t kkey[8] = {
		0x9bd95acd, 0xfb668394, 0xeb4c9210, 0xed976378,
		0x47c45751, 0xf20de6f1, 0x2e98b4c5, 0x59d13854
	};

	uint32_t input[4] = { 0xfc994152, 0xb42fce88, 0x9df67c5a, 0xd260d4e9 };
	uint32_t output[4];

	uint32_t round_keys[33][4];
	gen_keys(kkey, round_keys);

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