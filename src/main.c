

#include "sys.h"
#include "bios.h"
#include "disk.h"


const int __gba_multiboot;
#define GBA_BACK_COLOR *((u16 *) 0x05000000)

void printError(u8 resp);
u8 diskTest();

int main() {

    u8 resp;
    sysInit();

    bi_init();
    gConsPrint("EverDrive-GBA IO demo 05.09.16");
    gConsPrint("");
    
    resp = diskTest();
    if (resp)printError(resp);

    
    gConsPrint("Press any key to reboot");
    sysJoyWait();
    diskPowerDown();
    bi_reboot(1);
    for (;;);
}

u8 diskTest() {

    u8 resp;
    u8 buff[512];
    u32 offset=0;

    resp = diskInit();
    if (resp)return resp;

    resp = diskRead(0, buff, 1);
    if (resp)return resp;

    //check if FAT32 located in the begin of disk, or it has some offset
    if (buff[0x52] != 'F') {
        offset = buff[0x1c6] | (buff[0x1c7] << 8) | (buff[0x1c8] << 16) | (buff[0x1c9] << 24);
        resp = diskRead(offset, buff, 1);
        if (resp)return resp;
    }

    gConsPrint("boot sector offset: ");
    gAppendHex16(offset >> 16);
    gAppendHex16(offset);
    gConsPrint("boot sector content: ");
    gConsPrint("------------------------------");
    gConsPrint((char*)&buff[3]); //MSDOS...
    gConsPrint((char*)&buff[0x47]); //print partition name
    gConsPrint((char*)&buff[0x1AE]); //disk error message
    gConsPrint((char*)&buff[0x1AE + 30]);
    gConsPrint((char*)&buff[0x1AE + 60]);
    gConsPrint("------------------------------");
    

    return 0;
}

void printError(u8 resp) {

    //gCleanScreen();
    gConsPrint("ERROR: ");
    gAppendHex8(resp);

    //sysJoyWait();
}
