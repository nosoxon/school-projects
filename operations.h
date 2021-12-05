#include <stdint.h>

/*

S-box 
https://www.ii.uib.no/~osvik/pub/aes3.pdf


*/

#define rol(w, n) (((uint32_t)(w)) << (n)) | (((uint32_t)(w)) >> (32 - (n)))
#define ror(w, n) (((uint32_t)(w)) >> (n)) | (((uint32_t)(w)) << (32 - (n)))


// MSB..LSB
#define s0(a,b,c,d, w,x,y,z) { \
	uint32_t t; \
	a ^= d; t =  c; c &= a; t ^= b; \
	c ^= d; d |= a; d ^= t; t ^= a; \
	a ^= b; b |= c; b ^= t; t = ~t; \
	t |= c; c ^= a; c ^= t; a |= d; \
	c ^= a; t ^= a; \
	w = d; x = b; y = t; z = c; }

#define is0(a,b,c,d, w,x,y,z) { \
	uint32_t t; \
	b = ~b; t =  c; c |= d; t = ~t; \
	c ^= b; b |= t; c ^= a; d ^= t; \
	b ^= d; d &= a; t ^= d; d |= c; \
	d ^= b; a ^= t; b ^= c; a ^= d; \
	a ^= c; b &= a; t ^= b; \
	w = a; x = c; y = t; z = d; }

#define s1(a,b,c,d, w,x,y,z) { \
	uint32_t t; \
	d = ~d; b = ~b; t =  d; d &= c; \
	b ^= d; d |= a; a ^= b; c ^= d; \
	d ^= t; t |= c; c ^= a; b |= d; \
	b &= t; d ^= c; c &= b; c ^= d; \
	d &= b; d ^= t; \
	w = c; x = a; y = d; z = b; }

#define is1(a,b,c,d, w,x,y,z) { \
	uint32_t t; \
	t =  c; c ^= a; a &= c; t ^= b; \
	a ^= d; d |= c; b ^= a; d ^= t; \
	d |= b; c ^= a; d ^= c; c |= a; \
	c ^= d; t = ~t; t ^= c; c |= d; \
	c ^= d; c |= t; a ^= c; \
	w = b; x = a; y = d; z= t; }

#define s2(a,b,c,d, w,x,y,z) { \
	uint32_t t; \
	t =  d; d &= b; d ^= a; b ^= c; \
	b ^= d; a |= t; a ^= c; t ^= b; \
	c =  a; a |= t; a ^= d; d &= c; \
	t ^= d; c ^= a; c ^= t; t = ~t; \
	w = t; x = c; y = a; z = b; }

#define is2(a,b,c,d, w,x,y,z) { \
	uint32_t t; \
	b ^= a; a ^= d; t =  a; a &= b; \
	a ^= c; c |= b; c ^= t; t &= a; \
	b ^= a; t &= d; t ^= b; b &= c; \
	b |= d; a = ~a; b ^= a; d ^= a; \
	d &= c; a ^= t; a ^= d; \
	w = a; x = b; y = t; z = c; }

#define s3(a,b,c,d, w,x,y,z) { \
	uint32_t t; \
	t =  d; d |= a; a ^= c; c &= t; \
	t ^= b; b ^= a; a &= d; t |= c; \
	a ^= t; d ^= c; t &= d; c ^= a; \
	t ^= b; c |= d; c ^= b; d ^= a; \
	b =  c; c |= a; c ^= d; \
	w = t; x = a; y = b; z = c; }

#define is3(a,b,c,d, w,x,y,z) { \
	uint32_t t; \
	t =  b; b ^= c; d ^= b; t &= b; \
	t ^= d; d &= c; c ^= a; a |= t; \
	b ^= a; d ^= a; c ^= t; a &= b; \
	a ^= c; c ^= d; c |= b; d ^= a; \
	c ^= t; d ^= c; \
	w = d; x = a; y = c; z = b; }

#define s4(a,b,c,d, w,x,y,z) { \
	uint32_t t; \
	c ^= a; a = ~a; b ^= a; a ^= d; \
	t =  c; c &= a; c ^= b; t ^= a; \
	d ^= t; b &= t; b ^= d; d &= c; \
	a ^= d; t |= c; t ^= d; d |= a; \
	d ^= b; b &= a; d = ~d; t ^= b; \
	w = a; x = d; y = t; z = c; }

#define is4(a,b,c,d, w,x,y,z) { \
	uint32_t t; \
	t =  b; b &= a; b ^= c; c |= a; \
	c &= d; t ^= b; t ^= c; c &= b; \
	d = ~d; a ^= t; c ^= a; a &= d; \
	a ^= b; d ^= c; b &= d; a ^= d; \
	b ^= t; b |= a; a ^= d; b ^= c; \
	w = t; x = b; y = a; z = d; }

#define s5(a,b,c,d, w,x,y,z) { \
	uint32_t t; \
	d ^= c; c ^= a; a = ~a; t =  c; \
	c &= d; b ^= a; c ^= b; b |= t; \
	t ^= a; a &= c; a ^= d; t ^= c; \
	t ^= b; b ^= d; d &= a; b = ~b; \
	d ^= t; t |= a; b ^= t; \
	w = b; x = d; y = a; z = c; }

#define is5(a,b,c,d, w,x,y,z) { \
	uint32_t t; \
	c = ~c; t =  a; b ^= c; a |= d; \
	a ^= b; b |= c; b &= d; t ^= a; \
	b ^= t; t |= d; t ^= c; c &= b; \
	c ^= a; t ^= b; a &= t; t ^= c; \
	a ^= t; t = ~t; a ^= d; \
	w = b; x = a; y = t; z = c; }

#define s6(a,b,c,d, w,x,y,z) { \
	uint32_t t; \
	b = ~b; t =  a; a &= d; d ^= t; \
	a ^= b; b |= t; c ^= a; b ^= d; \
	d |= c; b ^= c; t ^= d; d |= a; \
	d ^= b; t ^= a; t ^= d; a = ~a; \
	b &= t; b ^= a; \
	w = b; x = t; y = c; z = d; }

#define is6(a,b,c,d, w,x,y,z) { \
	uint32_t t; \
	d ^= b; t =  b; b &= d; t ^= a; \
	b = ~b; a ^= c; b ^= a; t |= d; \
	d ^= b; a ^= t; t ^= c; c &= a; \
	c ^= d; d ^= a; d |= b; a ^= c; \
	t ^= d; \
	w = a; x = t; y = b; z = c; }

#define s7(a,b,c,d, w,x,y,z) { \
	uint32_t t; \
	t =  c; c |= b; c ^= a; t ^= b; \
	b ^= c; a |= t; a &= d; t ^= b; \
	a ^= c; c |= t; c ^= d; d |= t; \
	d ^= b; c ^= t; b ^= c; c &= d; \
	c ^= t; b = ~b; b |= d; t ^= b; \
	w = d; x = c; y = a; z = t; }

#define is7(a,b,c,d, w,x,y,z) { \
	uint32_t t; \
	t =  b; b ^= d; d &= a; t |= a; \
	b = ~b; a ^= c; c |= d; d ^= b; \
	b &= t; a &= t; c ^= b; b ^= d; \
	d |= b; t ^= c; d ^= a; a ^= t; \
	t |= d; a ^= b; t ^= b; \
	w = t; x = c; y = d; z = a; }

