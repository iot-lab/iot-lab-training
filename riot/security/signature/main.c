#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/* Add required includes here */


/* Declare here the buffers where the asymmetric keys are stored */


/* Declare here the buffers where the signature and its hexadecimal representation are stored */



/* Implement the key command handler here */
static int _key_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    /* Create the new keypair */


    /* Print the new keypair */


    return 0;
}

/* Implement the sign command handler here */
static int _sign_handler(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s <message>\n", argv[0]);
        return 1;
    }

    /* Clear signature temporary buffer content */
    memset(signature, 0, sizeof(signature));

    /* Generate the signature here */


    /* Convert the signature byte array to a string of hex characters */


    /* Print the signature */
    printf("%s\n", signature_hex);

    return 0;
}

/* Implement the verify command handler here */
static int _verify_handler(int argc, char **argv)
{
    if (argc != 3) {
        printf("usage: %s <message> <signature>\n", argv[0]);
        return 1;
    }

    /* Clear signature temporary buffer content */
    memset(signature, 0, sizeof(signature));

    /* Convert the input signature in hex to a byte array here */


    /* Verify the signature here */


    return 0;
}

/* Declare the list of shell commands */
static const shell_command_t shell_commands[] = {
    { "key", "Generate a new pair of keys", _key_handler },
    { "sign", "Compute the signature of a message", _sign_handler },
    { "verify", "Verify the signature of a message", _verify_handler },
    { NULL, NULL, NULL }
};

int main(void)
{
    /* Initialize the random seed */
    random_init(0);

    /* Configure and start the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE + 32];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE + 32);

    return 0;
}
