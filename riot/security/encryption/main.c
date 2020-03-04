#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/* Add required includes here */



/* Intermediate encryption/decryption buffers  */
#define BUF_SIZE (64U)
static uint8_t data[BUF_SIZE] = { 0 };
static char buf_str[BUF_SIZE * 2] = { 0 };
static uint8_t ctr_copy[16];


/* Add here the key and the nonce */


/* Implement the encrypt command handler here */
static int _encrypt_handler(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s <input to encrypt>\n", argv[0]);
        return 1;
    }

    /* Clear intermediate data buffer */
    memset(data, 0, BUF_SIZE);

    /* Copy the nonce in memory */
    memcpy(ctr_copy, ctr, 16);

    /* Encrypt the message */


    /* Convert the byte array to a string of hex characters */


    /* Print the result */
    printf("%s\n", buf_str);

    return 0;
}

/* Implement the decrypt command handler here */
static int _decrypt_handler(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s <input to decrypt>\n", argv[0]);
        return 1;
    }

    /* Clear intermediate data buffer */
    memset(data, 0, BUF_SIZE);

    /* Copy the nonce in memory */
    memcpy(ctr_copy, ctr, 16);

    /* Convert encrypt message from hex string to byte array */


    /* Decrypt the message */


    /* Print the result */
    printf("%s\n", buf_str);

    return 0;
}

/* Declare the list of shell commands */
static const shell_command_t shell_commands[] = {
    { "encrypt", "Encrypt input string", _encrypt_handler },
    { "decrypt", "Decrypt input string", _decrypt_handler },
    { NULL, NULL, NULL }
};

int main(void)
{
    /* Configure and start the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
