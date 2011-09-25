#include "drivers/ata.h"
#include "type.h"
#include "system/io.h"
#include "drivers/pci.h"

struct DriveInfo {
    size_t len;
    unsigned char number;
    unsigned char mode;
    unsigned short cylinders;
    unsigned char heads;
    unsigned char sectors;
    unsigned short ports;
};

#define BASE_PORT 0x1F0

#define PORT(n) (BASE_PORT + n)

#define DATA_PORT PORT(0)
#define INFO_PORT PORT(1)
#define SECTOR_COUNT_PORT PORT(2)
#define LBA_LOW_PORT PORT(3)
#define LBA_MID_PORT PORT(4)
#define LBA_HIGH_PORT PORT(5)
#define DRIVE_PORT PORT(6)
#define COMMAND_PORT PORT(7)
#define STATUS_PORT COMMAND_PORT

#define ERR(status) (status & 0x1)
#define DRQ(status) (status & (0x1 << 3))
#define SRV(status) (status & (0x1 << 4))
#define DF(status) (status & (0x1 << 5))
#define RDY(status) (status & (0x1 << 6))
#define BSY(status) (status & (0x1 << 7))

#define CONTROL_REGISTER 0x3F6

#define MASTER 0xA0
#define IDENTIFY 0xEC

unsigned long long sectors = 0L;

unsigned int bar4;


void ata_init(struct multiboot_info* info) {

    if (info->flags & (0x1 << 7)) {

        if (info->drives_length) {

            struct DriveInfo* drive = (struct DriveInfo*) info->drives_addr;
            if (drive->number == 0x80 && drive->mode) {
                sectors = drive->cylinders * drive->heads * drive->sectors;
                if (sectors > (1 << 28)) {
                    sectors = 1 << 28;
                }
            }

            if (inB(DRIVE_PORT) != MASTER) {
                //panic();
            }

            struct PCIDevice device;
            if (pci_find_device(&device, 0x01, 0x01) == -1) {
                panic();
            }

            bar4 = pci_read_config(&device, 0x20) & (~0x3);
            printf("BAR#4 %u\n", bar4);
            printf("At BAR#4 %u\n", inB(bar4));
            printf("At BAR#4 + 2 %u\n", inB(bar4 + 0x2));
            printf("At BAR#4 + 4 %u\n", inD(bar4 + 0x4));
            outB(bar4, 0x1);
        }
    }

    printf("Drive with sectors: %d\n", sectors);
}

