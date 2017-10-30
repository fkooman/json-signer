#include <stdio.h>
#include <stdlib.h>
#include <sodium.h>
#include <glib.h>
#include <string.h>
#include <json.h>
#include <time.h>
#include <bsd/string.h>
#include <basedir.h>

int main(int argc, char *argv[argc + 1])
{
    if (sodium_init() < 0) {
        perror("unable to initialize libsodium");
        return EXIT_FAILURE;
    }

    // check if private key exists
    xdgHandle handle;
    if (!xdgInitHandle(&handle)) {
        perror("unable to init xdg");
        return EXIT_FAILURE;
    }
    const char *userDataDir = xdgDataHome(&handle);

    // keyFiles
    const size_t BUF_SIZE = 128;

    char secretKeyFile[BUF_SIZE] = "\0";
    char publicKeyFile[BUF_SIZE] = "\0";

    strlcat(secretKeyFile, userDataDir, BUF_SIZE);
    strlcat(secretKeyFile, "/json-signer/secret.key", BUF_SIZE);
    strlcat(publicKeyFile, userDataDir, BUF_SIZE);
    strlcat(publicKeyFile, "/json-signer/public.key", BUF_SIZE);

    unsigned char pk[crypto_sign_PUBLICKEYBYTES];
    unsigned char sk[crypto_sign_SECRETKEYBYTES];

    // read keys
    FILE *secretKey = fopen(secretKeyFile, "r");
    if (!secretKey) {
        secretKey = fopen(secretKeyFile, "w");
        FILE *publicKey = fopen(publicKeyFile, "w");
        if (NULL == secretKey || NULL == publicKey) {
            // unable to open file(s) for writing
            return EXIT_FAILURE;
        }
        // generate keys
        crypto_sign_keypair(pk, sk);
        // write keys to file
        fwrite(sk, crypto_sign_SECRETKEYBYTES, 1, secretKey);
        fwrite(pk, crypto_sign_PUBLICKEYBYTES, 1, publicKey);
        fclose(secretKey);
        fclose(publicKey);
    } else {
        // we only need the secret key
        fread(sk, crypto_sign_SECRETKEYBYTES, 1, secretKey);
        fclose(secretKey);
    }

    // open the JSON file
    if (2 > argc) {
        perror("missing file parameter");
        return EXIT_FAILURE;
    }

    FILE *f = fopen(argv[1], "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    rewind(f);

    char *string = malloc(fsize);
    fread(string, fsize, 1, f);
    fclose(f);

    struct json_object *jobj, *jobj2;
    jobj = json_tokener_parse(string);
    json_object_object_get_ex(jobj, "seq", &jobj2);
    int seq = json_object_get_int(jobj2);
    json_object_object_add(jobj, "seq", json_object_new_int(++seq));



    // calculate and format current time
    time_t t = time(NULL);
    char dateTimeStr[20];
    strftime(dateTimeStr, 20, "%F %T", gmtime(&t));

    json_object_object_add(jobj, "signed_at",
                           json_object_new_string(dateTimeStr));
    const char *output = json_object_to_json_string(jobj);

    // write updated file
    f = fopen(argv[1], "w");
    fwrite(output, strlen(output), 1, f);
    fclose(f);

    // sign it
    f = fopen(argv[1], "rb");
    fseek(f, 0, SEEK_END);
    fsize = ftell(f);
    rewind(f);

    unsigned char *data = malloc(fsize);
    fread(data, fsize, 1, f);
    fclose(f);

    unsigned char sig[crypto_sign_BYTES];
    crypto_sign_detached(sig, NULL, data, fsize, sk);
    free(data);
    gchar *b64sig = g_base64_encode(sig, crypto_sign_BYTES);
    f = fopen(g_strjoin("", argv[1], ".sig", NULL), "w");
    fwrite(b64sig, crypto_sign_BYTES, 1, f);
    fclose(f);
    g_free(b64sig);

    return EXIT_SUCCESS;
}
