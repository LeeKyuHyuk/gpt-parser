#include "gptparser.h"
#include "ui_gptparser.h"

#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QHeaderView>

GptParser::GptParser(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GptParser)
{
    ui->setupUi(this);

    QStringList tableHeader, partitionTableHeader;
    tableHeader << "Signature" << "GPT Revision" << "Header Size" << "Header CRC32" << "Current LBA" << "Backup LBA" << "First Usable Partitions" << "Last Usable Partitions" << "Start Partition Entries" << "Number of Partition Entries" << "Partition Entry Size" << "Partition Array CRC32";
    ui->primaryTableWidget->setColumnCount(12);
    ui->primaryTableWidget->setHorizontalHeaderLabels(tableHeader);
    ui->primaryTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    partitionTableHeader << "First LBA (Disk offset)" << "Last LBA (Disk offset)" << "Partition Name";
    ui->partitionTableWidget->setColumnCount(3);
    ui->partitionTableWidget->setHorizontalHeaderLabels(partitionTableHeader);
    ui->partitionTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

GptParser::~GptParser()
{
    delete ui;
}

void GptParser::on_actionOpen_triggered()
{
    int index;
    QString tmpStr;
    QString strFileName = QFileDialog::getOpenFileName(this, "Open a file", QDir::homePath());
    QFile file(strFileName);
    if (!file.open(QIODevice::ReadOnly)) return;
    QByteArray gptData = file.readAll();
    gpt_t *gpt = reinterpret_cast<gpt_t*>(gptData.data());

    QString signature = "";
    for(int i = 0 ; i < 8;i++)
    {
        signature += static_cast<char>(gpt->primary_gpt_header.signature[i]);
    }
    if (signature != "EFI PART")
    {
        QMessageBox messageBox;
        messageBox.critical(this,"Error","Error opening GPT file!");
        messageBox.setFixedSize(500,200);
    }
    else
    {
        ui->primaryTableWidget->setRowCount(0);
        ui->partitionTableWidget->setRowCount(0);
        ui->primaryTableWidget->insertRow(ui->primaryTableWidget->rowCount());
        index = ui->primaryTableWidget->rowCount() - 1;
        tmpStr.sprintf("%s", gpt->primary_gpt_header.signature);
        ui->primaryTableWidget->setItem(index, 0, new QTableWidgetItem(tmpStr));
        tmpStr.sprintf("%d", gpt->primary_gpt_header.gpt_revision);
        ui->primaryTableWidget->setItem(index, 1, new QTableWidgetItem(tmpStr));
        tmpStr.sprintf("%d", gpt->primary_gpt_header.header_size);
        ui->primaryTableWidget->setItem(index, 2, new QTableWidgetItem(tmpStr));
        tmpStr.sprintf("%08X", gpt->primary_gpt_header.header_crc32);
        ui->primaryTableWidget->setItem(index, 3, new QTableWidgetItem(tmpStr));
        tmpStr.sprintf("0x%08llX", gpt->primary_gpt_header.current_lba);
        ui->primaryTableWidget->setItem(index, 4, new QTableWidgetItem(tmpStr));
        tmpStr.sprintf("0x%08llX", gpt->primary_gpt_header.backup_lba);
        ui->primaryTableWidget->setItem(index, 5, new QTableWidgetItem(tmpStr));
        tmpStr.sprintf("0x%08llX", gpt->primary_gpt_header.first_usable_partitions);
        ui->primaryTableWidget->setItem(index, 6, new QTableWidgetItem(tmpStr));
        tmpStr.sprintf("0x%08llX", gpt->primary_gpt_header.last_usable_partitions);
        ui->primaryTableWidget->setItem(index, 7, new QTableWidgetItem(tmpStr));
        tmpStr.sprintf("0x%08llX", gpt->primary_gpt_header.start_partition_entries);
        ui->primaryTableWidget->setItem(index, 8, new QTableWidgetItem(tmpStr));
        tmpStr.sprintf("%d", gpt->primary_gpt_header.number_of_partition_entries);
        ui->primaryTableWidget->setItem(index, 9, new QTableWidgetItem(tmpStr));
        tmpStr.sprintf("0x%08X", gpt->primary_gpt_header.number_of_partition_entries);
        ui->primaryTableWidget->setItem(index, 10, new QTableWidgetItem(tmpStr));
        tmpStr.sprintf("%08X", gpt->primary_gpt_header.partition_array_crc32);
        ui->primaryTableWidget->setItem(index, 11, new QTableWidgetItem(tmpStr));

        for(uint32_t partition = 0; partition < gpt->primary_gpt_header.number_of_partition_entries; partition++)
        {
            if(gpt->guid_partition_entries[partition].first_lba != 0x0 &&
                    gpt->guid_partition_entries[partition].last_lba != 0x00)
            {
                ui->partitionTableWidget->insertRow(ui->partitionTableWidget->rowCount());
                index = ui->partitionTableWidget->rowCount() - 1;
                tmpStr.sprintf("%lld (0x%08llX)", gpt->guid_partition_entries[partition].first_lba, gpt->guid_partition_entries[partition].first_lba * LBA_SIZE);
                ui->partitionTableWidget->setItem(index, 0, new QTableWidgetItem(tmpStr));
                tmpStr.sprintf("%lld (0x%08llX)", gpt->guid_partition_entries[partition].last_lba, gpt->guid_partition_entries[partition].last_lba * LBA_SIZE);
                ui->partitionTableWidget->setItem(index, 1, new QTableWidgetItem(tmpStr));
                tmpStr = "";
                for (int name = 0; name < 72; name++)
                {
                    if(gpt->guid_partition_entries[partition].partition_name[name] != 0x00)
                        tmpStr += static_cast<char>(gpt->guid_partition_entries[partition].partition_name[name]);
                }
                ui->partitionTableWidget->setItem(index, 2, new QTableWidgetItem(tmpStr));
            }
        }
    }
}

void GptParser::resizeEvent(QResizeEvent *)
{
    ui->primaryTableWidget->setFixedSize(this->width(), 65);
    ui->partitionTableWidget->setFixedSize(this->width(), this->height()-190);
}
