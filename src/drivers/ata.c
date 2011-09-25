#include "drivers/ata.h"
#include "type.h"
#include "system/io.h"
#include "drivers/pci.h"
#include "system/mm.h"

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

#define DMA_STATUS (bar4 + 0x2)
#define DMA_CMD (bar4)
#define DMA_PRDT (bar4 + 0x4)

#define ERR(status) (status & 0x1)
#define DRQ(status) (status & (0x1 << 3))
#define SRV(status) (status & (0x1 << 4))
#define DF(status) (status & (0x1 << 5))
#define RDY(status) (status & (0x1 << 6))
#define BSY(status) (status & (0x1 << 7))

#define CONTROL_REGISTER 0x3F6

#define MASTER 0xA0

#define LAST_ENTRY 0x8000u

struct PRD {
    unsigned int buffer;
    unsigned short bytes;
    unsigned short last;
};

static unsigned long long sectors = 0L;

static unsigned int bar4;

static struct PRD* entry;

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

            outB(DRIVE_PORT, MASTER);
            if (inB(DRIVE_PORT) != MASTER) {
                panic();
            }

            struct PCIDevice device;
            if (pci_find_device(&device, 0x01, 0x01) == -1) {
                panic();
            }

            bar4 = pci_read_config(&device, 0x20) & (~0x3);

            entry = allocPage();

            entry->last = LAST_ENTRY;
            entry->bytes = PAGE_SIZE;

            outD(DMA_PRDT, (unsigned int) entry);
        }
    }

    printf("Drive with sectors: %d\n", sectors);
}

void test_run(void) {
    int* buffer = allocPage();
    buffer[0] = 0xFFFFFFFF;
    entry->buffer = (unsigned int) buffer;

    outB(DMA_CMD, 0x1 | (0x1 << 8));
    outB(DRIVE_PORT, MASTER);
    outB(SECTOR_COUNT_PORT, 0x8);
    outB(LBA_LOW_PORT, 0x0);
    outB(LBA_MID_PORT, 0x0);
    outB(LBA_HIGH_PORT, 0x0);

    outB(COMMAND_PORT, 0xC8);
}

