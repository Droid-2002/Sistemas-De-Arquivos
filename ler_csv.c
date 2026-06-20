#include "string.h"
#include "stdio.h"
#include "stdlib.h"

void ler_csv(char* file_path) {
    
    FILE* file = fopen(file_path, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", file_path);
        return;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        // Process the line as needed
        printf("%s", line);
    }

    fclose(file);
}


int main() {
    char file_path[] = "C:\\Users\\duart\\directory\\Sistemas-De-Arquivos\\estacoes.csv"; // Replace with your CSV file path
    ler_csv(file_path);
    return 0;
}
