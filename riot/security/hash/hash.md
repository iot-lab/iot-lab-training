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

## Hash computation

In this exercise, you will write RIOT shell commands to compute the hash of a given input string:
- One command that produces and print the hash using the SHA256 algorithm
- One command that produces and print the hash using the SHA3-256 algorithm

In both cases, the hash is stored in memory in a byte array and, to print it, it must be converted to a string of hexadecimal characters. RIOT provides a helper module, `fmt`, with useful functions to do that easily.

Hash functions of several algorithms (SHA256, SHA3-256, SHA1, MD5, CMAC) are provided by the `hashes` module.

### Add required modules to the build

Since the application is about to implement shell functions, the `shell` module must be added to the build, as well as the `fmt` et `hashes` modules.

Edit the [Makefile](Makefile) and add there the required modules to the build.

```
USEMODULE += fmt
USEMODULE += hashes
USEMODULE += shell
```

### Implement the application

Now edit the [main.c](main.c) file as follows:

1. Add the required includes corresponding to the fmt, shell and hashes modules that will be used by the application:

```c
#include "fmt.h"
#include "shell.h"

#include "hashes/sha256.h"
#include "hashes/sha3.h"
```

2. Implement the sha256 command handler function:

    - Declare 2 buffers, the first one for the computed hash itself, and the second one for its hexadecimal string representation:

    ```c
    static uint8_t sha256_hash[SHA256_DIGEST_LENGTH];
    static char sha256_hash_hex[SHA256_DIGEST_LENGTH * 2 + 1];
    ```

     **Note:** the size of the hexadecimal string buffer is twice as large as the hash itself, because a single byte is represented by 2 hexadecimal characters.

    - Call the functions that will compute the hash from the command line argument `argv[1]`:
    
    ```c
          sha256_context_t sha256;
          sha256_init(&sha256);
          sha256_update(&sha256, (uint8_t*)argv[1], strlen(argv[1]));
          sha256_final(&sha256, sha256_hash);
    ```

    - Convert the computed hash (a byte array) to its hexadecimal string representation:
    
    ```c
          fmt_bytes_hex(sha256_hash_hex, sha256_hash, SHA256_DIGEST_LENGTH);
    ```

3. Implement the sha3 command handler function:
    - Declare 2 buffers, the first one for the computed hash itself, and the second one for its hexadecimal string representation:
    
    ```c
    static uint8_t sha3_hash[SHA3_256_DIGEST_LENGTH];
    static char sha3_hash_hex[SHA3_256_DIGEST_LENGTH * 2 + 1];
    ```

    - Call the functions that will compute the hash from the command line argument `argv[1]`:
    
    ```c
          keccak_state_t state;
          sha3_256_init(&state);
          sha3_update(&state, argv[1], strlen(argv[1]));
          sha3_256_final(&state, sha3_hash);
    ```
    
    - Convert the computed hash (a byte array) to its hexadecimal string representation:
    
    ```c
          fmt_bytes_hex(sha3_hash_hex, sha3_hash, SHA3_256_DIGEST_LENGTH);
    ```

### Build and test the application on native

Open a terminal with the `File > New > Terminal` menu, then build and run for native (the default board, according to the [Makefile](Makefile)):

<!-- #raw -->
make -C riot/security/hash all term
<!-- #endraw -->

You can now play with the RIOT shell and list the commands available, with the `help` command:

<!-- #raw -->
> help
help
Command              Description
---------------------------------------
sha256               Compute SHA256 hasg
sha3                 Compute SHA3 hash
<!-- #endraw -->

Verify the computed hashes are valid:

<!-- #raw -->
> sha256 riot
sha256 riot
SHA256: FCCDA8FAEFEBB2EC17684836D4DD8793011EA79DA7244E0E04D2460BEF080E3C

> sha3 riot
sha3 riot
SHA3: 759B2B5474663D26B675350B09E2C2A41B6090B0F2BBFFBFF5C1802F7628426B
<!-- #endraw -->

You can compare the sha256 output with the result of `sha256sum` command on Linux:

```python
!echo -n riot | sha256sum
```

### Build and test the application on IoT-LAB

The application is now ready and can be tested on real hardware, on an iotlab-m3 in the IoT-LAB testbed.

1. Choose your site (grenoble|lille|saclay|strasbourg):

```python
%env SITE=grenoble
```

2. Submit an experiment using the following command:

```python
!iotlab-experiment submit -n "riot-hash" -d 20 -l 1,archi=m3:at86rf231+site=$SITE
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
make BOARD=iotlab-m3 IOTLAB_NODE=auto -C riot/security/hash term
<!-- #endraw -->

Interact with the shell on the IoT-LAB M3 and verify that you get the same hash results:

<!-- #raw -->
> help
help
Command              Description
---------------------------------------
sha256               Compute SHA256 hasg
sha3                 Compute SHA3 hash
> sha256 riot
sha256 riot
SHA256: FCCDA8FAEFEBB2EC17684836D4DD8793011EA79DA7244E0E04D2460BEF080E3C
> sha3 riot
sha3 riot
SHA3: 759B2B5474663D26B675350B09E2C2A41B6090B0F2BBFFBFF5C1802F7628426B
<!-- #endraw -->

### Free up the resources

Stop your experiment to free up the device:

```python
!iotlab-experiment stop
```
