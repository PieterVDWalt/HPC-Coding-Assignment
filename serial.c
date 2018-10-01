#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dcdplugin.c"

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

int calculate_closest_pairs(char *set_A, char *set_B, float *x_coords, float *y_coords, float *z_coords, int k) {
    return 0;
}

int read_dcd_file(char *trajectory_file, char *set_A, char *set_B, int k) {
    molfile_timestep_t timestep;
    dcdhandle *dcd;
    void *file_data;
    int nr_atoms = 0;

    file_data = open_dcd_read(trajectory_file, "dcd", &nr_atoms);

    if (!file_data) {
        printf("open_dcd_read failed for file %s\n", trajectory_file);
        return 1;
    }
    dcd = (dcdhandle *)file_data;

    timestep.coords = (float *) malloc(3 * sizeof(float) * nr_atoms);
    int x;
    for (x = 0; x < dcd->nsets; x++) {
      int rc = read_next_timestep(file_data, nr_atoms, &timestep);
      if (rc) {
        fprintf(stderr, "error in read_next_timestep on frame %d\n", x);
        return 1;
      }
      float *x_coords = dcd->x;
      float *y_coords = dcd->y;
      float *z_coords = dcd->z;

      int calced = calculate_closest_pairs(set_A, set_B, x_coords, y_coords, z_coords, k);
      return 0;
    }
}

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

    if (!text_input) {
        printf("Error\n");
        return 1;
    }
    int counter = 0;
    char *trajectory_file;
    int k;
    char *end;
    char *set_A;
    char *set_B;

    while (fgets(buf,1000, text_input)!=NULL) {
        if (counter == 0) {
            trajectory_file = malloc(strlen(buf) + 1);
            strcpy(trajectory_file, buf);
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
            set_A = malloc(strlen(buf) + 1);
            strcpy(set_A, buf);
            int len = strlen(set_A);
            set_A[len-1] = '\0';
            printf("set_A: %s\n",set_A);
        }
        if (counter == 6) {
            set_B = malloc(strlen(buf) + 1);
            strcpy(set_B, buf);
            int len = strlen(set_B);
            set_B[len-1] = '\0';
            printf("set_B: %s\n",set_B);
        }
        counter++;
    }

    char *original_A = malloc(strlen(set_A) + 1);
    char *original_B = malloc(strlen(set_B) + 1);
    strncpy(original_A, set_A, strlen(set_A));
    strncpy(original_B, set_B, strlen(set_B));

    int final_a_size = 0;
    int final_b_size = 0;

    while (strchr(set_A, ',')) {
        char *temp = strchr(set_A, ',');
        int index = (int)(temp - set_A);
        memmove(set_A, set_A+index+1, strlen(set_A));
        final_a_size++;
    }
    final_a_size++;

    while (strchr(set_B, ',')) {
        char *temp = strchr(set_B, ',');
        int index = (int)(temp - set_B);
        memmove(set_B, set_B+index+1, strlen(set_B));
        final_b_size++;
    }
    final_b_size++;

    char *final_A[final_a_size];
    char *final_B[final_b_size];
    counter = 0;

    while (strchr(original_A, ',')) {
        char *temp = strchr(original_A, ',');
        int index = (int)(temp - original_A);
        char *substr = malloc(sizeof(char)*40);
        strncpy(substr, original_A, index);
        final_A[counter] = malloc(strlen(substr) + 1);
        strncpy(final_A[counter], substr, strlen(substr));
        memmove(original_A, original_A+index+1, strlen(original_A));
        free(substr);
        counter++;
    }
    final_A[counter] = malloc(strlen(original_A) + 1);
    strncpy(final_A[counter], original_A, strlen(original_A));
    counter = 0;

    while (strchr(original_B, ',')) {
        char *temp = strchr(original_B, ',');
        int index = (int)(temp - original_B);
        char *substr = malloc(sizeof(char)*40);
        strncpy(substr, original_B, index);
        final_B[counter] = malloc(strlen(substr) + 1);
        strncpy(final_B[counter], substr, strlen(substr));
        memmove(original_B, original_B+index+1, strlen(original_B));
        free(substr);
        counter++;
    }
    final_B[counter] = malloc(strlen(original_B) + 1);
    strncpy(final_B[counter], original_B, strlen(original_B));

    for (x = 0; x < NELEMS(final_A); x++) {
        printf("final_A[%d]: %s\n",x,final_A[x]);
    }

    for (x = 0; x < NELEMS(final_B); x++) {
        printf("final_B[%d]: %s\n",x,final_B[x]);
    }
    fclose(text_input);

    //read_dcd_file(trajectory_file, final_A, final_B, k);

    return 0;
}





















/**/
