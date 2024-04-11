# Simple Serpent Implementation with Bitslices
> 20211205 Oliver Emery, CS:4980 (University of Iowa)

This is my implementation of [Serpent](https://en.wikipedia.org/wiki/Serpent_(cipher))
in C, created for my final project for a cryptography course. I also learned and used
GNU argp to help with argument parsing.

This demo supports two modes: CBC and ECB. I cheaped out on the IV and made it
constant 0s. As this is a frontend for a raw block cipher, it only processes
128-bit blocks. I have it set to pad the last block with 0s if it isn't aligned
to a 16-byte boundary. However, decrypting will leave the added padding in the
plaintext. To change this behavior, I would most likely need to implement some sort
of header with metadata including length.

```
$ ./serpent --help
Usage: serpent [OPTION...]
Serpent block cipher encrypter and decrypter

  -d, --decrypt[=MODE]       Perform decryption (CBC, ECB)
  -e, --encrypt[=MODE]       Perform encryption (CBC, ECB)

 Key options:
  -f, --keyfile=FILE         Use up to 256 bits of FILE as secret key
  -k, --key=KEY              Use KEY (hex literal) as secret key

  -?, --help                 Give this help list
      --usage                Give a short usage message

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Reads data block by block from STDIN and writes processed blocks to STDOUT. CBC
mode is default; alternatively, ECB may be specified.
```

Compile on any recent Linux distro with
```
$ gcc [-O3] -o serpent main.c serpent.c
```

#### Generate a random 32-byte keyfile
```
$ dd if=/dev/random of=keyfile bs=32 count=1
```

#### Example usage
``` sh
$ dd if=/dev/random of=keyfile bs=32 count=1
$ dd if=/dev/random of=input bs=512 count=4
$ ./serpent --encrypt=CBC --keyfile=keyfile < input > input.enc
$ ./serpent --decrypt=CBC --keyfile=keyfile < input.enc > input.dec
$ cmp input input.dec
```


### Original Info Slide
<center>
  <img srcdemo/presentation.png>
</center>

## ECB / CBC comparison
The following illustrates the weakness of ECB mode as compared to CBC mode. The
images were generated with [this script](demo/cbc-ecb-demo), which converts the
original image to a bitmap with ImageMagick then applies the cipher in each of
the modes and converts the results back to PNG.
<center>
  <img src=demo/comparison.png>
</center>

## Discussion


### [`operations.h`](operations.h)
In my code I have included ILP-optimized functions for S-box calculations.
I found these published [here](https://www.ii.uib.no/~osvik/pub/aes3.pdf).

Serpent was designed for bitslice operation. The initial and final permutations
serve only to reformat input blocks when operations are performed traditionally.

In normal use, the Serpent S-boxes are 16-element arrays, for example:

```
S0: [3 8 15 1 10 6 5 11 14 13 4 2 7 0 9 12]
```

For an input value of 3, the result is `S0[3] = 1` and so forth. However, using
the S-boxes in this form is extraordinarly inefficient for modern computers.
Arrays lookups require memory access, and only 4 bits of a 128-bit block can
be processed at a time.

With bitslices, the S-boxes become combinational logic circuits, with the
possibility of optimization. Each 32-bit input word of a block is treated
as an array of bits, allowing all operations for a block to be performed in
parallel. I first attempted to optimize each output bit individually as I've
learned to do in Digital Design:

```
    abcd    w x y z
 0  0000    0 0 1 1
 1  0001    1 0 0 0
 2  0010    1 1 1 1     w = abc + ab'c' + a'bc'd' + a'b'cd' + bcd + b'c'd
 3  0011    0 0 0 1       = (ad) ^ a ^ b ^ c ^ d
 4  0100    1 0 1 0     x = abcd + ab'c' + ac'd' + a'bc'd + a'cd' + b'cd'
 5  0101    0 1 1 0       = a ^ c ^ ac ^ bd ^ cd ^ abc ^ bcd
 6  0110    0 1 0 1     etc...
 7  0111    1 0 1 1
 8  1000    1 1 1 0
 9  1001    1 1 0 1
10  1010    0 1 0 0
11  1011    0 0 1 0
12  1100    0 1 1 1
13  1101    0 0 0 0
14  1110    1 0 0 1
15  1111    1 1 0 0
```

I soon realized that the outputs bits must be optimized together to reach
an efficient solution. The paper linked above gives a general overview of
the process of finding these optimizations for Serpent S-boxes.

Essentially, Osvik created an algorithm that brute forces combinations of
machine instructions, with some heuristics to avoid useless combinations.
His algorithm aimed to exploit instruction-level parallelism to reduce
CPU cycles for each S-box dramatically.

## References and Links

- [Serpent Specification](https://www.cl.cam.ac.uk/~rja14/Papers/serpent.pdf)
- [Serpent Home Page](https://www.cl.cam.ac.uk/~rja14/serpent.html)
- [Speeding up Serpent](https://www.ii.uib.no/~osvik/pub/aes3.pdf)



