#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "const.h"

typedef struct {
    int endereco;
    char label[20];
} Label;

typedef struct {
    char label[20];
    int opcode;
} Const;

Label labels[MAX_MEMORIA];
Const constantes[MAX_MEMORIA];
char nome_arquivo_saida[MAX_CAMINHO];
int const_count = 0;
int label_count = 0;
int stop_found = 0;
int functions = 0;
int bss = 0;
int data = 0;  
 

void adicionarLabel(int endereco) {
    if (label_count < MAX_MEMORIA) {
        snprintf(labels[label_count].label, sizeof(labels[label_count].label), "label_%d", endereco);
        labels[label_count].endereco = endereco;
        label_count++;
    }
}

int buscarEnderecoLabel(int endereco) {
    for (int i = 0; i < label_count; i++) {
        if (labels[i].endereco == endereco) {
            return i;
        }
    }
    return -1;
}

void mapearLabels(FILE *entrada) {
    int opcode, argumento1, argumento2;
    int contador_programa = 0;
    int encontrado_stop = 0;

    while (fscanf(entrada, "%d", &opcode) != EOF) {
        if (encontrado_stop) {
            if (opcode == 0) {
                // Se após STOP, o opcode for 0, é um SPACE
                adicionarLabel(contador_programa);
                contador_programa++;
            } else {
                // Se após STOP, o opcode não for 0, é uma constante
                adicionarLabel(contador_programa);
                contador_programa++;
            }
        } else {
            switch (opcode) {
                case SPACE:
                case STOP:
                    contador_programa += 1;
                    encontrado_stop = 1;
                    break;
                case ADD:
                case SUB:
                case MUL:
                case DIV:
                case JMP:
                case JMPN:
                case JMPP:
                case JMPZ:
                case LOAD:
                case STORE:
                case INPUT:
                case OUTPUT:
                    fscanf(entrada, "%d", &argumento1);
                    if (opcode >= JMP && opcode <= JMPZ) {
                        adicionarLabel(argumento1);
                    }
                    contador_programa += 2;
                    break;
                case COPY:
                    fscanf(entrada, "%d %d", &argumento1, &argumento2);
                    contador_programa += 3;
                    break;
                default:
                    contador_programa += 1;
                    break;
            }
        }
    }

    rewind(entrada);
}

int traduzirOpcode(int opcode, FILE *saida, int *contador_programa, FILE *entrada) {
    int argumento1, argumento2;

    // Checa se há uma label para o endereço atual do contador de programa
    int index = buscarEnderecoLabel(*contador_programa);
    if (index != -1) {
        fprintf(saida, "%s:\n", labels[index].label);
    }

    switch (opcode) {
        case ADD:
            fscanf(entrada, "%d", &argumento1);
            fprintf(saida, "    add eax, DWORD [label_%d]\n", argumento1);
            (*contador_programa) += 2;
            break;
        case SUB:
            fscanf(entrada, "%d", &argumento1);
            fprintf(saida, "    sub eax, DWORD [label_%d]\n", argumento1);
            (*contador_programa) += 2;
            break;
        case MUL:
            fscanf(entrada, "%d", &argumento1);
            fprintf(saida, "    mov ebx, DWORD [label_%d]\n", argumento1);
            fprintf(saida, "    imul eax, ebx\n");
            fprintf(saida, "    jo overflow\n");
            (*contador_programa) += 2;
            break;
        case DIV:
            fscanf(entrada, "%d", &argumento1);
            fprintf(saida, "    mov ebx, DWORD [label_%d]\n", argumento1);
            fprintf(saida, "    cdq\n");
            fprintf(saida, "    idiv ebx\n");
            fprintf(saida, "    jo overflow\n");
            (*contador_programa) += 2;
            break;
        case JMP:
            fscanf(entrada, "%d", &argumento1);
            fprintf(saida, "    jmp SHORT label_%d\n", argumento1);
            (*contador_programa) += 2;
            break;
        case JMPN:
            fscanf(entrada, "%d", &argumento1);
            fprintf(saida, "    jl SHORT label_%d\n", argumento1);
            (*contador_programa) += 2;
            break;
        case JMPP:
            fscanf(entrada, "%d", &argumento1);
            fprintf(saida, "    jg SHORT label_%d\n", argumento1);
            (*contador_programa) += 2;
            break;
        case JMPZ:
            fscanf(entrada, "%d", &argumento1);
            fprintf(saida, "    je SHORT label_%d\n", argumento1);
            (*contador_programa) += 2;
            break;
        case COPY:
            fscanf(entrada, "%d %d", &argumento1, &argumento2);
            fprintf(saida, "    mov eax, DWORD [label_%d]\n", argumento1);
            fprintf(saida, "    mov DWORD [label_%d], eax\n", argumento2);
            (*contador_programa) += 3;
            break;
        case LOAD:
            fscanf(entrada, "%d", &argumento1);
            fprintf(saida, "    mov eax, DWORD [label_%d]\n", argumento1);
            (*contador_programa) += 2;
            break;
        case STORE:
            fscanf(entrada, "%d", &argumento1);
            fprintf(saida, "    mov DWORD [label_%d], eax\n", argumento1);
            (*contador_programa) += 2;
            break;
        case INPUT:
            fscanf(entrada, "%d", &argumento1);
            fprintf(saida, "    call INPUT\n");
            fprintf(saida, "    call STR_TO_INT\n");
            fprintf(saida, "    mov eax, [number]\n");
            fprintf(saida, "    mov DWORD [label_%d], eax\n", argumento1);
            (*contador_programa) += 2;
            break;
        case OUTPUT:
            fscanf(entrada, "%d", &argumento1);
            fprintf(saida, "    mov eax, [label_%d]\n", argumento1);
            // fprintf(saida, "    mov [number], eax\n");
            fprintf(saida, "    call INT_TO_STR\n");
            fprintf(saida, "    call OUTPUT\n");
            fprintf(saida, "    call PRINT_NEWLINE\n");
            (*contador_programa) += 2;
            break;
        case STOP:
            fprintf(saida, "    mov eax, 1\n");
            fprintf(saida, "    mov ebx, 0\n");
            fprintf(saida, "    int 0x80\n");
            (*contador_programa) += 1;
            return 1;
        default:
            fprintf(saida, "    ; Opcode %d não reconhecido\n", opcode);
            (*contador_programa) += 1;
            break;
    }

    return 0;
}

void adicionarSectionData(FILE *saida) {
    fprintf(saida, "section .data\n");
    fprintf(saida, "    newline db 10\n");
}

void adicionarSectionBss(FILE *saida) {
    fprintf(saida, "section .bss\n");
    fprintf(saida, "    buffer resb 10\n");
    fprintf(saida, "    number resd 1\n");
}

void alocar_memoria(int opcode, FILE *saida, int *contador_programa, FILE *entrada) {
    int index;

    if (!bss){
        adicionarSectionBss(saida);
        bss = 1;
    }
    

    // Aloca memória com base no opcode
    switch (opcode) {
        case SPACE:
            index = buscarEnderecoLabel(*contador_programa);
            if (index != -1) {
                fprintf(saida, "    %s resd 1\n", labels[index].label);
            }
            (*contador_programa) += 1;
            break;

        default:
            // Para constantes, aloca memória na seção .data para memória inicializada
            index = buscarEnderecoLabel(*contador_programa);
            if (index != -1) {
                strcpy(constantes[const_count].label, labels[index].label);
                constantes[const_count].opcode = opcode;
            }
             
            (*contador_programa) += 1;
            const_count++;
            break;
    }
}

void adicionarSectionText(FILE *saida) {

    fprintf(saida, "section .text\n");
    fprintf(saida, "    global _start\n");
    fprintf(saida, "_start:\n");
}

void adicionarFuncoes(FILE *saida) {
    fprintf(saida, "overflow:\n");
    fprintf(saida, "    ; Código para tratamento de overflow\n");
    fprintf(saida, "    mov eax, 1\n");
    fprintf(saida, "    mov ebx, 1\n");
    fprintf(saida, "    int 0x80\n");
    
    fprintf(saida, "INPUT:\n");
    fprintf(saida, "    push ebp\n");
    fprintf(saida, "    mov ebp, esp\n");
    fprintf(saida, "    push eax\n");
    fprintf(saida, "    push ebx\n");
    fprintf(saida, "    push ecx\n");
    fprintf(saida, "    push edx\n");
    fprintf(saida, "    mov eax, 3\n");
    fprintf(saida, "    mov ebx, 0\n");
    fprintf(saida, "    mov ecx, buffer\n");
    fprintf(saida, "    mov edx, 10\n");
    fprintf(saida, "    int 0x80\n");
    // remover
    // fprintf(saida, "    mov byte [ecx + eax - 1], 0\n");
    fprintf(saida, "    pop edx\n");
    fprintf(saida, "    pop ecx\n");
    fprintf(saida, "    pop ebx\n");
    fprintf(saida, "    pop eax\n");
    fprintf(saida, "    mov esp, ebp\n");
    fprintf(saida, "    pop ebp\n");
    fprintf(saida, "    ret\n");

    fprintf(saida, "OUTPUT:\n");
    fprintf(saida, "    push ebp\n");
    fprintf(saida, "    mov ebp, esp\n");
    fprintf(saida, "    push eax\n");
    fprintf(saida, "    push ebx\n");
    fprintf(saida, "    push ecx\n");
    fprintf(saida, "    push edx\n");
    fprintf(saida, "    mov eax, 4\n");
    fprintf(saida, "    mov ebx, 1\n");
    fprintf(saida, "    mov ecx, buffer\n");
    fprintf(saida, "    mov edx, 10\n");
    fprintf(saida, "    int 0x80\n");
    fprintf(saida, "    pop edx\n");
    fprintf(saida, "    pop ecx\n");
    fprintf(saida, "    pop ebx\n");
    fprintf(saida, "    pop eax\n");
    fprintf(saida, "    mov esp, ebp\n");
    fprintf(saida, "    pop ebp\n");
    fprintf(saida, "    ret\n");

    fprintf(saida, "STR_TO_INT:\n");
    fprintf(saida, "    mov eax, 0\n");
    fprintf(saida, "    mov ecx, 0\n");
    
    fprintf(saida, "convert_loop:\n");
    fprintf(saida, "    mov bl, [buffer + ecx]\n");
    fprintf(saida, "    cmp bl, 10\n");
    fprintf(saida, "    je done_conversion\n");
    
    fprintf(saida, "    sub bl, '0'\n");
    fprintf(saida, "    imul eax, eax, 10\n");
    fprintf(saida, "    add eax, ebx\n");
    
    fprintf(saida, "    inc ecx\n");
    fprintf(saida, "    jmp convert_loop\n");
    
    fprintf(saida, "done_conversion:\n");
    fprintf(saida, "    mov [number], eax\n");
    fprintf(saida, "    ret\n");

    fprintf(saida, "INT_TO_STR:\n");
    fprintf(saida, "    mov ebx, 10\n");
    fprintf(saida, "    mov ecx, buffer + 9\n");
    fprintf(saida, "    mov byte [ecx], 0\n");

    fprintf(saida, "convert_to_str_loop:\n");
    fprintf(saida, "    xor edx, edx\n");
    fprintf(saida, "    div ebx\n");
    fprintf(saida, "    add dl, '0'\n");
    fprintf(saida, "    dec ecx\n");
    fprintf(saida, "    mov [ecx], dl\n");
    fprintf(saida, "    test eax, eax\n");
    fprintf(saida, "    jnz convert_to_str_loop\n");
    // fprintf(saida, "    mov byte [ecx - 1], 0\n");
    fprintf(saida, "    ret\n");

    fprintf(saida, "PRINT_NEWLINE:\n");
    fprintf(saida, "    mov eax, 4\n");
    fprintf(saida, "    mov ebx, 1\n");
    fprintf(saida, "    mov ecx, newline\n");
    fprintf(saida, "    mov edx, 1\n");
    fprintf(saida, "    int 0x80\n");
    fprintf(saida, "    ret\n");
}

void substituirExtensao(char *nome_arquivo, const char *nova_extensao, char *nome_arquivo_com_nova_extensao) {
    char *ponto = strrchr(nome_arquivo, '.'); // Encontra o último ponto na string
    if (ponto != NULL) {
        *ponto = '\0'; // Remove a extensão antiga
    }
    snprintf(nome_arquivo_com_nova_extensao, 256, "%s%s", nome_arquivo, nova_extensao); // Adiciona a nova extensão
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Uso: %s <arquivo de entrada>\n", argv[0]);
        return 1;
    }

    FILE *entrada = fopen(argv[1], "r");
    if (entrada == NULL) {
        perror("Erro ao abrir o arquivo de entrada");
        return 1;
    }

    // Mapeia os labels antes de traduzir
    mapearLabels(entrada);
    substituirExtensao(argv[1], ".s", nome_arquivo_saida);

    FILE *saida = fopen(nome_arquivo_saida, "w");
    if (saida == NULL) {
        perror("Erro ao abrir o arquivo de saída");
        fclose(entrada);
        return 1;
    }

    adicionarSectionText(saida);

    int opcode;
    int contador_programa = 0;

    while (fscanf(entrada, "%d", &opcode) != EOF) {
        if (!stop_found) {
            stop_found = traduzirOpcode(opcode, saida, &contador_programa, entrada);
        } 
        else {
            if (!functions){
                adicionarFuncoes(saida);
                functions = 1;                
            }
            
            alocar_memoria(opcode, saida, &contador_programa, entrada);
        }
    }

    adicionarSectionData(saida);

    for (int i = 0; i < const_count; i++){
        fprintf(saida, "    %s dd %d\n", constantes[i].label, constantes[i].opcode);
    }
    
    fclose(entrada);
    fclose(saida);

    return 0;
}
