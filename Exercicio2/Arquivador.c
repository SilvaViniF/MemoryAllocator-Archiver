#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXNAME 256

#pragma pack(push, 1)
struct fileheader {
    char name[MAXNAME];      // Nome do arquivo
    unsigned int filesize;   // Tamanho do arquivo em bytes
};
#pragma pack(pop)

void archive_files(const char *output_file, int file_count, char *file_paths[]) {
    FILE *output_fp = fopen(output_file, "wb");
    if (!output_fp) {
        perror("Failed to open output file");
        return;
    }

    for (int i = 0; i < file_count; ++i) {
        FILE *input_fp = fopen(file_paths[i], "rb");
        if (!input_fp) {
            perror("Failed to open input file");
            continue;
        }

        struct fileheader header;
        strncpy(header.name, file_paths[i], MAXNAME);
        fseek(input_fp, 0, SEEK_END);
        header.filesize = ftell(input_fp);
        fseek(input_fp, 0, SEEK_SET);

        fwrite(&header, sizeof(struct fileheader), 1, output_fp);

        char *buffer = (char *)malloc(header.filesize);
        fread(buffer, 1, header.filesize, input_fp);
        fwrite(buffer, 1, header.filesize, output_fp);

        free(buffer);
        fclose(input_fp);
    }

    fclose(output_fp);
}

void extract_file(const char *archive_path, const char *file_name) {
    FILE *archive_fp = fopen(archive_path, "rb");
    if (!archive_fp) {
        perror("Failed to open archive file");
        return;
    }

    struct fileheader header;
    while (fread(&header, sizeof(struct fileheader), 1, archive_fp)) {
        if (strncmp(header.name, file_name, MAXNAME) == 0) {
            char *buffer = (char *)malloc(header.filesize);
            fread(buffer, 1, header.filesize, archive_fp);

            FILE *output_fp = fopen(file_name, "wb");
            if (!output_fp) {
                perror("Failed to create output file");
                free(buffer);
                fclose(archive_fp);
                return;
            }

            fwrite(buffer, 1, header.filesize, output_fp);

            free(buffer);
            fclose(output_fp);
            fclose(archive_fp);
            return;
        } else {
            fseek(archive_fp, header.filesize, SEEK_CUR);
        }
    }

    printf("File not found in the archive.\n");
    fclose(archive_fp);
}

void list_files(const char *archive_path) {
    FILE *archive_fp = fopen(archive_path, "rb");
    if (!archive_fp) {
        perror("Failed to open archive file");
        return;
    }

    struct fileheader header;
    while (fread(&header, sizeof(struct fileheader), 1, archive_fp)) {
        printf("File: %s, Size: %u bytes\n", header.name, header.filesize);
        fseek(archive_fp, header.filesize, SEEK_CUR);
    }

    fclose(archive_fp);
}

int main(int argc, char **argv) {
    // Verifique se temos os argumentos mínimos necessários
    if (argc < 2) {
        printf("Uso:\n");
        printf("\t%s -c <arquivo_de_saida> <arquivo1> <arquivo2> ... <arquivoN> - Cria arquivo.\n", argv[0]);
        printf("\t%s -l <arquivo_de_entrada> - Lista arquivos.\n", argv[0]);
        printf("\t%s -e <arquivo_de_entrada> <arquivo> - Extrai conteúdo.\n", argv[0]);
        return 1;
    }

    // O primeiro argumento é a opção
    char *opt = argv[1];

    if (strcmp(opt, "-l") == 0) {
        if (argc < 3) {
            printf("Por favor, forneça o arquivo de entrada para listagem.\n");
            return 1;
        }
        list_files(argv[2]);
        return 0;
    }

    if (strcmp(opt, "-c") == 0) {
        if (argc < 4) {
            printf("Por favor, forneça o arquivo de saída e pelo menos um arquivo de entrada.\n");
            return 1;
        }
        char *output_file = argv[2];
        char **input_files = &argv[3];
        int num_input_files = argc - 3;

        printf("Arquivo de saída: %s\n", output_file);
        archive_files(output_file, num_input_files, input_files);
        return 0;
    }

    if (strcmp(opt, "-e") == 0) {
        if (argc < 4) {
            printf("Por favor, forneça o arquivo de entrada e o nome do arquivo a ser extraído.\n");
            return 1;
        }
        extract_file(argv[2], argv[3]);
        return 0;
    }

    printf("Opção desconhecida: %s\n", opt);
    return 1;
}
