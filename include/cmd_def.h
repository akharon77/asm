CMD_DEF(ADD,  ZERO_ARG,
        {
            VAL_TYPE val1 = POP COMMA
                     val2 = POP;
            PUSH(val1 + val2);
        })
CMD_DEF(SUB,  ZERO_ARG,
        {
            VAL_TYPE val1 = POP COMMA
                     val2 = POP;
            PUSH(val2 - val1);
        })
CMD_DEF(MUL,  ZERO_ARG,
        {
            VAL_TYPE val1 = POP COMMA
                     val2 = POP;
            PUSH(val1 * val2);
        })
CMD_DEF(DIV,  ZERO_ARG,
        {
            VAL_TYPE val1 = POP COMMA
                     val2 = POP;
            PUSH(val2 / val1);
        })

CMD_DEF(OUT,  ZERO_ARG,
        {
            printf("%d\n" COMMA ARG);
        })
CMD_DEF(HLT,  ZERO_ARG,
        {
            exit(0);
        })

CMD_DEF(CMP,   TWO_ARG,
        {
            CMD_FLAGS_TYPE flags0 = 0;
                           flags1 = 0,
                           flags2 = 0;

            VAL_TYPE       val1   = 0,
                           val2   = 0;

            flags0 =  CMD    >> BITS_CMD_CONT;
            flags1 =  flags0                     | FLAGS_MASK;
            flags2 = (flags0 >> FLAGS_POS_OCCUP) | FLAGS_MASK;

            val1 = ARG(ARG_VAL_IP(1));
            val2 = ARG(ARG_VAL_IP(2));
        })

CMD_DEF(PUSH,  ONE_ARG)
CMD_DEF(POP,   ONE_ARG)

CMD_DEF(JMP,   LBL_ARG)
CMD_DEF(JA,    LBL_ARG)
CMD_DEF(JAE,   LBL_ARG)
CMD_DEF(JB,    LBL_ARG)
CMD_DEF(JBE,   LBL_ARG)
CMD_DEF(JE,    LBL_ARG)
CMD_DEF(JNE,   LBL_ARG)
