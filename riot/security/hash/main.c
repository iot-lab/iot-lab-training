#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/* Add required includes here */


/* Add here the buffers to store the SHA256 hash and its hexadecimal representation */


/* Add here the buffers to store the SHA3 hash and its hexadecimal representation */


/* Implement the SHA256 command handler here */
static int _sha256_handler(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s <input to hash>\n", argv[0]);
        return 1;
    }

    /* Compute the hash here */


    /* Convert the hash array to a string of hex characters */


    /* Print the hash */
    printf("SHA256: %s\n", sha256_hash_hex);

    return 0;
}


/* Implement the SHA3 command handler here */
static int _sha3_handler(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s <input to hash>\n", argv[0]);
        return 1;
    }

    /* Compute the hash here */


    /* Convert the hash array to a string in hex format */


    /* Print the hash */
    printf("SHA3: %s\n", sha3_hash_hex);

    return 0;
}

/* Declare the list of shell commands */
static const shell_command_t shell_commands[] = {
    { "sha256", "Compute SHA256 hasg", _sha256_handler },
    { "sha3", "Compute SHA3 hash", _sha3_handler },
    { NULL, NULL, NULL }
};

int main(void)
{
    /* Configure and start the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
