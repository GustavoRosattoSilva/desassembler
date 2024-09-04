#include <stdio.h>
#include <stdlib.h>

void traduzirOpcode(int opcode, FILE *saida) {
    switch(opcode) {
        case 0: // SPACE
            fprintf(saida, "; Espaço reservado\n");
            break;
        case 1: // ADD
            fprintf(saida, "ADD EAX, [mem]\n");
            break;
        case 2: // SUB
            fprintf(saida, "SUB EAX, [mem]\n");
            break;
        case 3: // MUL
            fprintf(saida, "IMUL EAX, [mem]\n");
            // Adiciona a lógica para verificar overflow
            break;
        case 4: // DIV
            fprintf(saida, "IDIV [mem]\n");
            break;
        case 5: // JMP
            fprintf(saida, "JMP label\n"); //  arrumar para  substituir "label"  pelo o registradore 
            break;
        case 6: // JMPN (Jump if Negative)
            fprintf(saida, "JL label\n");
            break;
        case 7: // JMPP (Jump if Positive)
            fprintf(saida, "JG label\n");
            break;
        case 8: // JMPZ (Jump if Zero)
            fprintf(saida, "JE label\n");
            break;
        case 9: // COPY
            fprintf(saida, "MOV [dest], [src]\n"); // arrumar "dest" e "src" para receber os registradores
            break;
        case 10: // LOAD
            fprintf(saida, "MOV EAX, [mem]\n");
            break;
        case 11: // STORE
            fprintf(saida, "MOV [mem], EAX\n");
            break;
        case 12: // INPUT
            fprintf(saida, "CALL read_input\n");
            break;
        case 13: // OUTPUT
            fprintf(saida, "CALL write_output\n");
            break;
        case 14: // STOP
            fprintf(saida, "HLT\n");
            break;
        default:
            fprintf(saida, "; Opcode %d não reconhecido\n", opcode);
            break;
    }
}

int main() {
    FILE *entrada, *saida;
    int opcode;

    entrada = fopen("myfile.obj", "r");
    if (entrada == NULL) {
        perror("Erro ao abrir o arquivo de entrada");
        return 1;
    }

    saida = fopen("myfile.s", "w");
    if (saida == NULL) {
        perror("Erro ao criar o arquivo de saída");
        fclose(entrada);
        return 1;
    }

    while (fscanf(entrada, "%d", &opcode) != EOF) {
        traduzirOpcode(opcode, saida);
    }

    fclose(entrada);
    fclose(saida);

    printf("Tradução concluída. Saída salva em myfile.s\n");
    return 0;
}

