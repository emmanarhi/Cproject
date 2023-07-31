#include "project.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO:: implement your project here!

int add_student(struct Student **students, int *student_count, 
    char *student_number, char *last_name, char *first_name){

    //Tarkistetaan, onko opiskelija jo lisätty students listaan
    for (int i = 0; i < *student_count; i++){
        if (strcmp((*students)[i].student_number, student_number) == 0){
            printf("Student is already in the database\n");
            return 0;
        }
    }

    *students = realloc(*students, (*student_count + 1) * sizeof(struct Student));

    struct Student new_student;

    if (strlen(student_number) <= 6){
        strcpy(new_student.student_number, student_number);
    }else{
        printf("Studen number cannot be more than 6 digits\n");
        return 0;
    }

    int firstname_len = strlen(first_name);
    int lastname_len = strlen(last_name);
    new_student.first_name = (char *)malloc((firstname_len + 1) * sizeof(char));
    new_student.last_name = (char *)malloc((lastname_len + 1) * sizeof(char));

    strcpy(new_student.first_name, first_name);
    strcpy(new_student.last_name, last_name);

    for (int i = 0; i < 6; i++){
        new_student.exercise_points[i] = 0;
    }

    new_student.total_points = 0;

    (*students)[*student_count] = new_student;
    (*student_count)++;
    return 1;
}

int update_points(struct Student *students, int *student_count, char *student_number, int round, int point){

    int i_student = -1; 
    for (int i  = 0; i < *student_count; i++){
        if (strcmp(students[i].student_number, student_number) == 0){
            i_student = i;
        }
    }
    //Tarkistetaan, että opiskelija on students listassa
    if (i_student == -1){
        printf("Student is not in the database\n");
        return 0;
    }

    //Tarkistetaan, että round on välillä 1-6 ja point > 0

    if ((round > 6) || (round < 1)){
        printf("There are only 6 rounds\n");
        return 0;
    }

    if (point < 0){
        printf("Points cannot be negative\n");
        return 0;
    }

    students[i_student].exercise_points[round - 1] = point;

    int points = 0;

    for (int i = 0; i < 6; i++){
        points += students[i_student].exercise_points[i];
    }

    students[i_student].total_points = points;

    return 1;
}

void print_students(struct Student *students, int *student_count){

    qsort(students, *student_count, sizeof(struct Student), compare_points);

    for (int i = 0; i < *student_count; i++){
        printf("%s %s %s", students[i].student_number, students[i].last_name, students[i].first_name);
        for (int j = 0; j < 6; j++){
            printf(" %d", students[i].exercise_points[j]);
        }
        printf(" %d\n", students[i].total_points);
    }

}

//qsort käyttämistä varten tarvitaan compare funktio
int compare_points(const void *a, const void *b){
    const struct Student *student_a = (const struct Student *)a;
    const struct Student *student_b = (const struct Student *)b;

    return student_b->total_points - student_a->total_points;
}

int save_to_file(struct Student *students, char *filename, int *student_count){
    FILE *file = fopen(filename, "w");

    //Tarkistetaan, että tiedosto on olemassa
    if (!file){
        printf("File couldn't be found");
        return 0;
    }

    for (int i = 0; i < *student_count; i++){
        fprintf(file, "%s %s %s", students[i].student_number, students[i].last_name, students[i].first_name);
        for (int j = 0; j < 6; j++){
            fprintf(file, " %d", students[i].exercise_points[j]);
        }
        fprintf(file, " %d\n", students[i].total_points);
    }
    fclose(file);
    return 1;
}

int load_from_file(struct Student **students, const char *filename, int *student_count){
    FILE *file = fopen(filename, "r");

    struct Student *temp = NULL;

    if (!file){
        printf("File couldn't be found\n");
        return 0;
    }

    char line[1001], student_number[7], last_name[100], first_name[100];
    int exercise_points[6], total_points;
    int count = 0;

    //Käydään läpi kaikki rivit ja tarkistetaan, että rivit ok
    while (fgets(line, sizeof(line), file)){
        if(sscanf(line, "%6s %s %s %d %d %d %d %d %d %d", student_number, last_name, first_name, &exercise_points[0], &exercise_points[1],
            &exercise_points[2], &exercise_points[3], &exercise_points[4], &exercise_points[5], &total_points) == 10){

                if(add_student(&temp, &count, student_number, last_name, first_name)){
                    for (int i = 0; i < 6; i++){
                        temp[count - 1].exercise_points[i] = exercise_points[i];
                    }

                    temp[count - 1].total_points = total_points;
                }else{
                    count = count - 1;
                    free_students(&temp, &count);
                    printf("Couldn't add a student in the file to the database\n");
                    fclose(file);
                    return 0;
                }
        }else{
            count = count - 1;
            free_students(&temp, &count);
            fclose(file);
            printf("Students in file are not valid\n");
            return 0;
        }
    }

    free_students(students, student_count);
    *students = temp;
    *student_count = count;
    fclose(file);
    return 1;
}

void free_students(struct Student **students, int *student_count){
    for (int i = 0; i < *student_count; i++) {
        free((*students)[i].first_name);
        free((*students)[i].last_name);
    }
    free(*students);
    *student_count = 0;
}

int main(void){
    struct Student *students = NULL;

    students = malloc(sizeof(struct Student));

    int student_count = 0;
    char student_number[10];
    char command;
    char first_name[100];
    char last_name[100];
    char file_name[100];
    char input[1001];
    int round, points;

    while(1){
        fgets(input, sizeof(input), stdin);
        sscanf(input, "%c", &command);

    switch(command)
    {
        case 'A':
            if (sscanf(input, "%*c %s %s %s", student_number, last_name, first_name) == 3){
                if (add_student(&students, &student_count, student_number, last_name, first_name)){
                    printf("SUCCES\n");
                    if (strlen(student_number) == 6){
                        printf("yey");
                    }
                }
            }else{
                printf("There should be 3 arguments\n");
            }
            break;
        case 'U':
            sscanf(input, "%*c %6s %d %d", student_number, &round, &points);
            if (update_points(students, &student_count, student_number, round, points)){
                printf("SUCCES\n");
            }
            break;
        case 'L':
            print_students(students, &student_count);
            printf("SUCCES\n");
            break;
        case 'W':
            sscanf(input, "%*c %s", file_name);
            if (save_to_file(students, file_name, &student_count)){
                printf("SUCCES\n");
            }
            break;
        case 'O':
            sscanf(input, "%*c %s", file_name);
            if (load_from_file(&students, file_name, &student_count)){
                printf("SUCCES\n");
            }
            break;
        case 'Q':
            free_students(&students, &student_count);
            printf("SUCCESS\n");
            return 0;
        default:
            printf("Invalid command %c\n", command);
            break;
    }

    }
    return 0;
}
