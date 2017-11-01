#include <stdio.h>
#include <stdlib.h>
#include <sodium.h>
#include <string.h>
#include <json.h>
#include <time.h>
#include <basedir.h>
#include <basedir_fs.h>
#include <resolv.h>
#include <sys/types.h>
#include <sys/stat.h>

void update_seq_signed_at(struct json_object *jobj)
{
    struct json_object *jobj2;
    json_object_object_get_ex(jobj, "seq", &jobj2);
    int seq = json_object_get_int(jobj2);
    json_object_object_add(jobj, "seq", json_object_new_int(++seq));

    // calculate and format current time
    time_t t = time(NULL);
    char dateTimeStr[20];
    strftime(dateTimeStr, 20, "%F %T", gmtime(&t));

    json_object_object_add(jobj, "signed_at",
                           json_object_new_string(dateTimeStr));
}

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
/*    free(handle);*/

    char *dataDir;
    asprintf(&dataDir, "%s/json-signer", userDataDir);

    struct stat sb;
    if (-1 == stat(dataDir, &sb)) {
        if (0 != xdgMakePath(dataDir, S_IRWXU)) {
            perror("unable to create directory");
            return EXIT_FAILURE;
        }
    }

    free(dataDir);

    char *secretKeyFile, *publicKeyFile;
    asprintf(&secretKeyFile, "%s/json-signer/secret.key", userDataDir);
    asprintf(&publicKeyFile, "%s/json-signer/public.key", userDataDir);

    unsigned char pk[crypto_sign_PUBLICKEYBYTES];
    unsigned char sk[crypto_sign_SECRETKEYBYTES];

    // read keys
    FILE *secretKey = fopen(secretKeyFile, "r");
    if (!secretKey) {
        secretKey = fopen(secretKeyFile, "w");
        FILE *publicKey = fopen(publicKeyFile, "w");
        if (NULL == secretKey || NULL == publicKey) {
            perror("unable to open files for writing");
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

    free(secretKeyFile);
    free(publicKeyFile);

    // open the JSON file
    if (2 > argc) {
        perror("missing file parameter");
        return EXIT_FAILURE;
    }

    FILE *f = fopen(argv[1], "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    rewind(f);

    char *string = malloc(fsize + 1);
    memset(string, 0, fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);

    struct json_object *jobj;
    jobj = json_tokener_parse(string);
    free(string);
    update_seq_signed_at(jobj);
    const char *output = json_object_to_json_string(jobj);

    // write updated file
    f = fopen(argv[1], "w");
    fwrite(output, strlen(output), 1, f);
    fclose(f);

    // free json
    json_object_put(jobj);

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

    // clear private key from memory
    memset(sk, 0, crypto_sign_SECRETKEYBYTES);

    char b64[1024];
    int enc_sig_len = b64_ntop(sig, crypto_sign_BYTES, b64, sizeof(b64));

    char *signatureFile;
    asprintf(&signatureFile, "%s.sig", argv[1]);
    f = fopen(signatureFile, "w");
    fwrite(b64, enc_sig_len, 1, f);
    fclose(f);
    free(signatureFile);

    return EXIT_SUCCESS;
}
