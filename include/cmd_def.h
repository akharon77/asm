CMD_DEF(ADD,  ZERO_ARG,
        {
            INC(CMD_FLAGS);
            VAL_TYPE val1 = POP COMMA
                     val2 = POP;
            PUSH(val1 + val2);
        })
CMD_DEF(SUB,  ZERO_ARG,
        {
            INC(CMD_FLAGS);
            VAL_TYPE val1 = POP COMMA
                     val2 = POP;
            PUSH(val2 - val1);
        })
CMD_DEF(MUL,  ZERO_ARG,
        {
            INC(CMD_FLAGS);
            VAL_TYPE val1 = POP COMMA
                     val2 = POP;
            PUSH(val1 * val2);
        })
CMD_DEF(DIV,  ZERO_ARG,
        {
            INC(CMD_FLAGS);
            VAL_TYPE val1 = POP COMMA
                     val2 = POP;
            PUSH(val2 / val1);
        })

CMD_DEF(OUT,  ZERO_ARG,
        {
            INC(CMD_FLAGS);
            printf("%d\n" COMMA ARG);
        })
CMD_DEF(HLT,  ZERO_ARG,
        {
            INC(CMD_FLAGS);
            exit(0);
        })

CMD_DEF(CMP,   TWO_ARG,
        {
            CMD_FLAGS_TYPE flags0 = 0 COMMA
                           flags1 = 0 COMMA
                           flags2 = 0;

            VAL_TYPE       val1   = 0 COMMA
                           val2   = 0;

            flags0 = ARG(FLAGS);
            INC(FLAGS);

            flags1 =  flags0                     & FLAGS_MASK;
            flags2 = (flags0 >> FLAGS_POS_OCCUP) & FLAGS_MASK;

            GET_VAL(flags1 COMMA val1);

            if (flags1 & FLG_MEM)
                val1 = MEM[val1];

            GET_VAL(flags2 COMMA val2);

            if (flags2 & FLG_MEM)
                val2 = MEM[val2];

            if (val1 == val2)
                ZF = 1;
            else
                ZF = 0;

            if (val1 > val2)
                CF = 0;
            else
                CF = 1;
        })

CMD_DEF(PUSH,  ONE_ARG,
        {
            CMD_FLAGS_TYPE flags = 0;
            VAL_TYPE       val   = 0;

            flags = ARG(FLAGS) & FLAGS_MASK;
            INC(FLAGS);

            GET_VAL(flags COMMA val);

            if (flags & FLG_MEM)
                val = MEM[val];
                
            PUSH(val);
        })
CMD_DEF(POP,   ONE_ARG,
        {
            CMD_FLAGS_TYPE flags = 0;
            VAL_TYPE       val   = 0;

            flags = ARG(FLAGS) & FLAGS_MASK;
            INC(FLAGS);

            GET_VAL(flags COMMA val);

            VAL_TYPE pop_val = POP;

            if (flags & FLG_MEM)
                MEM[val] = pop_val;
        })

JMP_DEF(JMP,   true)
JMP_DEF(JA,    ZF == 0 && CF == 0)
JMP_DEF(JAE,   ZF == 1 || CF == 0)
JMP_DEF(JB,    ZF == 0 && CF == 1)
JMP_DEF(JBE,   ZF == 1 || CF == 1)
JMP_DEF(JE,    ZF == 1)
JMP_DEF(JNE,   ZF == 0)

