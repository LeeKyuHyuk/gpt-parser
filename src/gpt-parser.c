/********************************************************
* gpt-parser.c - Print GPT Infomation                   *
*                                                       *
* Author:  KyuHyuk Lee                                  *
* E-Mail:  lee@kyuhyuk.kr                               *
*                                                       *
********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define LBA_SIZE 512

#pragma pack(1)

typedef struct {
        uint8_t signature[8];
        uint32_t gpt_revision;
        uint32_t header_size;
        uint32_t header_crc32;
        uint32_t reserved1;
        uint64_t current_lba;
        uint64_t backup_lba;
        uint64_t first_usable_partitions;
        uint64_t last_usable_partitions;
        uint8_t uuid[16];
        uint64_t start_partition_entries;
        uint32_t number_of_partition_entries;
        uint32_t partition_entry_size;
        uint32_t partition_array_crc32;
        uint8_t reserved2[420];
} primary_gpt_header_t;

typedef struct {
        uint8_t partition_type_guid[16];
        uint8_t unique_partition_guid[16];
        uint64_t first_lba;
        uint64_t last_lba;
        uint64_t attribute_flags;
        uint8_t partition_name[72];
} guid_partition_entrie_t;

typedef struct {
        uint8_t protective_mbr[512];
        primary_gpt_header_t primary_gpt_header;
        guid_partition_entrie_t guid_partition_entries[128];
} gpt_t;

#pragma pack()

int main(int argc, char const *argv[]) {
        int partition, name;

        if(argc != 2) {
                fprintf(stderr,"Usage: %s <file name>\n",argv[0]);
                return EXIT_FAILURE;
        }

        FILE *gptFile = fopen(argv[1], "r");
        if(gptFile == NULL) {
                fprintf(stderr, "Error opening gpt file\n");
                return EXIT_FAILURE;
        }
        gpt_t gpt;
        if(fread((void *)&gpt, 1, sizeof(gpt_t), gptFile) != sizeof(gpt_t)) {
                fprintf(stderr,"Error reading gpt\n");
                return EXIT_FAILURE;
        }

        printf("##### Primary GPT Header #####\n");
        printf("Signature : %s\n", gpt.primary_gpt_header.signature);
        printf("GPT Revision : %d\n", gpt.primary_gpt_header.gpt_revision);
        printf("Header Size : %d\n", gpt.primary_gpt_header.header_size);
        printf("Header CRC32 : 0x%08X\n", gpt.primary_gpt_header.header_crc32);
        printf("Current LBA : 0x%08lX\n", gpt.primary_gpt_header.current_lba);
        printf("Backup LBA : 0x%08lX\n", gpt.primary_gpt_header.backup_lba);
        printf("First Usable Partitions : 0x%08lX\n", gpt.primary_gpt_header.first_usable_partitions);
        printf("Last Usable Partitions : 0x%08lX\n", gpt.primary_gpt_header.last_usable_partitions);
        printf("Start Partition Entries : 0x%08lX\n", gpt.primary_gpt_header.start_partition_entries);
        printf("Number of Partition Entries : %d\n", gpt.primary_gpt_header.number_of_partition_entries);
        printf("Partition Entry Size : 0x%08x\n", gpt.primary_gpt_header.partition_entry_size);
        printf("Partition Array CRC32 : %#08x\n\n", gpt.primary_gpt_header.partition_array_crc32);

        for(partition = 0; partition < gpt.primary_gpt_header.number_of_partition_entries; partition++) {
                if(gpt.guid_partition_entries[partition].first_lba != 0x0 &&
                   gpt.guid_partition_entries[partition].last_lba != 0x00) {
                        printf("##### GUID Partition Entries [%d] #####\n", partition);
                        printf("First LBA : %ld\n", gpt.guid_partition_entries[partition].first_lba);
                        printf("\t=> Disk Offset : 0x%08lX\n", gpt.guid_partition_entries[partition].first_lba * LBA_SIZE);
                        printf("Last LBA : %ld\n", gpt.guid_partition_entries[partition].last_lba);
                        printf("\t=> Disk Offset : 0x%08lX\n", gpt.guid_partition_entries[partition].last_lba * LBA_SIZE);
                        printf("Partition Name : ");
                        for (name = 0; name < 72; name++)
                                printf("%c", gpt.guid_partition_entries[partition].partition_name[name]);
                        printf("\n\n");
                }
        }

        return 0;
}
