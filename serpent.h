#ifndef __serpent_h
#define __serpent_h

#include <stdint.h>

typedef uint32_t subkey_t[33][4];

void generate_subkeys(uint32_t user_key[8], subkey_t subkey);
void encrypt_block(uint32_t in[4], uint32_t out[4], subkey_t sk);
void decrypt_block(uint32_t in[4], uint32_t out[4], subkey_t sk);

#endif