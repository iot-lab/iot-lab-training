---
jupyter:
  jupytext:
    text_representation:
      extension: .md
      format_name: markdown
      format_version: '1.3'
      jupytext_version: 1.19.3
  kernelspec:
    display_name: Python 3 (ipykernel)
    language: python
    name: python3
---

<!-- #region -->
## Message encryption

In this exercise, you will write 2 RIOT shell commands to encrypt and decrypt simple messages. The algorithm will use AES 128 symmetric encryption and more precisely, this exercise will use the CTR cipher mode which is able to encrypt/decrypt messages of arbitrary sizes.

AES encryption is provided by `crypto` module of RIOT and CTR cipher mode is provided by the `cipher_modes` module.

The encrypted message is computed in memory in a byte array and, to print it, it must be converted to a string of hexadecimal characters. To do this, RIOT provides a helper module, `fmt`, with useful functions to do that easily.

Since the algorithm is based on a symmetric key, a key is provided statically in the code of the exercise.

### Add required modules to the build

Since the application is about to implement shell functions, the `shell` module must be added to the build, as well as the `fmt`, `crypto` and `cipher_modes` modules.

Edit the [Makefile](Makefile) and add there the required modules to the build.

```
USEMODULE += crypto
USEMODULE += crypto_aes_128
USEMODULE += cipher_modes
USEMODULE += fmt
USEMODULE += shell
```

### Implement the application

Now edit the [main.c](main.c) file as follows:

    
1. Add the required includes corresponding to the fmt, shell and hashes modules that will be used by the application:

```c
#include "shell.h"
#include "fmt.h"
#include "crypto/ciphers.h"
#include "crypto/modes/ctr.h"
```

2. Define the symmetric key and the nonce:
    
```c
static const uint8_t key[] = {
    0x23, 0xA0, 0x18, 0x53, 0xFA, 0xB3, 0x89, 0x23,
    0x65, 0x89, 0x2A, 0xBC, 0x43, 0x99, 0xCC, 0x00
};

static const uint8_t ctr[] = {
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
    0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};
```

3. Implement the encrypt command handler function that will return the input message in its encrypted form. The encrypted message is printed as a string of hexadecimal characters:

    - In the `_encrypt_handler` function, call the functions that will encrypt the message:
    ```c
          cipher_t cipher;
          cipher_init(&cipher, CIPHER_AES, key, sizeof(key));
          size_t enc_len = cipher_encrypt_ctr(&cipher, ctr_copy, 0, (uint8_t *)argv[1], strlen(argv[1]), data);
    ```
    We use a copy of the nonce buffer because it is modifed by the call to `cypher_encrypt_ctr`.
    
    - Convert the encrypted message (a byte array) to its hexadecimal string representation:
    ```c
          size_t len = fmt_bytes_hex(buf_str, data, enc_len);
          buf_str[len] = 0;
    ```
    The hexadecimal string buffer must be closed to ensure a proper output.

3. Implement the decrypt command handler function:

    - In the `_decrypt_handler` function, you must first convert the hexadecimal representation of the input encrypted message into a byte array:
    ```c
          size_t len = fmt_hex_bytes(data, argv[1]);
    ```

    - Then, decrypt the content of the `data` buffer:
    ```c
          cipher_t cipher;
          cipher_init(&cipher, CIPHER_AES, key, sizeof(key));
          cipher_decrypt_ctr(&cipher, ctr_copy, 0, data, len, (uint8_t *)buf_str);
          buf_str[len] = 0;
    ```
    The output should directly be readable, so there's no need to convert it to hexadecimal.

### Build and test the application on native

Open a terminal with the File > New > Terminal menu, then build and run for native (the default board, according to the Makefile):
<!-- #endregion -->

<!-- #raw -->
make -C riot/security/encryption all term
<!-- #endraw -->

You can now play with the RIOT shell and list the commands available, with the help command:

<!-- #raw -->
> help
help
Command              Description
---------------------------------------
encrypt              Encrypt input string
decrypt              Decrypt input string
<!-- #endraw -->

Encrypt a message:

<!-- #raw -->
> encrypt riot
encrypt riot
E7461EA1
<!-- #endraw -->

Decrypt the encrypted message and check you get the initial message back:

<!-- #raw -->
> decrypt E7461EA1
decrypt E7461EA1
riot
<!-- #endraw -->

You can also verify that an altered encrypted message produces an invalid (or empty) output:

<!-- #raw -->
> decrypt E7461EA2
decrypt E7461EA2
riow
<!-- #endraw -->

### Build and test the application on IoT-LAB

The application is now ready and can be tested on real hardware, on an iotlab-m3 in the IoT-LAB testbed.

1. Choose your site (grenoble|lille|saclay|strasbourg):

```python
%env SITE=grenoble
```

2. Submit an experiment using the following command:

```python
!iotlab-experiment submit -n "riot-encryption" -d 20 -l 1,archi=m3:at86rf231+site=$SITE
```

3. Wait for the experiment to be in the Running state:

```python
!iotlab-experiment wait --timeout 30 --cancel-on-timeout
```

**Note:** If the command above returns the message `Timeout reached, cancelling experiment <exp_id>`, try to re-submit your experiment later or try on another site.


4. Get the experiment nodes list:

```python
!iotlab-experiment --jmespath="items[*].network_address | sort(@)" get --nodes
```

5. Build and flash the shell application for the iotlab-m3 target device:

```python
!make BOARD=iotlab-m3 IOTLAB_NODE=auto flash
```

6. In the terminal, open the serial link:

<!-- #raw -->
make BOARD=iotlab-m3 IOTLAB_NODE=auto -C riot/security/encryption term
<!-- #endraw -->

Interact with the shell on the IoT-LAB M3 to encrypt and decrypt messages:

<!-- #raw -->
> help
help
Command              Description
---------------------------------------
encrypt              Encrypt input string
decrypt              Decrypt input string
> encrypt riot
encrypt riot
EB68EC38
> decrypt EB68EC38
decrypt EB68EC38
riot
> decrypt EB68EC37
decrypt EB68EC37
rio{
> 
<!-- #endraw -->

### Free up the resources

Stop your experiment to free up the device:

```python
!iotlab-experiment stop
```
