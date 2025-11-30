#ifndef DATABASE_H
#define DATABASE_H

#define MAX_RECORDS 100
#define MAX_STRING 100

// Doctor Structure
typedef struct {
    int id;
    char name[MAX_STRING];
    char specialization[MAX_STRING];
    char phone[20];
    char email[MAX_STRING];
} Doctor;

// Patient Structure
typedef struct {
    int id;
    char name[MAX_STRING];
    int age;
    char gender[10];
    char phone[20];
    char disease[MAX_STRING];
    char admission_date[20];
} Patient;

// Employee Structure
typedef struct {
    int id;
    char name[MAX_STRING];
    char position[MAX_STRING];
    char department[MAX_STRING];
    char phone[20];
    char email[MAX_STRING];
} Employee;

// Function declarations
void print_http_header();
void parse_query_string(char *query, char params[][2][MAX_STRING], int *count);
char* url_decode(char *str);
void json_escape(char *dest, const char *src);

// Doctor functions
int load_doctors(Doctor doctors[]);
int save_doctors(Doctor doctors[], int count);
void list_doctors();
void add_doctor(char *json_data);
void update_doctor(char *json_data);
void delete_doctor(int id);
void get_doctor(int id);
void count_doctors();

// Patient functions
int load_patients(Patient patients[]);
int save_patients(Patient patients[], int count);
void list_patients();
void add_patient(char *json_data);
void update_patient(char *json_data);
void delete_patient(int id);
void get_patient(int id);
void count_patients();

// Employee functions
int load_employees(Employee employees[]);
int save_employees(Employee employees[], int count);
void list_employees();
void add_employee(char *json_data);
void update_employee(char *json_data);
void delete_employee(int id);
void get_employee(int id);
void count_employees();

#endif
