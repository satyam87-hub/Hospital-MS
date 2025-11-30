#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"

#define PATIENT_FILE "patients.dat"

int load_patients(Patient patients[]) {
    FILE *file = fopen(PATIENT_FILE, "rb");
    if (!file) return 0;
    
    int count = 0;
    while (fread(&patients[count], sizeof(Patient), 1, file) == 1) {
        count++;
    }
    fclose(file);
    return count;
}

int save_patients(Patient patients[], int count) {
    FILE *file = fopen(PATIENT_FILE, "wb");
    if (!file) return 0;
    
    for (int i = 0; i < count; i++) {
        fwrite(&patients[i], sizeof(Patient), 1, file);
    }
    fclose(file);
    return 1;
}

void list_patients() {
    Patient patients[MAX_RECORDS];
    int count = load_patients(patients);
    
    printf("[");
    for (int i = 0; i < count; i++) {
        char name[MAX_STRING * 2], disease[MAX_STRING * 2];
        json_escape(name, patients[i].name);
        json_escape(disease, patients[i].disease);
        
        printf("{\"id\":%d,\"name\":\"%s\",\"age\":%d,\"gender\":\"%s\",\"phone\":\"%s\",\"disease\":\"%s\",\"admission_date\":\"%s\"}",
               patients[i].id, name, patients[i].age, patients[i].gender, 
               patients[i].phone, disease, patients[i].admission_date);
        if (i < count - 1) printf(",");
    }
    printf("]");
}

void add_patient(char *json_data) {
    Patient patients[MAX_RECORDS];
    int count = load_patients(patients);
    
    if (count >= MAX_RECORDS) {
        printf("{\"success\":false,\"message\":\"Maximum patients limit reached\"}");
        return;
    }
    
    Patient new_patient;
    new_patient.id = count > 0 ? patients[count-1].id + 1 : 1;
    
    sscanf(json_data, "{\"action\":\"add\",\"name\":\"%[^\"]\",\"age\":%d,\"gender\":\"%[^\"]\",\"phone\":\"%[^\"]\",\"disease\":\"%[^\"]\",\"admission_date\":\"%[^\"]\"",
           new_patient.name, &new_patient.age, new_patient.gender, 
           new_patient.phone, new_patient.disease, new_patient.admission_date);
    
    patients[count] = new_patient;
    save_patients(patients, count + 1);
    
    printf("{\"success\":true,\"message\":\"Patient added successfully\",\"id\":%d}", new_patient.id);
}

void count_patients() {
    Patient patients[MAX_RECORDS];
    int count = load_patients(patients);
    printf("{\"count\":%d}", count);
}

// Similar update, delete, get functions as doctor.c but for patients

int main() {
    print_http_header();
    
    char *method = getenv("REQUEST_METHOD");
    char *query = getenv("QUERY_STRING");
    
    if (method && strcmp(method, "GET") == 0) {
        char params[MAX_RECORDS][2][MAX_STRING];
        int param_count;
        parse_query_string(query, params, &param_count);
        
        char action[MAX_STRING] = "";
        
        for (int i = 0; i < param_count; i++) {
            if (strcmp(params[i][0], "action") == 0) {
                strcpy(action, params[i][1]);
            }
        }
        
        if (strcmp(action, "list") == 0) {
            list_patients();
        } else if (strcmp(action, "count") == 0) {
            count_patients();
        }
    } else if (method && strcmp(method, "POST") == 0) {
        char *content_length_str = getenv("CONTENT_LENGTH");
        int content_length = content_length_str ? atoi(content_length_str) : 0;
        
        if (content_length > 0) {
            char *post_data = malloc(content_length + 1);
            fread(post_data, 1, content_length, stdin);
            post_data[content_length] = '\0';
            
            if (strstr(post_data, "\"action\":\"add\"")) {
                add_patient(post_data);
            }
            // Add other POST actions...
            
            free(post_data);
        }
    }
    
    return 0;
}
