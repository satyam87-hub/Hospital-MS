#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"

#define DOCTOR_FILE "doctors.dat"

int load_doctors(Doctor doctors[]) {
    FILE *file = fopen(DOCTOR_FILE, "rb");
    if (!file) return 0;
    
    int count = 0;
    while (fread(&doctors[count], sizeof(Doctor), 1, file) == 1) {
        count++;
    }
    fclose(file);
    return count;
}

int save_doctors(Doctor doctors[], int count) {
    FILE *file = fopen(DOCTOR_FILE, "wb");
    if (!file) return 0;
    
    for (int i = 0; i < count; i++) {
        fwrite(&doctors[i], sizeof(Doctor), 1, file);
    }
    fclose(file);
    return 1;
}

void list_doctors() {
    Doctor doctors[MAX_RECORDS];
    int count = load_doctors(doctors);
    
    printf("[");
    for (int i = 0; i < count; i++) {
        char name[MAX_STRING * 2], spec[MAX_STRING * 2], email[MAX_STRING * 2];
        json_escape(name, doctors[i].name);
        json_escape(spec, doctors[i].specialization);
        json_escape(email, doctors[i].email);
        
        printf("{\"id\":%d,\"name\":\"%s\",\"specialization\":\"%s\",\"phone\":\"%s\",\"email\":\"%s\"}",
               doctors[i].id, name, spec, doctors[i].phone, email);
        if (i < count - 1) printf(",");
    }
    printf("]");
}

void add_doctor(char *json_data) {
    Doctor doctors[MAX_RECORDS];
    int count = load_doctors(doctors);
    
    if (count >= MAX_RECORDS) {
        printf("{\"success\":false,\"message\":\"Maximum doctors limit reached\"}");
        return;
    }
    
    Doctor new_doctor;
    new_doctor.id = count > 0 ? doctors[count-1].id + 1 : 1;
    
    // Parse JSON (simplified - in production use a JSON library)
    sscanf(json_data, "{\"action\":\"add\",\"name\":\"%[^\"]\",\"specialization\":\"%[^\"]\",\"phone\":\"%[^\"]\",\"email\":\"%[^\"]\"",
           new_doctor.name, new_doctor.specialization, new_doctor.phone, new_doctor.email);
    
    doctors[count] = new_doctor;
    save_doctors(doctors, count + 1);
    
    printf("{\"success\":true,\"message\":\"Doctor added successfully\",\"id\":%d}", new_doctor.id);
}

void update_doctor(char *json_data) {
    Doctor doctors[MAX_RECORDS];
    int count = load_doctors(doctors);
    int id;
    
    sscanf(json_data, "{\"action\":\"update\",\"id\":%d", &id);
    
    for (int i = 0; i < count; i++) {
        if (doctors[i].id == id) {
            sscanf(json_data, "{\"action\":\"update\",\"id\":%d,\"name\":\"%[^\"]\",\"specialization\":\"%[^\"]\",\"phone\":\"%[^\"]\",\"email\":\"%[^\"]\"",
                   &id, doctors[i].name, doctors[i].specialization, doctors[i].phone, doctors[i].email);
            save_doctors(doctors, count);
            printf("{\"success\":true,\"message\":\"Doctor updated successfully\"}");
            return;
        }
    }
    printf("{\"success\":false,\"message\":\"Doctor not found\"}");
}

void delete_doctor(int id) {
    Doctor doctors[MAX_RECORDS];
    int count = load_doctors(doctors);
    int found = 0;
    
    for (int i = 0; i < count; i++) {
        if (doctors[i].id == id) {
            for (int j = i; j < count - 1; j++) {
                doctors[j] = doctors[j + 1];
            }
            count--;
            found = 1;
            break;
        }
    }
    
    if (found) {
        save_doctors(doctors, count);
        printf("{\"success\":true,\"message\":\"Doctor deleted successfully\"}");
    } else {
        printf("{\"success\":false,\"message\":\"Doctor not found\"}");
    }
}

void get_doctor(int id) {
    Doctor doctors[MAX_RECORDS];
    int count = load_doctors(doctors);
    
    for (int i = 0; i < count; i++) {
        if (doctors[i].id == id) {
            char name[MAX_STRING * 2], spec[MAX_STRING * 2], email[MAX_STRING * 2];
            json_escape(name, doctors[i].name);
            json_escape(spec, doctors[i].specialization);
            json_escape(email, doctors[i].email);
            
            printf("{\"id\":%d,\"name\":\"%s\",\"specialization\":\"%s\",\"phone\":\"%s\",\"email\":\"%s\"}",
                   doctors[i].id, name, spec, doctors[i].phone, email);
            return;
        }
    }
    printf("{\"success\":false,\"message\":\"Doctor not found\"}");
}

void count_doctors() {
    Doctor doctors[MAX_RECORDS];
    int count = load_doctors(doctors);
    printf("{\"count\":%d}", count);
}

int main() {
    print_http_header();
    
    char *method = getenv("REQUEST_METHOD");
    char *query = getenv("QUERY_STRING");
    
    if (method && strcmp(method, "GET") == 0) {
        char params[MAX_RECORDS][2][MAX_STRING];
        int param_count;
        parse_query_string(query, params, &param_count);
        
        char action[MAX_STRING] = "";
        int id = 0;
        
        for (int i = 0; i < param_count; i++) {
            if (strcmp(params[i][0], "action") == 0) {
                strcpy(action, params[i][1]);
            } else if (strcmp(params[i][0], "id") == 0) {
                id = atoi(params[i][1]);
            }
        }
        
        if (strcmp(action, "list") == 0) {
            list_doctors();
        } else if (strcmp(action, "get") == 0) {
            get_doctor(id);
        } else if (strcmp(action, "count") == 0) {
            count_doctors();
        } else {
            printf("{\"success\":false,\"message\":\"Invalid action\"}");
        }
    } else if (method && strcmp(method, "POST") == 0) {
        char *content_length_str = getenv("CONTENT_LENGTH");
        int content_length = content_length_str ? atoi(content_length_str) : 0;
        
        if (content_length > 0) {
            char *post_data = malloc(content_length + 1);
            fread(post_data, 1, content_length, stdin);
            post_data[content_length] = '\0';
            
            if (strstr(post_data, "\"action\":\"add\"")) {
                add_doctor(post_data);
            } else if (strstr(post_data, "\"action\":\"update\"")) {
                update_doctor(post_data);
            } else if (strstr(post_data, "\"action\":\"delete\"")) {
                int id;
                sscanf(post_data, "{\"action\":\"delete\",\"id\":%d", &id);
                delete_doctor(id);
            }
            
            free(post_data);
        }
    }
    
    return 0;
}
