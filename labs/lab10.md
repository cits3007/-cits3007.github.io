---
title:  CITS3007 lab 10 (week 11)&nbsp;--&nbsp;Cryptography
---

## 1. Cryptography libraries

We will investigate how to perform basic encryption tasks using a cryptography library
called [Sodium][libso], which is written in C.  It is well-documented (you can find the
documentation [here][libso-docs]), well-tested, highly portable, and used by many other projects.
It allows us to perform tasks like encryption, decryption, signature checking, and password
hashing.

Although Sodium is a C library, we will use it from the Python language, as that requires
much less boilerplate code.
In the CITS3007 SDE, we need to install the Python library [PyNaCl][pynacl], which "wraps"
the C Sodium library, and provides a "Pythonic" interface to it (the documentation for
PyNaCl is available [here][pynacl-doc]).[^python-crypto-libs]
Run the following commands in your development VM:

[libso]: https://doc.libsodium.org
[libso-docs]: https://doc.libsodium.org/
[pynacl]: https://github.com/pyca/pynacl
[pynacl-doc]: https://pynacl.readthedocs.io/en/latest/

[^python-crypto-libs]: There are actually multiple Python libraries which provide
  access to the C Sodium library, which can be confusing, but they have quite
  different purposes. PyNaCl, which we use, provides a fairly *high-level* interface to
  Sodium, and allows Python programmers to use Python types (such as classes and lists)
  which they are familiar with. \
  &nbsp; &nbsp; Two other Python libraries are [pysodium](https://github.com/stef/pysodium)
  and [libnacl](https://github.com/saltstack/libnacl). These are *not* high-level -- they
  pretty directly wrap the exact C functions exposed by the C Sodium library, and allow
  them to be called from Python.





```bash
$ sudo apt-get update
$ sudo apt-get install python3-pip
$ pip install pynacl
```

This ensures we have the `pip` command available for managing Python libraries, then uses it
to install PyNaCl. We'll show how to use the PyNaCl library to create a public--private key
pair (like those used by GitHub to allow repositories to be cloned or pushed without using a
password). The lecture slides contain more information about public key cryptosystems like
this, as does the PyNaCl documentation,
[here](https://pynacl.readthedocs.io/en/latest/public/).

**Exercise**

:   Suppose Alice and Bob are both using a [public-key cryptosystem][pk], and
    both make their public keys available on the Web for anyone to access.
    Explain how could they use their keys so that Alice can securely send an encrypted
    message or file which can only be read by Bob.

[pk]: https://en.wikipedia.org/wiki/Public-key_cryptography

### 1.1. Generating a key pair

In this section and the following ones, we will generate public--private key pairs, and use
them to transfer encrypted content in exactly the way Alice and Bob could, in the previous
exercise.

Save the following as `keygen.py`:

```{.python .numberLines}
import nacl.utils
from nacl.public import PrivateKey
from nacl.encoding import HexEncoder

def write(name, hex, suffix):
    filename = 'key_' + name + suffix 
    with open(filename, 'wb') as ofp:
      ofp.write(hex)

def make_keys(name):
    secretKey = PrivateKey.generate()
    write(name, secretKey.encode(encoder=HexEncoder), '.sk')
    publicKey = secretKey.public_key
    write(name, publicKey.encode(encoder=HexEncoder), '.pk')

key_name = input("Enter a name for the key pair to generate: ")

make_keys(key_name)
```

Run it by executing `python3 keygen.py`, and entering a name
(this could be a particular purpose you're generating the key pair
for -- for instance, `secret-hushmoney-communications-with-my-accountant` -- or just
your own name).

This will generate two files, `key_[NAME].sk` and `key_[NAME].pk`,
which hold our private and public keys, respectively. If you inspect those files (e.g. by
using `less`) you will see that they simply contain a long sequence of hexadecimal digits.

In detail, here's how the code works:

- Lines 1-4 import several modules:

  - `nacl.utils`: This module provides general utility functions for
    working with libsodium.
  - `nacl.public.PrivateKey`: This is a class from the `nacl.public`
    module used to generate a pair of public and private keys for
    encryption.
  - `nacl.encoding.HexEncoder`: This class from the `nacl.encoding`
    module is used to encode binary data as hexadecimal strings.

- The code defines two functions:
  - `write(name, hex, suffix)`: This function is responsible for writing
    the hexadecimal representation of a key (either a secret key or a
    public key) to a file with a specific name and suffix.
  - `make_keys(name)`: This function generates a pair of public and
    private keys, writes them to separate files, and takes a
    user-provided name for the key pair.

- Lines 11-15:
   - Inside the `make_keys(name)` function, a secret key is generated
     using `PrivateKey.generate()`. This secret key will be used for
     encryption and decryption by you (the user creating the key pair).
   - The secret key is encoded as a hexadecimal string using
     `HexEncoder` and written to a file with the `.sk` suffix.
   - The public key is extracted from the secret key and similarly
     encoded and written to a file with the `.pk` suffix.

- Lines 17--19:

  The code uses `input()` to prompt the user to enter a name for the key
  pair they want to generate, then calls `make_keys` to do the
  generation.

The secret key (in the ".sk" file) can be used by the user, you, to
encrypt, decrypt and sign messages. The public key (in the ".pk" file)
can be published to others, and can be used by other people to encrypt
messages written to you, or decrypt messages written by you.

### 1.2. Using the key pair to encrypt

If possible, get another person in the lab to generate a key pair,
and exchange public keys. Alternatively, create a second key pair with
a different name (e.g. "other"), and choose this to be the "other
person".

Encrypt a message using the recipient's public key and your private key.
Save the following script as `encrypt.py`:

```{.python .numberLines}
import nacl.utils
from nacl.public import PrivateKey, PublicKey, Box
from nacl.encoding import HexEncoder

class EncryptFile :
    def __init__(self, sender, receiver):
        self.sender = sender
        self.receiver = receiver
        self.sk = PrivateKey(self.get_key(sender, '.sk'), encoder=HexEncoder)
        self.pk = PublicKey(self.get_key(receiver, '.pk'), encoder=HexEncoder)

    def get_key(self, name, suffix):
        filename = 'key_' + name + suffix
        file = open(filename, 'rb')
        data = file.read()
        file.close()
        return data

    def encrypt(self, textfile, encfile):
        box = Box(self.sk, self.pk)
        tfile = open(textfile, 'rb')
        text = tfile.read()
        tfile.close()
        etext = box.encrypt(text)
        efile = open(encfile, 'wb')
        efile.write(etext)
        efile.close()

sender = input("Enter the name for your key pair: ")
recip = input("Enter the name for the recipient's key pair: ")
encrypter = EncryptFile(sender, recip)
target_file = input("Enter a file to encrypt: ")
encrypter.encrypt(target_file, f'{target_file}.enc')
print('Done!')
```

Run it with the command `python3 encrypt.py`.
You will need to provide the name of your key pair (from the
previous exercise), the recipient's key pair, and a file to encrypt
(you can just choose the `encrypt.py` script if you have
no other text file handy).

The script should create a binary file `ORIG_FILE.enc` (where
`ORIG_FILE` is whatever the name of the original file was) -- this is
the encrypted file.

In more detail, here is what the script does:

- Define a class `EncryptFile`.

  - The code defines a Python class named `EncryptFile`. This class is
    designed to handle file encryption operations.
  - The constructor (`__init__`) of this class initializes the sender
    and receiver names and loads the sender's private key (with
    extension `.sk`) and the
    receiver's public key (with extension `.pk`) from files.
   - The `get_key` method is used to read the contents of a key file
     (either a `.sk` or `.pk` file) and return it as binary data.
   - The `encrypt` method is used to encrypt a file. It loads the
     contents of a text file specified by `textfile`, encrypts it using
     the sender's private key (`sk`) and the recipient's public key
     (`pk`), and then writes the encrypted data to a new file specified
     by `encfile`.

- lines 29--34:

  - The code prompts the user to enter the names for their key pair
    (`sender`) and the recipient's key pair (`recip`) and a file to
    encrypt.
  - An instance of the `EncryptFile` class is created with the sender's name and the recipient's name.
  - The `encrypt` method of the `EncryptFile` instance is called with
    the target file and the name of the encrypted output file (the
    encrypted file will have a `.enc` extension).

### 1.3. Using the key pair to decrypt

Save the following as `decrypt.py`:

```python
import nacl.utils
from nacl.public import PrivateKey, PublicKey, Box
from nacl.encoding import HexEncoder
import sys

class DecryptFile:
    def __init__(self, sender, receiver):
        self.sender = sender
        self.receiver = receiver
        self.sk = PrivateKey(self.get_key(receiver, '.sk'), encoder=HexEncoder)
        self.pk = PublicKey(self.get_key(sender, '.pk'), encoder=HexEncoder)

    def get_key(self, name, suffix):
        filename = 'key_' + name + suffix
        try:
            with open(filename, 'rb') as file:
                data = file.read()
            return data
        except FileNotFoundError:
            print(f"Key file '{filename}' not found.")
            sys.exit(1)

    def decrypt(self, encfile, textfile):
        box = Box(self.sk, self.pk)
        try:
            with open(encfile, 'rb') as efile:
                etext = efile.read()
            dtext = box.decrypt(etext)
            with open(textfile, 'wb') as tfile:
                tfile.write(dtext)
            print(f"Decrypted file saved as '{textfile}'")
        except FileNotFoundError:
            print(f"Encrypted file '{encfile}' not found.")
            sys.exit(1)

sender = input("Enter the name for the sender's key pair: ")
recip = input("Enter your name for your key pair: ")
decrypter = DecryptFile(sender, recip)
enc_file = input("Enter the name of the encrypted file to decrypt: ")
target_file = input("Enter the name for the decrypted output file: ")
decrypter.decrypt(enc_file, target_file)
```

To use the script, you need to have
an encrypted file (with a `.enc` extension) generated by the
"encrypt.py" script in the same directory -- ideally, swap
with another person and attempt to decrypt their `.enc` file --
together with your private
key (with an `.sk` extension), and the other person's public key (with
a `.pk` extension).

Run `python3 decrypt.py` and follow the prompts: enter the sender's key
pair name, your key pair name, the name of the encrypted file to
decrypt, and the name for the decrypted output file.

The script will decrypt the file using the private key associated with
your name and the sender's public key and save the decrypted content to
the specified output file.

### 1.4. Challenge task

As a challenge task, you might like to research how to use libsodium
to *sign* a message with your key pair so that other users can verify
a (plaintext) message came from you.

## 2. Cryptography questions and exercises

See if you can answer the following questions, after reviewing the material on cryptography
in the lectures.

**Question 2(a)**

:   Suppose in the CITS3007 SDE you create the MD5 hash of some password, using a command like:

    ```
    $ printf mypassword | md5sum
    ```

    In what format is the hash displayed? How large is the hash, in bytes?
    How would you write it in C syntax?



**Question 2(b)**

:   What is the purpose of salting passwords, when creating a password hash?



**Question 2(c)**

:   Look up Wikipedia to refresh your memory of what a *hash collision* is. Explain why hash
    collisions necessarily occur. That is, why must there always be two different plaintexts
    that have the same hash value?



## 3. CITS3007 project

You can use your lab time to work on the CITS3007 project. You may wish to discuss your
project tests and code design with other students or the lab facilitators (although the
actual code you submit must be your own, individual work).

<!-- vim: syntax=markdown tw=92 :
-->
