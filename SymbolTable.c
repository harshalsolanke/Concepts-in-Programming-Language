#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_NAME_LENGTH 50

typedef struct SymbolTableEntry 
{
    char name[MAX_NAME_LENGTH];
    int value;
    struct SymbolTableEntry* next;

} SymbolTableEntry;

typedef struct SymbolTable 
{
    SymbolTableEntry* head;

} SymbolTable;

typedef struct Scope 
{
    SymbolTable symbol_table;
    struct Scope* parent_scope;

} Scope;

Scope* create_scope(Scope* parent_scope) 
{
    Scope* new_scope = (Scope*)malloc(sizeof(Scope));

    if (new_scope == NULL) 
    {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    new_scope->parent_scope = parent_scope;
    new_scope->symbol_table.head = NULL;
    return new_scope;
}

void insert_symbol(Scope* scope, const char* name, int value) 
{
    SymbolTableEntry* new_entry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));

    if (new_entry == NULL) 
    {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    strcpy(new_entry->name, name);
    new_entry->value = value;
    new_entry->next = scope->symbol_table.head;
    scope->symbol_table.head = new_entry;
}

int lookup_symbol(Scope* scope, const char* name) 
{
    int ret_val = INT_MIN;

    while (scope != NULL) 
    {
        SymbolTableEntry* current_entry = scope->symbol_table.head;
        while (current_entry != NULL) 
        {
            if (strcmp(current_entry->name, name) == 0) 
            {
                ret_val=current_entry->value;
            }
            current_entry = current_entry->next;
        }
        scope = scope->parent_scope;
    }

    if(ret_val == INT_MIN)
    {
        printf("%s:Key not found!!\n",name);
    }

    return ret_val; // Symbol not found
}

void end_scope(Scope** current_scope) 
{
    if (*current_scope == NULL) 
    {
        fprintf(stderr, "Current scope is NULL\n");
        exit(EXIT_FAILURE);
    }
    Scope* parent = (*current_scope)->parent_scope;
    free(*current_scope);
    *current_scope = parent;
}

void log_error(const char* message) 
{
    FILE* error_file = fopen("error.log", "a");
    if (error_file != NULL) 
    {
        fprintf(error_file, "Error: %s\n", message);
        fclose(error_file);
    } 
    else 
    {
        fprintf(stderr, "Failed to open error file for writing\n");
    }
}

int main() 
{
    FILE* input_file = fopen("input.txt", "r");
    if (input_file == NULL) 
    {
        log_error("Failed to open input file");
        return 1;
    }

    char line[100];
    Scope* global_scope = create_scope(NULL);
    Scope* current_scope = global_scope;

    while (fgets(line, sizeof(line), input_file)) 
    {
        char* token = strtok(line, " ");
        if (strcmp(token, "begin") == 0) 
        {
            current_scope = create_scope(current_scope);
        } 
        else if (strcmp(token, "assign") == 0) 
        {
            char* var_name = strtok(NULL, " ");
            char value_str[MAX_NAME_LENGTH];
            strcpy(value_str, strtok(NULL, " \n"));
            int value;
            if (sscanf(value_str, "%d", &value) == 1) 
            {
                insert_symbol(current_scope, var_name, value);
            } 
            else 
            {
                log_error("Failed to convert string to integer");
                // Handle error appropriately
            }
        } 
        else if (strcmp(token, "print") == 0) 
        {
            int flag;
            char* var_name = strtok(NULL, " \n");
            flag = lookup_symbol(current_scope,var_name);

            if(flag != INT_MIN)
            {
                printf("%s: %d\n", var_name, flag);
            }
        } 
        else if (strcmp(token, "end") == 0) 
        {
            end_scope(&current_scope);
        }
    }

    fclose(input_file);
    return 0;
}
