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
## Signature verification

In this exercise, you will write RIOT shell commands to sign and verify simple messages:
- One command that signs and print the signature
- One command that verify the signature
- One command to generate the key pair used to sign/verify the message

The signature mechanism is provided by an external package of RIOT, `c25519`.

The signature is stored in memory in a byte array and, to print it, it must be converted to a string of hexadecimal characters. RIOT provides a helper module, `fmt`, with useful functions to do that easily.

To generate random asymmetric public and secret keys, the application will use the `random` module of RIOT.

### Add required modules to the build

Since the application is about to implement shell functions, the `shell` module must be added to the build, as well as the `fmt` and the `random` modules. The `c25519` is provided as a package.

Edit the [Makefile](Makefile) and add there the required modules to the build.

```
USEMODULE += fmt
USEMODULE += shell
USEMODULE += random

USEPKG += c25519
```

### Implement the application

Now edit the [main.c](main.c) file as follows:

    
1. Add the required includes corresponding to the fmt, shell, random and c25519 modules that will be used by the application:

```c
#include "shell.h"
#include "fmt.h"
#include "edsign.h"
#include "ed25519.h"
#include "random.h"
```

2. Implement the key command handler function that will generate a new pair of key each time it is called:

    - First, under the header includes, declare 2 buffers where the asymmetric keys are stored:
    ```c
    static uint8_t secret_key[EDSIGN_SECRET_KEY_SIZE] = { 0 };
    static uint8_t public_key[EDSIGN_PUBLIC_KEY_SIZE] = { 0 };
    ```

    - In the `_key_handler` function, call the functions that will create the new keypair:
    ```c
          random_bytes(secret_key, sizeof(secret_key));
          ed25519_prepare(secret_key);
          edsign_sec_to_pub(public_key, secret_key);
    ```
    
    - In the `_key_handler` function, print the new keypair:
    ```c
          puts("New keypair generated:");
          printf("  - Secret: ");
          for (uint8_t i = 0; i < EDSIGN_SECRET_KEY_SIZE; ++i) {
              printf("%02X", secret_key[i]);
          }

          printf("\n  - Public: ");
          for (uint8_t i = 0; i < EDSIGN_PUBLIC_KEY_SIZE; ++i) {
              printf("%02X", public_key[i]);
          }
          puts("");
    ```

3. Implement the sign command handler function that will generate a signature from an input message:

    - First, under the header includes, declare 2 buffers where the signatures (byte and hex representation) are stored:
    ```c
    static uint8_t signature[EDSIGN_SIGNATURE_SIZE] = { 0 };
    static char signature_hex[EDSIGN_SIGNATURE_SIZE * 2 + 1] = { 0 };
    ```

     **Note:** the size of the hexadecimal string signature buffer is twice as large as the signature buffer itself, because a single byte is represented by 2 hexadecimal characters.

    - In the `_sign_handler` function, call the functions that will generate the signature of the input command argument message in `argv[1]`:
    ```c
          edsign_sign(signature, public_key, secret_key, (uint8_t *)argv[1], strlen(argv[1]));
    ```

    - Convert the signature (a byte array) to its hexadecimal string representation:
    ```c
          fmt_bytes_hex(signature_hex, signature, EDSIGN_SIGNATURE_SIZE);
    ```

4. Implement the verify command handler function:
    The message and the signature are provided as command line argument respectively in `argv[1]` and `argv[2]`.

    - In the `_verify_handler` function, you must first convert the signature provided as a string of hex characters to a byte array, this is done with the `hex_bytes` function (from the `fmt` module):
    ```c
          fmt_hex_bytes(signature, argv[2]);
    ```

    - Now call the functions that check the signature:
    ```c
          if (edsign_verify(signature, public_key, (uint8_t *)argv[1], strlen(argv[1]))) {
              puts("Message verified");
          }
          else {
              puts("Message not verified");
          }
    ```

### Build and test the application on native

Open a terminal with the `File > New > Terminal` menu, then build and run for native (the default board, according to the [Makefile](Makefile)):
<!-- #endregion -->

<!-- #raw -->
make -C riot/security/signature all term
<!-- #endraw -->

You can now play with the RIOT shell and list the commands available, with the help command:

<!-- #raw -->
> help
help
Command              Description
---------------------------------------
key                  Generate a new pair of keys
sign                 Compute the signature of a message
verify               Verify the signature of a message
<!-- #endraw -->

First, generate a new keypair:

<!-- #raw -->
> key
New keypair generated:
 - Secret: E002F870F121D3605A8D54709BCE4D32936C14F50D9F51867A6062DF2E6C0249
 - Public: 93F6592CC893E1DDF549225603E20B5B9038C9DC1830102546AAC9521968AC55
<!-- #endraw -->

Sign a first message:

<!-- #raw -->
> sign riot
0BC26A14A5AF5763A3F43B58AE965985657D5BF24A1A764E8BB0F5EA0236BB969D5E2B5FD42B56AD27DB54097AE60488637CB3F8AB504299AC1CF8D284534002
<!-- #endraw -->

Verify the signature:

<!-- #raw -->
> verify riot 0BC26A14A5AF5763A3F43B58AE965985657D5BF24A1A764E8BB0F5EA0236BB969D5E2B5FD42B56AD27DB54097AE60488637CB3F8AB504299AC1CF8D284534002
Message verified

> verify rito 0BC26A14A5AF5763A3F43B58AE965985657D5BF24A1A764E8BB0F5EA0236BB969D5E2B5FD42B56AD27DB54097AE60488637CB3F8AB504299AC1CF8D284534002
Message not verified
<!-- #endraw -->

### Build and test the application on IoT-LAB

The application is now ready and can be tested on real hardware, on an iotlab-m3 in the IoT-LAB testbed.

1. Choose your site (grenoble|lille|saclay|strasbourg):

```python
%env SITE=grenoble
```

2. Submit an experiment using the following command:

```python
!iotlab-experiment submit -n "riot-signature" -d 20 -l 1,archi=m3:at86rf231+site=$SITE
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
make BOARD=iotlab-m3 IOTLAB_NODE=auto -C riot/security/signature term
<!-- #endraw -->

Interact with the shell on the IoT-LAB M3 and verify that you can sign and verify signatures:

<!-- #raw -->
> help
help
Command              Description
---------------------------------------
key                  Generate a new pair of keys
sign                 Compute the signature of a message
verify               Verify the signature of a message
> key
New keypair generated:
 - Secret: E002F870F121D3605A8D54709BCE4D32936C14F50D9F51867A6062DF2E6C0249
 - Public: 93F6592CC893E1DDF549225603E20B5B9038C9DC1830102546AAC9521968AC55
> sign riot
0BC26A14A5AF5763A3F43B58AE965985657D5BF24A1A764E8BB0F5EA0236BB969D5E2B5FD42B56AD27DB54097AE60488637CB3F8AB504299AC1CF8D284534002
> verify riot 0BC26A14A5AF5763A3F43B58AE965985657D5BF24A1A764E8BB0F5EA0236BB969D5E2B5FD42B56AD27DB54097AE60488637CB3F8AB504299AC1CF8D284534002
Message verified
> verify rito 0BC26A14A5AF5763A3F43B58AE965985657D5BF24A1A764E8BB0F5EA0236BB969D5E2B5FD42B56AD27DB54097AE60488637CB3F8AB504299AC1CF8D284534002
Message not verified
<!-- #endraw -->

### Free up the resources

Stop your experiment to free up the device:

```python
!iotlab-experiment stop
```
