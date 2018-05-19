#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <elf.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/procfs.h>
#include <fcntl.h>

int main(int argc, char** argv){
    printf("Bad ELF\n");

    if(argc < 2){
        printf("Usage: %s file", argv[0]);
        return 1;
    }
    else{

        int f;
        static Elf64_Ehdr* header;

        if((f = open(argv[1], O_RDWR)) < 0){
            perror("open");
            return 1;
        }

        if((header = (Elf64_Ehdr *) mmap(NULL, sizeof(header), PROT_READ |
                        PROT_WRITE, MAP_SHARED, f, 0)) == MAP_FAILED){
            perror("mmap");
            close(f);
            return 1;
        }
        printf("[*] Patching..\n");

        header->e_ident[4] = 0x00;
        header->e_ident[5] = 0x00;
        header->e_version = 0x0;
        header->e_ehsize = 0x100;
        header->e_shentsize = 0x00;
        header->e_shoff = 256;
        header->e_shnum = 0x00;
        header->e_shstrndx = 0x00;

        printf("[*] Patched\n");

        if(msync(NULL, 0, MS_SYNC) == -1){
            perror("msync");
            close(f);
            return 1;
        }

        close(f);
        munmap(header, 0);
    }
    return 0;
}