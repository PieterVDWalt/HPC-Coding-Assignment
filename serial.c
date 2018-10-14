#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include "dcdplugin.c"

typedef struct point point;

typedef int bool;
#define true 1
#define false 0

struct point {
    int index;
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
char *output_file;

struct pair * closest_k_pairs;
int amount_in_k;

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

int comparison_x(const void * elem1, const void * elem2)
{
    struct point f = *((struct point *)elem1);
    struct point s = *((struct point *)elem2);
    if (f.x > s.x) return  1;
    if (f.x < s.x) return -1;
    return 0;
}

int comparison_y(const void * elem1, const void * elem2)
{
    struct point f = *((struct point *)elem1);
    struct point s = *((struct point *)elem2);
    if (f.y > s.y) return  1;
    if (f.y < s.y) return -1;
    return 0;
}

void check_inner_strip(struct point *inner_strip, int strip_size, float y_distance) {
    qsort(inner_strip, strip_size, sizeof(*inner_strip), comparison_y);

    int x;
    for (x = 0; x < strip_size - 1; x++) {
        int y;
        for (y = 1; y < strip_size; y++) {
        if ( (strcmp(inner_strip[x].set,"a") == 0  && strcmp(inner_strip[y].set,"b") == 0) || (strcmp(inner_strip[x].set,"b") == 0 && strcmp(inner_strip[y].set,"a") == 0) ) {
            if (abs(inner_strip[x].y - inner_strip[y].y) < y_distance) {
                float dist = sqrt( pow(inner_strip[x].x - inner_strip[y].x , 2) + pow(inner_strip[x].y - inner_strip[y].y , 2) + pow(inner_strip[x].z - inner_strip[y].z , 2) );
                int new_a;
                int new_b;
                if (strcmp(inner_strip[x].set,"a") == 0) {
                    new_a = inner_strip[x].index;
                    new_b = inner_strip[y].index;
                } else {
                    new_a = inner_strip[y].index;
                    new_b = inner_strip[x].index;
                }
                if (amount_in_k < glob_k) {

                    if (amount_in_k == 0) {
                        struct pair temp;
                        temp.a = new_a;
                        temp.b = new_b;
                        temp.distance = dist;
                        closest_k_pairs[0] = temp;
                        amount_in_k++;
                    } else {
                        int z;
                        bool already_used = false;
                        for (z = 0; z < amount_in_k; z++) {
                            if ((closest_k_pairs[z].a == new_a && closest_k_pairs[z].b == new_b) || (closest_k_pairs[z].b == new_a && closest_k_pairs[z].a == new_b)) {
                                already_used = true;
                                break;
                            }
                        }
                        if (!already_used) {
                            int new_pos = 0;
                            for (z = 0; z < amount_in_k; z++) {
                                if (closest_k_pairs[z].distance < dist) {
                                    new_pos++;
                                } else {
                                    break;
                                }
                            }
                            struct pair temp;
                            temp.a = new_a;
                            temp.b = new_b;
                            temp.distance = dist;
                            for (z = amount_in_k-1; z >= new_pos; z--) {
                                closest_k_pairs[z+1] = closest_k_pairs[z];
                            }
                            closest_k_pairs[new_pos] = temp;
                            amount_in_k++;
                        }
                    }

                } else {
                    if (dist < closest_k_pairs[glob_k-1].distance) {
                        int z;
                        bool already_used = false;
                        for (z = 0; z < amount_in_k; z++) {
                            if ((closest_k_pairs[z].a == new_a && closest_k_pairs[z].b == new_b) || (closest_k_pairs[z].b == new_a && closest_k_pairs[z].a == new_b)) {
                                already_used = true;
                                break;
                            }
                        }

                        if (!already_used) {
                            int t;
                            int new_pos = glob_k-1;
                            for (t = glob_k-2; t >= 0; t--) {
                                if (dist < closest_k_pairs[t].distance) {
                                    new_pos--;
                                } else {
                                    break;
                                }
                            }
                            struct pair temp;
                            temp.a = new_a;
                            temp.b = new_b;
                            temp.distance = dist;
                            for (t = glob_k-1; t > new_pos; t--) {
                                closest_k_pairs[t] = closest_k_pairs[t-1];
                            }
                            closest_k_pairs[new_pos] = temp;
                        }
                    }
                }
            } else {
                break;
            }
        }
        }
    }
}

void calculate_closest_pairs(struct point *set, int total_size) {
    int p;
    int x;
    if (pow(total_size,2) <= glob_k * 4) {
        for (x = 0; x < total_size-1; x++) {
            int y;
            for (y = 1; y < total_size; y++) {
                if ( (strcmp(set[x].set,"a") == 0 && strcmp(set[y].set,"b") == 0) || (strcmp(set[x].set,"b") == 0 && strcmp(set[y].set,"a") == 0) ) {
                    float dist = sqrt( pow(set[x].x - set[y].x , 2) + pow(set[x].y - set[y].y , 2) + pow(set[x].z - set[y].z , 2) );
                    int new_a;
                    int new_b;
                    if (strcmp(set[x].set,"a") == 0) {
                        new_a = set[x].index;
                        new_b = set[y].index;
                    } else {
                        new_a = set[y].index;
                        new_b = set[x].index;
                    }
                    if (amount_in_k < glob_k) {
                        if (amount_in_k == 0) {
                            struct pair temp;
                            temp.a = new_a;
                            temp.b = new_b;
                            temp.distance = dist;
                            closest_k_pairs[0] = temp;
                            amount_in_k++;
                        } else {
                            int z;
                            bool already_used = false;
                            for (z = 0; z < amount_in_k; z++) {
                                if ((closest_k_pairs[z].a == new_a && closest_k_pairs[z].b == new_b) || (closest_k_pairs[z].b == new_a && closest_k_pairs[z].a == new_b)) {
                                    already_used = true;
                                    break;
                                }
                            }
                            if (!already_used) {
                                int new_pos = 0;
                                for (z = 0; z < amount_in_k; z++) {
                                    if (closest_k_pairs[z].distance < dist) {
                                        new_pos++;
                                    } else {
                                        break;
                                    }
                                }
                                struct pair temp;
                                temp.a = new_a;
                                temp.b = new_b;
                                temp.distance = dist;
                                for (z = amount_in_k-1; z >= new_pos; z--) {
                                    closest_k_pairs[z+1] = closest_k_pairs[z];
                                }
                                closest_k_pairs[new_pos] = temp;
                                amount_in_k++;
                            }
                        }

                    } else {
                        if (dist < closest_k_pairs[glob_k-1].distance) {
                            int z;
                            bool already_used = false;
                            for (z = 0; z < amount_in_k; z++) {
                                if ((closest_k_pairs[z].a == new_a && closest_k_pairs[z].b == new_b) || (closest_k_pairs[z].b == new_a && closest_k_pairs[z].a == new_b)) {
                                    already_used = true;
                                    break;
                                }
                            }

                            if (!already_used) {
                                int t;
                                int new_pos = glob_k-1;
                                for (t = glob_k-2; t >= 0; t--) {
                                    if (dist < closest_k_pairs[t].distance) {
                                        new_pos--;
                                    } else {
                                        break;
                                    }
                                }
                                struct pair temp;
                                temp.a = new_a;
                                temp.b = new_b;
                                temp.distance = dist;

                                for (t = glob_k-1; t > new_pos; t--) {
                                    closest_k_pairs[t] = closest_k_pairs[t-1];
                                }
                                closest_k_pairs[new_pos] = temp;
                            }
                        }
                    }
                }
            }
        }
    } else {
        int middle;
        if (total_size % 2 != 0) {
            middle = (int)((total_size / 2)+1);
        } else {
            middle = (int)(total_size / 2);
        }
        struct point left[middle];
        struct point right[total_size - middle];
        memcpy(left, &set[0], (middle)*sizeof(struct point));
        memcpy(right, &set[middle], (total_size - middle)*sizeof(struct point));
        calculate_closest_pairs(left, middle);
        calculate_closest_pairs(right, total_size - middle);
        int strip_size = 0;
        int left_checker = middle - 1;
        int right_checker = 0;
        float middle_x = (left[middle-1].x + right[0].x) / 2;
        float lower_limit = middle_x - closest_k_pairs[amount_in_k-1].distance;
        float upper_limit = middle_x + closest_k_pairs[amount_in_k-1].distance;
        while (left[left_checker].x > lower_limit && left_checker >= 0) {
            left_checker--;
            strip_size++;
        }
        while (right[right_checker].x < upper_limit && right_checker < (total_size - middle)) {
            right_checker++;
            strip_size++;
        }
        struct point inner_strip[strip_size];
        int q;
        for (q = left_checker; q < middle - 1; q++) {
            inner_strip[q - left_checker] = left[q + 1];
        }
        for (q = 0; q < right_checker; q++) {
            inner_strip[middle - left_checker - 1 + q] = right[q];
        }
        check_inner_strip(inner_strip, strip_size, closest_k_pairs[amount_in_k-1].distance);
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
          set[p].index = final_A[p];
          set[p].set = "a";
          set[p].x = dcd->x[final_A[p]];
          set[p].y = dcd->y[final_A[p]];
          set[p].z = dcd->z[final_A[p]];
      }
      for (p = final_a_size; p < final_a_size + final_b_size; p++) {
          set[p].index = final_B[p-final_a_size];
          set[p].set = "b";
          set[p].x = dcd->x[final_B[p-final_a_size]];
          set[p].y = dcd->y[final_B[p-final_a_size]];
          set[p].z = dcd->z[final_B[p-final_a_size]];
      }
      qsort (set, final_a_size + final_b_size, sizeof(*set), comparison_x);
      calculate_closest_pairs(set, final_a_size + final_b_size);
      FILE *fp;
      fp = fopen(output_file, "a");
      for (p = 0; p < glob_k; p++) {
          char temp_timestep[100];
          sprintf(temp_timestep ,"%d" , x);
          char temp_a[10];
          char temp_b[10];
          sprintf(temp_a ,"%d" , closest_k_pairs[p].a);
          sprintf(temp_b ,"%d" , closest_k_pairs[p].b);
          char temp_dist[10];
          sprintf(temp_dist ,"%.6f" , closest_k_pairs[p].distance);
          strcat(temp_timestep, ", ");
          strcat(temp_timestep, temp_a);
          strcat(temp_timestep, ", ");
          strcat(temp_timestep, temp_b);
          strcat(temp_timestep, ", ");
          strcat(temp_timestep, temp_dist);
          strcat(temp_timestep, "\n");
          fprintf(fp, temp_timestep);
      }
      fclose(fp);
      memset(closest_k_pairs, 0, sizeof(closest_k_pairs));
      amount_in_k = 0;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int x;
    char *input_file = "";
    char buf[1000];
    if (argc < 5) {
        return 1;
    } else {
        for (x = 0; x < argc; x++) {
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
    counter = 0;

    while (strchr(original_A, ',')) {
        char *temp = strchr(original_A, ',');
        int index = (int)(temp - original_A);
        char *substr = malloc(sizeof(char)*40);
        strncpy(substr, original_A, index);
        final_A[counter] = malloc(strlen(substr) + 1);
        strncpy(final_A[counter], substr, strlen(substr));
        memmove(original_A, original_A+index+1, strlen(original_A));
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
