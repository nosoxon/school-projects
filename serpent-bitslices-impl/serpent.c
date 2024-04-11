/* 20211205 Oliver Emery (University of Iowa) CS:4980
 *
 * This file contains the mid-level cryptographic routines for Serpent:
 *   - key schedule generation
 *   - single block encrypt
 *   - single block decrypt
 *
 * I used several preprocessor macros to avoid too much repetition and to increase
 * efficiency via inlined "functions." There's not a whole lot to see here, most of
 * the interesting stuff is in `main.c' or `operations.h'
 */

#include <sys/types.h>

#include "operations.h"
#include "serpent.h"

#define FRAC_PHI	0x9e3779b9UL

#define key_transform(n, sk, rk, i, j) \
	s##n(sk[11+4*(i+j)], sk[10+4*(i+j)], sk[9+4*(i+j)], sk[8+4*(i+j)], \
	     rk[i+j][3], rk[i+j][2], rk[i+j][1], rk[i+j][0])

void generate_subkeys(uint32_t user_key[8], subkey_t subkey) {
	uint32_t prekey[140];
	for (size_t i = 0; i < 8; ++i)
		prekey[i] = user_key[i];
	for (uint32_t i = 8; i < 140; ++i)
		prekey[i] = rol(prekey[i-8] ^ prekey[i-5] ^ prekey[i-3] ^ prekey[i-1] ^ FRAC_PHI ^ (i - 8), 11);

	key_transform(3, prekey, subkey, 0, 0);
	for (size_t i = 0; i < 32; i += 8) {
		key_transform(2, prekey, subkey, i, 1);
		key_transform(1, prekey, subkey, i, 2);
		key_transform(0, prekey, subkey, i, 3);
		key_transform(7, prekey, subkey, i, 4);
		key_transform(6, prekey, subkey, i, 5);
		key_transform(5, prekey, subkey, i, 6);
		key_transform(4, prekey, subkey, i, 7);
		key_transform(3, prekey, subkey, i, 8);
	}
}

#define encrypt_round(n, a,b,c,d, w,x,y,z, sk) \
	mix_key(a,b,c,d, sk); \
	s##n(d,c,b,a, z,y,x,w); \
	linear_transform(w,x,y,z, a,b,c,d);

void encrypt_block(uint32_t in[4], uint32_t out[4], subkey_t sk) {
	register uint32_t w, x, y, z,
		a = in[0], b = in[1], c = in[2], d = in[3];

	encrypt_round(0, a,b,c,d, w,x,y,z, sk[0])
	encrypt_round(1, a,b,c,d, w,x,y,z, sk[1])
	encrypt_round(2, a,b,c,d, w,x,y,z, sk[2])
	encrypt_round(3, a,b,c,d, w,x,y,z, sk[3])
	encrypt_round(4, a,b,c,d, w,x,y,z, sk[4])
	encrypt_round(5, a,b,c,d, w,x,y,z, sk[5])
	encrypt_round(6, a,b,c,d, w,x,y,z, sk[6])
	encrypt_round(7, a,b,c,d, w,x,y,z, sk[7])
	encrypt_round(0, a,b,c,d, w,x,y,z, sk[8])
	encrypt_round(1, a,b,c,d, w,x,y,z, sk[9])
	encrypt_round(2, a,b,c,d, w,x,y,z, sk[10])
	encrypt_round(3, a,b,c,d, w,x,y,z, sk[11])
	encrypt_round(4, a,b,c,d, w,x,y,z, sk[12])
	encrypt_round(5, a,b,c,d, w,x,y,z, sk[13])
	encrypt_round(6, a,b,c,d, w,x,y,z, sk[14])
	encrypt_round(7, a,b,c,d, w,x,y,z, sk[15])
	encrypt_round(0, a,b,c,d, w,x,y,z, sk[16])
	encrypt_round(1, a,b,c,d, w,x,y,z, sk[17])
	encrypt_round(2, a,b,c,d, w,x,y,z, sk[18])
	encrypt_round(3, a,b,c,d, w,x,y,z, sk[19])
	encrypt_round(4, a,b,c,d, w,x,y,z, sk[20])
	encrypt_round(5, a,b,c,d, w,x,y,z, sk[21])
	encrypt_round(6, a,b,c,d, w,x,y,z, sk[22])
	encrypt_round(7, a,b,c,d, w,x,y,z, sk[23])
	encrypt_round(0, a,b,c,d, w,x,y,z, sk[24])
	encrypt_round(1, a,b,c,d, w,x,y,z, sk[25])
	encrypt_round(2, a,b,c,d, w,x,y,z, sk[26])
	encrypt_round(3, a,b,c,d, w,x,y,z, sk[27])
	encrypt_round(4, a,b,c,d, w,x,y,z, sk[28])
	encrypt_round(5, a,b,c,d, w,x,y,z, sk[29])
	encrypt_round(6, a,b,c,d, w,x,y,z, sk[30])

	mix_key(a,b,c,d, sk[31]);
	s7(d,c,b,a, z,y,x,w);
	mix_key(w,x,y,z, sk[32]);

	out[0] = w; out[1] = x; out[2] = y; out[3] = z;
}

#define decrypt_round(n, a,b,c,d, w,x,y,z, sk) \
	inverse_linear_transform(w,x,y,z, a,b,c,d); \
	is##n(d,c,b,a, z,y,x,w); \
	mix_key(w,x,y,z, sk);

void decrypt_block(uint32_t in[4], uint32_t out[4], subkey_t sk) {
	register uint32_t w, x, y, z,
		a = in[0], b = in[1], c = in[2], d = in[3];

	mix_key(a,b,c,d, sk[32]);
	is7(d,c,b,a, z,y,x,w);
	mix_key(w,x,y,z, sk[31]);

	decrypt_round(6, a,b,c,d, w,x,y,z, sk[30])
	decrypt_round(5, a,b,c,d, w,x,y,z, sk[29])
	decrypt_round(4, a,b,c,d, w,x,y,z, sk[28])
	decrypt_round(3, a,b,c,d, w,x,y,z, sk[27])
	decrypt_round(2, a,b,c,d, w,x,y,z, sk[26])
	decrypt_round(1, a,b,c,d, w,x,y,z, sk[25])
	decrypt_round(0, a,b,c,d, w,x,y,z, sk[24])
	decrypt_round(7, a,b,c,d, w,x,y,z, sk[23])
	decrypt_round(6, a,b,c,d, w,x,y,z, sk[22])
	decrypt_round(5, a,b,c,d, w,x,y,z, sk[21])
	decrypt_round(4, a,b,c,d, w,x,y,z, sk[20])
	decrypt_round(3, a,b,c,d, w,x,y,z, sk[19])
	decrypt_round(2, a,b,c,d, w,x,y,z, sk[18])
	decrypt_round(1, a,b,c,d, w,x,y,z, sk[17])
	decrypt_round(0, a,b,c,d, w,x,y,z, sk[16])
	decrypt_round(7, a,b,c,d, w,x,y,z, sk[15])
	decrypt_round(6, a,b,c,d, w,x,y,z, sk[14])
	decrypt_round(5, a,b,c,d, w,x,y,z, sk[13])
	decrypt_round(4, a,b,c,d, w,x,y,z, sk[12])
	decrypt_round(3, a,b,c,d, w,x,y,z, sk[11])
	decrypt_round(2, a,b,c,d, w,x,y,z, sk[10])
	decrypt_round(1, a,b,c,d, w,x,y,z, sk[9])
	decrypt_round(0, a,b,c,d, w,x,y,z, sk[8])
	decrypt_round(7, a,b,c,d, w,x,y,z, sk[7])
	decrypt_round(6, a,b,c,d, w,x,y,z, sk[6])
	decrypt_round(5, a,b,c,d, w,x,y,z, sk[5])
	decrypt_round(4, a,b,c,d, w,x,y,z, sk[4])
	decrypt_round(3, a,b,c,d, w,x,y,z, sk[3])
	decrypt_round(2, a,b,c,d, w,x,y,z, sk[2])
	decrypt_round(1, a,b,c,d, w,x,y,z, sk[1])
	decrypt_round(0, a,b,c,d, w,x,y,z, sk[0])

	out[0] = w; out[1] = x; out[2] = y; out[3] = z;
}
