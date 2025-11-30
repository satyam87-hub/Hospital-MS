#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"

#define EMPLOYEE_FILE "employees.dat"

int load_employees(Employee employees[]) {
    FILE *file = fopen(EMPLOYEE_FILE, "rb");
    if (!file) return 0;
    
    int count = 0;
    while (fread(&employees[count], sizeof(Employee), 1, file) == 1) {
        count++;
    }
    fclose(file);
    return count;
}

int save_employees(Employee employees[], int count) {
    FILE *file = fopen(EMPLOYEE_FILE, "wb");
    if (!file) return 0;
    
    for (int i = 0; i < count; i++) {
        fwrite(&employees[i], sizeof(Employee), 1, file);
    }
    fclose(file);
    return 1;
}

void list_employees() {
    Employee employees[MAX_RECORDS];
    int count = load_employees(employees);
    
    printf("[");
    for (int i = 0; i < count; i++) {
        char name[MAX_STRING * 2], position[MAX_STRING * 2];
        char department[MAX_STRING * 2], email[MAX_STRING * 2];
        
        json_escape(name, employees[i].name);
        json_escape(position, employees[i].position);
        json_escape(department, employees[i].department);
        json_escape(email, employees[i].email);
        
        printf("{\"id\":%d,\"name\":\"%s\",\"position\":\"%s\",\"department\":\"%s\",\"phone\":\"%s\",\"email\":\"%s\"}",
               employees[i].id, name, position, department, 
               employees[i].phone, email);
        
        if (i < count - 1) printf(",");
    }
    printf("]");
}

void add_employee(char *json_data) {
    Employee employees[MAX_RECORDS];
    int count = load_employees(employees);
    
    if (count >= MAX_RECORDS) {
        printf("{\"success\":false,\"message\":\"Maximum employees limit reached\"}");
        return;
    }
    
    Employee new_employee;
    new_employee.id = count > 0 ? employees[count-1].id + 1 : 1;
    
    // Parse JSON data
    sscanf(json_data, 
           "{\"action\":\"add\",\"name\":\"%[^\"]\",\"position\":\"%[^\"]\",\"department\":\"%[^\"]\",\"phone\":\"%[^\"]\",\"email\":\"%[^\"]\"",
           new_employee.name, new_employee.position, new_employee.department,
           new_employee.phone, new_employee.email);
    
    employees[count] = new_employee;
    save_employees(employees, count + 1);
    
    printf("{\"success\":true,\"message\":\"Employee added successfully\",\"id\":%d}", 
           new_employee.id);
}

void update_employee(char *json_data) {
    Employee employees[MAX_RECORDS];
    int count = load_employees(employees);
    int id;
    
    sscanf(json_data, "{\"action\":\"update\",\"id\":%d", &id);
    
    for (int i = 0; i < count; i++) {
        if (employees[i].id == id) {
            sscanf(json_data, 
                   "{\"action\":\"update\",\"id\":%d,\"name\":\"%[^\"]\",\"position\":\"%[^\"]\",\"department\":\"%[^\"]\",\"phone\":\"%[^\"]\",\"email\":\"%[^\"]\"",
                   &id, employees[i].name, employees[i].position, 
                   employees[i].department, employees[i].phone, employees[i].email);
            
            save_employees(employees, count);
            printf("{\"success\":true,\"message\":\"Employee updated successfully\"}");
            return;
        }
    }
    printf("{\"success\":false,\"message\":\"Employee not found\"}");
}

void delete_employee(int id) {
    Employee employees[MAX_RECORDS];
    int count = load_employees(employees);
    int found = 0;
    
    for (int i = 0; i < count; i++) {
        if (employees[i].id == id) {
            // Shift all elements after this position
            for (int j = i; j < count - 1; j++) {
                employees[j] = employees[j + 1];
            }
            count--;
            found = 1;
            break;
        }
    }
    
    if (found) {
        save_employees(employees, count);
        printf("{\"success\":true,\"message\":\"Employee deleted successfully\"}");
    } else {
        printf("{\"success\":false,\"message\":\"Employee not found\"}");
    }
}

void get_employee(int id) {
    Employee employees[MAX_RECORDS];
    int count = load_employees(employees);
    
    for (int i = 0; i < count; i++) {
        if (employees[i].id == id) {
            char name[MAX_STRING * 2], position[MAX_STRING * 2];
            char department[MAX_STRING * 2], email[MAX_STRING * 2];
            
            json_escape(name, employees[i].name);
            json_escape(position, employees[i].position);
            json_escape(department, employees[i].department);
            json_escape(email, employees[i].email);
            
            printf("{\"id\":%d,\"name\":\"%s\",\"position\":\"%s\",\"department\":\"%s\",\"phone\":\"%s\",\"email\":\"%s\"}",
                   employees[i].id, name, position, department, 
                   employees[i].phone, email);
            return;
        }
    }
    printf("{\"success\":false,\"message\":\"Employee not found\"}");
}

void count_employees() {
    Employee employees[MAX_RECORDS];
    int count = load_employees(employees);
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
        
        // Parse parameters
        for (int i = 0; i < param_count; i++) {
            if (strcmp(params[i][0], "action") == 0) {
                strcpy(action, params[i][1]);
            } else if (strcmp(params[i][0], "id") == 0) {
                id = atoi(params[i][1]);
            }
        }
        
        // Handle GET actions
        if (strcmp(action, "list") == 0) {
            list_employees();
        } else if (strcmp(action, "get") == 0) {
            get_employee(id);
        } else if (strcmp(action, "count") == 0) {
            count_employees();
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
            
            // Handle POST actions
            if (strstr(post_data, "\"action\":\"add\"")) {
                add_employee(post_data);
            } else if (strstr(post_data, "\"action\":\"update\"")) {
                update_employee(post_data);
            } else if (strstr(post_data, "\"action\":\"delete\"")) {
                int id;
                sscanf(post_data, "{\"action\":\"delete\",\"id\":%d", &id);
                delete_employee(id);
            } else {
                printf("{\"success\":false,\"message\":\"Invalid action\"}");
            }
            
            free(post_data);
        }
        
    } else {
        printf("{\"success\":false,\"message\":\"Invalid request method\"}");
    }
    
    return 0;
}
