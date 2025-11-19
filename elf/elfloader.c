#include <stdio.h>
#include <stdlib.h>
#include "elfheader.h"
#include <elf.h>
#include <sys/mman.h>
int main(int argc, char *argv[]) {
  //printf("%d", argc);
  if(argc != 4){
    fprintf(stderr, "Intended Usage: ./elfloader file arg1 arg2\n");
    return 1;
  }
  char* filename;
  int a;
  int b;
  filename = argv[1];
  a = atoi(argv[2]);
  b = atoi(argv[3]);

  FILE *file;
  file = fopen(filename, "rb");
  Elf64_Ehdr head;
  fread(&head, sizeof(head), 1, file);
  // e_ident id = head.e_ident;
  if(head.e_ident[0] == 0x7f && head.e_ident[1] == 'E'
     && head.e_ident[2] == 'L' && head.e_ident[3] == 'F') {
    printf("Confirmed Elf File\n");
  }
  
  // int offset = 0;
  //fclose(file);
  //file = fopen(filename, "rb");
  fseek(file, head.e_phoff, SEEK_SET);
  char* buf[50];
  Elf64_Phdr phd;
  fread(&phd, sizeof(phd), 1, file);
  if(phd.p_type == PT_LOAD){  // Checks if the type is LOAD
    printf("type = LOAD\n");
  } else {
    fclose(file);
    return 3;
  }
  if(phd.p_flags & PF_X){  // Checks if the file is executable
    printf("executable!\n");
  } else {
    fclose(file);
    return 3;
  }
  printf("phd.p_type == %u, phd.p_offset = %u, phd.p_vaddr = %u\n", phd.p_type,  phd.p_offset, phd.p_vaddr);
  fseek(file, phd.p_offset, SEEK_SET);
  void* codep = mmap(NULL, phd.p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC,
		     MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);


  int offset;
  fread(codep,phd.p_memsz, 1, file);
  if(head.e_entry == 0){
    offset = 0;
  } else if (head.e_entry != 0){
    if(phd.p_vaddr > 0){
      offset = phd.p_vaddr - head.e_entry;
    } else {
      offset = head.e_entry;
    }
  }

  int result = 0;
  //fread(&codep, phd.p_memsz, 1, file); 
  unsigned int (*mainfunc)(int, int) = codep;
  result = (*mainfunc)(a, b);
  
  printf("Answer: %d\n", result);
  munmap(codep, phd.p_memsz);
  fclose(file);

  
  return 0;
}
