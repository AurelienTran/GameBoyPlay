#include <Memory.h>
#include <Z80.h>
#include <Log.h>

int main(int argc, char *argv[])
{
    LOG_INFO("Initialization\n");
    Memory_Initialize();
    Z80_Initialize();

    LOG_INFO("Load bootstrap\n");
    Memory_LoadFile("rom/bootstrap.bin", 0);

    LOG_INFO("Run game\n");
    for(;;)
    {
        Z80_Print();
        Z80_Run();
    }

    return 0;
}
