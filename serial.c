#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dcdplugin.c"

int main(int argc, char *argv[]) {
    int x;
    char *input_file = "";
    char *output_file = "";
    char buf[1000];
    if (argc < 5) {
        return 1;
    } else {
        for (x = 0; x < argc; x++) {
            printf("Argument %d is %s\n",x,argv[x]);
            if (strncmp(argv[x],"-i", 2) == 0) {
                input_file = argv[x+1];
            } else if (strncmp(argv[x],"-o", 2) == 0) {
                output_file = argv[x+1];
            }
        }
    }

    FILE *text_input;
    text_input = fopen(input_file, "r");

    printf("Here\n");

    if (!text_input) {
        printf("Error\n");
        return 1;
    }
    int counter = 0;
    char *trajectory_file = "";
    int k;
    char *end;
    char *set_A = "";
    char *set_B = "";

    while (fgets(buf,1000, text_input)!=NULL) {
        if (counter == 0) {
            trajectory_file = buf;
            int len = strlen(trajectory_file);
            trajectory_file[len-1] = '\0';
            printf("trajectory_file: %s\n",trajectory_file);
        }
        if (counter == 2) {
            char *temp_k = buf;
            int len = strlen(temp_k);
            temp_k[len-1] = '\0';
            k = strtol(temp_k, &end, 10);
            printf("k is %d\n",k);
        }
        if (counter == 4) {
            set_A = buf;
            int len = strlen(set_A);
            set_A[len-1] = '\0';
            printf("set_A: %s\n",set_A);
        }
        if (counter == 6) {
            set_B = buf;
            int len = strlen(set_B);
            set_B[len-1] = '\0';
            printf("set_B: %s\n",set_B);
        }
        counter++;
    }


    fclose(text_input);
    return 0;
}
