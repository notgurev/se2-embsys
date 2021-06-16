#include <stdio.h>
#include <windows.h>

/*
 Написать программу, которая сохраняет информацию о секциях,
 адресе точки входа в текстовом виде в один файл
 и сохраняет в бинарном виде программный код в другой файл
 Протестировать написанную программу на любом исполняемом файле, полученном по результатам первой части мастер-класса
 */

int main() {
    FILE *exe = fopen("../executable.exe", "rb");

    IMAGE_DOS_HEADER dos_header;
    IMAGE_NT_HEADERS headers;

    // read dos header
    fread(&dos_header, sizeof(IMAGE_DOS_HEADER), 1, exe);

    // move to start of PE header
    fseek(exe, dos_header.e_lfanew, SEEK_SET);

    // read other stuff
    fread(&headers, sizeof(IMAGE_NT_HEADERS), 1, exe);

    WORD number_of_sections = headers.FileHeader.NumberOfSections;
    IMAGE_SECTION_HEADER section_headers[number_of_sections];

    for (int i = 0; i < number_of_sections; ++i) {
        fread(&section_headers[i], sizeof(IMAGE_SECTION_HEADER), 1, exe);
    }

    // print sections info

    FILE *out_sections = fopen("../sections.txt", "w");
    fprintf(out_sections, "Entry point address: %p\n\n", headers.OptionalHeader.AddressOfEntryPoint);

    DWORD pointer;
    DWORD size;
    for (int i = 0; i < number_of_sections; ++i) {
        IMAGE_SECTION_HEADER section = section_headers[i];
        if (strcmp(section.Name, ".text") == 0) {
            pointer = section.PointerToRawData;
            size = section.SizeOfRawData;
        }
        fprintf(out_sections, "Section name: %s\n", section.Name);
        fprintf(out_sections, "Virtual address: %p\n", section.VirtualAddress);
        fprintf(out_sections, "Virtual size: %lu\n", section.Misc.VirtualSize);
        fprintf(out_sections, "Characteristics: %X\n", section.Characteristics);
        fprintf(out_sections, "\n");
    }

    fclose(out_sections);

    // print code

    FILE *out_code = fopen("../code.bin", "wb");

    fseek(exe, pointer, SEEK_SET);

    printf("Size to read: %lu\n", size);
    char buf[size];

    size_t read = fread(buf, sizeof(char), size, exe);
    if (read != size) printf("Error reading code\n");
    printf("Read: %zu\n", read);

    size_t written = fwrite(buf, sizeof(char), size, out_code);
    if (written != size) printf("Error writing code\n");
    printf("Written: %zu\n", written);

    fclose(out_code);

    fclose(exe);
    return 0;
}