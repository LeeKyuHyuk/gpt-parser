#ifndef GPTPARSER_H
#define GPTPARSER_H

#include <QMainWindow>

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

namespace Ui {
class GptParser;
}

class GptParser : public QMainWindow
{
    Q_OBJECT

public:
    explicit GptParser(QWidget *parent = nullptr);
    ~GptParser();

private slots:
    void on_actionOpen_triggered();

private:
    Ui::GptParser *ui;

    void resizeEvent(QResizeEvent*);
};

#endif // GPTPARSER_H
