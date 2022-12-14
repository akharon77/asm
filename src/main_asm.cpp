#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "iostr.h"
#include "asm.h"
#include "cmds.h"
#include "colors.h"

int main(int argc, const char* argv[])
{
    int err = 0;
    const char *input_filename   = "input.asm", 
               *output_filename  = "output.bin",
               *listing_filename = "listing.txt";

    int options[N_EXEC_OPTIONS] = {};

    bool ok = GetOptions(argc, argv, options, EXEC_OPTIONS, N_EXEC_OPTIONS);
    if (!ok)
    {
        printf(RED "Wrong arguments\n" NORMAL);
        return 1;
    }

    if (options[HELP_OPTION])
    {
        for (int i = 0; i < N_EXEC_OPTIONS; ++i)
            printf("%10s %5s %s\n",
                    EXEC_OPTIONS[i].strFormLong,
                    EXEC_OPTIONS[i].strFormShort,
                    EXEC_OPTIONS[i].description);
        return 0;
    }

    if (options[INPUT_FILE_OPTION])
        input_filename  = argv[options[INPUT_FILE_OPTION] + 1];
    if (options[OUTPUT_FILE_OPTION])
        output_filename = argv[options[OUTPUT_FILE_OPTION] + 1];
    if (options[LISTING_FILE_OPTION])
        output_filename = argv[options[LISTING_FILE_OPTION] + 1];

    Asm asmbler = {};
    AsmCtor(&asmbler, input_filename, &err);

    AsmRun(&asmbler);
    AsmListing(&asmbler, listing_filename);

    AsmOut(&asmbler, output_filename);

    AsmDtor(&asmbler);

    return 0;
}

