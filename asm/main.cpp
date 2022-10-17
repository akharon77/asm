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
    const char *input_filename  = "input.asm", 
               *output_filename = "output.bin";

    int options[N_EXEC_OPTIONS] = {};

    bool ok = GetOptions(argc, argv, options);
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

    if (options[FILE_OPTION])
        input_filename  = argv[options[FILE_OPTION] + 1];
    if (options[OUTPUT_FILE_OPTION])
        output_filename = argv[options[OUTPUT_FILE_OPTION] + 1];

    Asm asmbler = {};
    AsmCtor(&asmbler, input_filename, &err);

    AsmRun(&asmbler);

    int fd_output = creat(output_filename, S_IRWXU);

    write(fd_output, (void*) &asmbler.instr_ptr, BYTES_SIZE);
    write(fd_output, asmbler.buf,                asmbler.instr_ptr);

    close(fd_output);

    AsmDtor(&asmbler);

    return 0;
}

