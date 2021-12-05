#define fn(a, b) { \
	int tmp = a; \
	a = b; \
	b = tmp; }

#define fk(a, b) { fuk u a b; }

#define ooga(mac, x, y) f##mac(y, x)

#define key_trans(n, pk, kw, off) \
	s##n(pk[off], pk[off+1], pk[off+2], pk[off+3], kw[off], kw[off+1], kw[off+2], kw[off+3])


int main() {
	key_trans(3, pk, kw, 0);
	key_trans(2, pk, kw, 4*i);
	key_trans(1, pk, kw, 4*i);

	return 0;
}
