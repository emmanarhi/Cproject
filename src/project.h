#ifndef _PROJECT__H_
#define _PROJECT__H_

struct Student{
    char student_number[7];
    char *first_name;
    char *last_name;
    int exercise_points[6];
    int total_points;
};

int add_student(struct Student **students, int *student_count, 
    char *student_number, char *last_name, char *first_name);

int update_points(struct Student *students, int *student_count, char *student_number, int round, int point);

void print_students(struct Student *students, int *student_count);

int save_to_file(struct Student *students, char *filename, int *student_count);

int load_from_file(struct Student **students, const char *filename, int *student_count);

void free_students(struct Student **students, int *student_count);

int compare_points(const void *a, const void *b);

#endif //! _PROJECT__H_
