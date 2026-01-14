
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stub.h"


void trim_newline(char *str) {
    str[strcspn(str, "\n")] = 0;
}


int main() {
    
    char* horse_file;
    char* warrior_file;
    int buf_size = 100;
    char cmd[2048];

    horse_file = malloc(buf_size*sizeof(char));
    if (horse_file == NULL){
        perror("failed to allocate memory for reading file path\n");
        exit(1);
    }
    fprintf(stdout, "Please enter first file path : ");
    if (fgets(horse_file, buf_size, stdin)) {
        printf("horse file path %s", horse_file);
    } else {
        perror("failed to read horse file path");
        exit(1);
    }
    trim_newline(horse_file);

    warrior_file = malloc(buf_size*sizeof(char));
    if (warrior_file == NULL){
        perror("failed to allocate memory for reading file path\n");
        exit(1);
    }
    fprintf(stdout, "Please enter second file path : ");
    if (fgets(warrior_file, buf_size, stdin)) {
        printf("worrior file path %s", warrior_file);
    } else {
        perror("failed to read worrior file path");
        exit(1);
    }
    trim_newline(warrior_file);

    snprintf(cmd, sizeof(cmd), "xxd -i -n horse %s > include/horse_data.h", horse_file);
    if (system(cmd) != 0) {
        fprintf(stderr, "Failed to convert horse file\n");
        exit(1);
    }

    snprintf(cmd, sizeof(cmd), "xxd -i -n warrior %s > include/warrior_data.h", warrior_file);
    if (system(cmd) != 0) {
        fprintf(stderr, "Failed to convert warrior file\n");
        exit(1);
    }

    printf("Compiling bundle...\n");
    const char *stub_code = 
        "#include \"stub.h\"\n"
        "#include \"horse_data.h\"\n"
        "#include \"warrior_data.h\"\n"

        "int main(){\n"
        "    wrap(horse, horse_len, warrior, warrior_len);\n"
        "    return 0;\n"
        "}\n";

    const char *compile_cmd = "gcc -x c - -o bundle_launcher -I './include' -L './lib' -lstub";

    FILE *fp = popen(compile_cmd, "w");
    if (fp == NULL) {
        perror("popen failed");
        return 1;
    }
    fprintf(fp, "%s", stub_code);

    int status = pclose(fp);
    if (status == 0) {
        printf("Compilation successful: bundle_launcher created.\n");
    } else {
        fprintf(stderr, "Compilation failed with status %d\n", status);
    }


    free(horse_file);
    free(warrior_file);
    
    return 0;
}

