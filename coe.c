#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define TABLE_SIZE 100
#define MAX_LINE_LENGTH 256
typedef struct Symbol {
int size;
char *name,*type;
int dim;
int lor;
int *lineofusage;
int address;
int usage_count;
struct Symbol *next;
} Symbol;
typedef SymbolTable {
Symbol *table[TABLE_SIZE];
int address_counter;
} SymbolTable;
unsigned int hash(char *name){
unsigned int hash=0;
while(*name){
hash=(hash<< 5) + *name++;
}
return hash % TABLE_SIZE;
}
Symbol *create_symbol(char *name, char *type, int size, int lor, int address){
Symbol *new_symbol= (Symbol *)malloc(sizeof(Symbol));
new_symbol-> name =strdup(name);
new_symbol-> type =strdup(type);
new_symbol-> size =size;
new_symbol-> dim =0;
new_symbol-> lor=lor;
new_symbol-> lineofusage= (int *)malloc(sizeof(int) *10);
new_symbol-> address=address;
new_symbol-> usage_count=0;
new_symbol-> next=NULL;
return new_symbol;
};
SymbolTable *create_symbol_table(){
SymbolTable *symbol_table= (SymbolTable *)malloc(sizeof(SymbolTable));
for(int i=0;i<TABLE_SIZE;i++){
symbol_table -> table[i]=NULL;
}
symbol_table-> address_counter=0;
return symbol_table;
}
void insert_symbol(SymbolTable *symbol_table, char *name, char *type, int
size, int lor){
unsigned int index = hash(name);
int address = symbol_table->address_counter++;
Symbol *new_symbol = create_symbol(name, type, size, lor, address);
if (symbol_table->table[index] == NULL) {
symbol_table->table[index] = new_symbol;
}
else {
Symbol *current = symbol_table->table[index];
while (current->next != NULL) {
current = current->next;
}
current->next = new_symbol;
}
}
Symbol *search_symbol(SymbolTable *symbol_table, char *name) {
unsigned int index = hash(name);
Symbol *current = symbol_table->table[index];
while (current != NULL) {
if (strcmp(current->name, name) == 0) {
return current;
}
current = current->next;
}
return NULL;
}
void add_usage_line(Symbol *symbol, int line) {
if (symbol->usage_count % 10 == 0) {
symbol->line_usage = (int *)realloc(symbol->line_usage, sizeof(int) * (symbol-
>usage_count + 10));
}
symbol->line_usage[symbol->usage_count++] = line;
}
void parse_code(SymbolTable *symbol_table, const char *code) {
char line[MAX_LINE_LENGTH];
FILE *fp = fopen(code, "r");
if (!fp) {
printf("Failed to open file.\n");
return;
}
int line_number = 0;
while (fgets(line, sizeof(line), fp)) {
line_number++;
char *token = strtok(line, " ;\t\n");
while (token) {
if (strcmp(token, "int") == 0 || strcmp(token,"float") == 0 || strcmp(token,
"char") == 0) {
char *type = token;
token = strtok(NULL, " ;\t\n");
if (token) {
char *name = token;
int size = (strcmp(type, "int") == 0) ? sizeof(int) :(strcmp(type, "float") == 0) ?
sizeof(float) :(strcmp(type, "char") == 0) ? sizeof(char) : 0;
insert_symbol(symbol_table, name, type, size, line_number);
}
}
else {
Symbol *symbol = search_symbol(symbol_table, token);
if (symbol) {
add_usage_line(symbol, line_number);
}
}
token = strtok(NULL, " ;\t\n");
}
}
fclose(fp);
}
void display_symbol_table(SymbolTable *symbol_table) {
for (int i = 0; i < TABLE_SIZE; i++) {
Symbol *current = symbol_table->table[i];
if (current != NULL) {
while (current != NULL) {
printf("Name: %s, Type: %s, Size: %d, Line of Declaration: %d, Address: %d,
Line of Usage: ",current->name, current->type, current->size, current->lor,
current->address);
for (int j = 0; j < current->usage_count; j++) {
printf("%d ", current->line_usage[j]);
}
printf("\n");
current = current->next;
}
}
}
}
int main() {
SymbolTable *symbol_table = create_symbol_table();
const char *code = "input_code.c";
parse_code(symbol_table, code);
display_symbol_table(symbol_table);
return 0;
}