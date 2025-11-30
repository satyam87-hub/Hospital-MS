#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "database.h"

void print_http_header() {
    printf("Content-Type: application/json\r\n");
    printf("Access-Control-Allow-Origin: *\r\n");
    printf("Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n");
    printf("Access-Control-Allow-Headers: Content-Type\r\n");
    printf("\r\n");
}

char* url_decode(char *str) {
    char *decoded = malloc(strlen(str) + 1);
    char *pstr = str, *pdecoded = decoded;
    
    while (*pstr) {
        if (*pstr == '%') {
            if (pstr[1] && pstr[2]) {
                int value;
                sscanf(pstr + 1, "%2x", &value);
                *pdecoded++ = value;
                pstr += 3;
            }
        } else if (*pstr == '+') {
            *pdecoded++ = ' ';
            pstr++;
        } else {
            *pdecoded++ = *pstr++;
        }
    }
    *pdecoded = '\0';
    return decoded;
}

void parse_query_string(char *query, char params[][2][MAX_STRING], int *count) {
    *count = 0;
    if (query == NULL) return;
    
    char *token = strtok(query, "&");
    while (token != NULL && *count < MAX_RECORDS) {
        char *eq = strchr(token, '=');
        if (eq) {
            *eq = '\0';
            strncpy(params[*count][0], token, MAX_STRING - 1);
            strncpy(params[*count][1], eq + 1, MAX_STRING - 1);
            url_decode(params[*count][1]);
            (*count)++;
        }
        token = strtok(NULL, "&");
    }
}

void json_escape(char *dest, const char *src) {
    while (*src) {
        if (*src == '"' || *src == '\\') {
            *dest++ = '\\';
        }
        *dest++ = *src++;
    }
    *dest = '\0';
}
