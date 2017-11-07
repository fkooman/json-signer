/**
 * indent -kr -i 4 -nut json-signer.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <sodium.h>
#include <string.h>
#include <json.h>
#include <time.h>
#include <resolv.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>

char *get_xdg_data_home()
{
    // $XDG_DATA_HOME defines the base directory relative to which user 
    // specific data files should be stored. If $XDG_DATA_HOME is either not 
    // set or empty, a default equal to $HOME/.local/share should be used.
    char *xdgDataHome = getenv("XDG_DATA_HOME");
    if (NULL != xdgDataHome) {
        return xdgDataHome;
    }

    char *homeDir = getenv("HOME");
    if (NULL == homeDir) {
        homeDir = "/";
    }

    asprintf(&xdgDataHome, "%s/.local/share", homeDir);

    return xdgDataHome;
}

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

void update_json_file(const char *json_file)
{
    FILE *f = fopen(json_file, "rb");
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
    f = fopen(json_file, "w");
    fwrite(output, strlen(output), 1, f);
    fclose(f);

    // free json
    json_object_put(jobj);
}

int mkdirr(char *dirName)
{
    if (0 == strcmp("/", dirName)) {
        return 0;
    }
    char *dirNameCopy = strdup(dirName);
    mkdirr(dirname(dirNameCopy));
    struct stat sb;
    if (-1 == stat(dirName, &sb)) {
        return mkdir(dirName, S_IRWXU);
    }

    return 0;
}

void sign_json_file(const char *json_file, unsigned char *sk)
{
    // sign it
    FILE *f = fopen(json_file, "r");
    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
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
    asprintf(&signatureFile, "%s.sig", json_file);
    f = fopen(signatureFile, "w");
    fwrite(b64, enc_sig_len, 1, f);
    fclose(f);
    free(signatureFile);
}

int main(int argc, char *argv[argc + 1])
{
    if (sodium_init() < 0) {
        perror("unable to initialize libsodium");
        return EXIT_FAILURE;
    }

    char *dataDir = NULL;
    asprintf(&dataDir, "%s/json-signer", get_xdg_data_home());

    if (-1 == mkdirr(dataDir)) {
        perror("unable to create directory");
        return EXIT_FAILURE;
    }



    char *secretKeyFile, *publicKeyFile;
    asprintf(&secretKeyFile, "%s/secret.key", dataDir);
    asprintf(&publicKeyFile, "%s/public.key", dataDir);


    free(dataDir);

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

    update_json_file(argv[1]);
    sign_json_file(argv[1], sk);

    return EXIT_SUCCESS;
}
