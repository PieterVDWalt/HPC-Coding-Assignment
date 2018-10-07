#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include "dcdplugin.c"

typedef struct point point;

struct point {
    char *set;
    float x;
    float y;
    float z;
};

struct pair {
    int a;
    int b;
    float distance;
};

int glob_k;

struct pair * closest_k_pairs;
int amount_in_k;

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

int comparison(const void * elem1, const void * elem2)
{
    struct point f = *((struct point *)elem1);
    struct point s = *((struct point *)elem2);
    if (f.x > s.x) return  1;
    if (f.x < s.x) return -1;
    return 0;
}

void calculate_closest_pairs(struct point *set, int total_size) {
    qsort (set, total_size, sizeof(*set), comparison);
    // /printf("SORTED STRUCT:\n");
    int p;
    /*for (p = 0; p < total_size; p++) {
        printf("pos: %d, set: %s, x: %.6f, y: %.6f, z: %.6f\n", p, set[p].set, set[p].x, set[p].y, set[p].z);
    }*/
    int x;
    if (pow(total_size,2) <= glob_k * 4) {
        for (x = 0; x < total_size-1; x++) {
            int y;
            for (y = 1; y < total_size; y++) {
                if ( (strcmp(set[x].set,"a") == 0 && strcmp(set[y].set,"b") == 0) || (strcmp(set[x].set,"b") == 0 && strcmp(set[y].set,"a") == 0) ) {
                    float dist = sqrt( pow(set[x].x + set[y].x , 2) + pow(set[x].y + set[y].y , 2) + pow(set[x].z + set[y].z , 2) );
                    if (amount_in_k < glob_k) {
                        int new_pos = 0;
                        int z;
                        for (z = 0; z < amount_in_k; z++) {
                            if (closest_k_pairs[z].distance < dist) {
                                new_pos++;
                            } else {
                                break;
                            }
                        }
                        struct pair temp;
                        temp.a = x;
                        temp.b = y;
                        temp.distance = dist;
                        for (z = amount_in_k-1; z >= new_pos; z--) {
                            closest_k_pairs[z+1] = closest_k_pairs[z];
                        }
                        closest_k_pairs[new_pos] = temp;
                        amount_in_k++;
                    } else {
                        //check if smaller than last element.
                        //if smaller
                        //insert into correct position, move rest of elements on, lose last element
                    }
                }
            }
        }
    } else {
        int middle = (int)(total_size / 2);
        struct pair left[middle];
        struct pair right[total_size - middle];
    }
}

int read_dcd_file(char *trajectory_file, char **set_A, int A_size, char **set_B, int B_size) {
    molfile_timestep_t timestep;
    dcdhandle *dcd;
    void *file_data;
    char *end;
    int nr_atoms = 0;
    int final_a_size = 0;
    int final_b_size = 0;

    int r;

    for (r = 0; r < A_size; r++) {
        char *curr;
        char *orig;
        curr = malloc(strlen(set_A[r]) + 1);
        orig = malloc(strlen(set_A[r]) + 1);
        strcpy(curr, set_A[r]);
        strcpy(orig, set_A[r]);
        if (strchr(set_A[r], '-')) {
            char *to = strchr(set_A[r], '-');
            char *from = strtok(curr, "-");
            memmove(to, to+1, strlen(to));
            int f_from = strtol(from, &end, 10);
            int f_to = strtol(to, &end, 10);
            int difference = f_to - f_from + 1;
            final_a_size = final_a_size + difference;
            strcpy(set_A[r], orig);
        } else {
            final_a_size++;
        }
        free(curr);
        free(orig);
    }

    for (r = 0; r < B_size; r++) {
        char *curr;
        char *orig;
        curr = malloc(strlen(set_B[r]) + 1);
        orig = malloc(strlen(set_B[r]) + 1);
        strcpy(curr, set_B[r]);
        strcpy(orig, set_B[r]);
        if (strchr(set_B[r], '-')) {
            char *to = strchr(set_B[r], '-');
            char *from = strtok(curr, "-");
            memmove(to, to+1, strlen(to));
            int f_from = strtol(from, &end, 10);
            int f_to = strtol(to, &end, 10);
            int difference = f_to - f_from + 1;
            final_b_size = final_b_size + difference;
            strcpy(set_B[r], orig);
        } else {
            final_b_size++;
        }
        free(curr);
        free(orig);
    }

    int final_A[final_a_size];
    int final_B[final_b_size];
    int final_index = 0;

    for (r = 0; r < A_size; r++) {
        char *curr;
        curr = malloc(strlen(set_A[r]) + 1);
        strcpy(curr, set_A[r]);
        if (strchr(set_A[r], '-')) {
            char *to = strchr(set_A[r], '-');
            char *from = strtok(curr, "-");
            memmove(to, to+1, strlen(to));
            int f_from = strtol(from, &end, 10);
            int f_to = strtol(to, &end, 10);
            int t;
            for (t = f_from; t <= f_to; t++) {
                final_A[final_index] = t;
                final_index++;
            }
        } else {
            int temp = strtol(set_A[r], &end, 10);
            final_A[final_index] = temp;
            final_index++;
        }
        free(curr);
    }

    final_index = 0;

    for (r = 0; r < B_size; r++) {
        char *curr;
        curr = malloc(strlen(set_B[r]) + 1);
        strcpy(curr, set_B[r]);
        if (strchr(set_B[r], '-')) {
            char *to = strchr(set_B[r], '-');
            char *from = strtok(curr, "-");
            memmove(to, to+1, strlen(to));
            int f_from = strtol(from, &end, 10);
            int f_to = strtol(to, &end, 10);
            int t;
            for (t = f_from; t <= f_to; t++) {
                final_B[final_index] = t;
                final_index++;
            }
        } else {
            int temp = strtol(set_B[r], &end, 10);
            final_B[final_index] = temp;
            final_index++;
        }
        free(curr);
    }

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

      int p;

      struct point set[final_a_size + final_b_size];
      for (p = 0; p < final_a_size; p++) {
          set[p].set = "a";
          set[p].x = dcd->x[final_A[p]];
          set[p].y = dcd->y[final_A[p]];
          set[p].z = dcd->z[final_A[p]];
      }
      for (p = final_a_size; p < final_a_size + final_b_size; p++) {
          set[p].set = "b";
          set[p].x = dcd->x[final_B[p-final_a_size]];
          set[p].y = dcd->y[final_B[p-final_a_size]];
          set[p].z = dcd->z[final_B[p-final_a_size]];
      }

      calculate_closest_pairs(set, final_a_size + final_b_size);

    }
    return 0;
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
            glob_k = k;
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

    fclose(text_input);

    closest_k_pairs = (struct pair *)malloc(glob_k * sizeof(struct pair));
    amount_in_k = 0;

    read_dcd_file(trajectory_file, final_A, final_a_size, final_B, final_b_size);

    return 0;
}





















/**/
