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
CMD_DEF(SQRT, ZERO_ARG,
        {
            INC(CMD_FLAGS);
            VAL_TYPE val = POP;
            PUSH(sqrtl(val));
        })

CMD_DEF(INP,  ZERO_ARG,
        {
            INC(CMD_FLAGS);
            double val = 0;
            scanf("%lg" COMMA &val);
            PUSH(val * 1000);
        })
CMD_DEF(OUT,  ZERO_ARG,
        {
            INC(CMD_FLAGS);
            printf("%.3lf\n" COMMA TOP / 1000.);
        })
CMD_DEF(VMOUT, ZERO_ARG,
        {
            INC(CMD_FLAGS);
            for (int x = 0; x < HEIGHT; ++x)
            {
                for (int y = 0; y < WIDTH; ++y)
                {
                    if (MEM[REG(RVB) + x * WIDTH + y])
                        putc(MEM[REG(RVB) + x * WIDTH + y] COMMA stdout);
                    else
                        putc('.' COMMA stdout);
                }
                putc('\n' COMMA stdout);
            }

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

            flags0 = ARG(CMD_FLAGS);
            INC(CMD_FLAGS);

            flags1 = FLAGS(flags0);
            /*flags1 =  flags0                     & FLAGS_MASK;*/

            flags0 = RET_INC_FLAGS(flags0);
            flags2 = FLAGS(flags0);

            GET_VAL(flags1, val1);

            GET_VAL(flags2, val2);

            if (val1 == val2)
                RFLAGS |=   (1 << ZF_IND);
            else
                RFLAGS &= ~(1 << ZF_IND);

            if (val1 > val2)
                RFLAGS &= ~(1 << CF_IND);
            else
                RFLAGS |=   1 << CF_IND;
        })

CMD_DEF(PUSH,  ONE_ARG,
        {
            CMD_FLAGS_TYPE flags = 0;
            VAL_TYPE       val   = 0;

            flags = FLAGS(ARG(CMD_FLAGS));
            /*flags = ARG(CMD_FLAGS) & FLAGS_MASK;*/
            INC(CMD_FLAGS);

            GET_VAL(flags, val);

            PUSH(val);
        })
CMD_DEF(POP,   ONE_ARG,
        {
            CMD_FLAGS_TYPE flags = 0;
            VAL_TYPE       val   = 0;

            flags = FLAGS(ARG(CMD_FLAGS));
            /*flags = ARG(CMD_FLAGS) & FLAGS_MASK;*/
            INC(CMD_FLAGS);

            VAL_TYPE pop_val = POP;

            if (flags & FLG_MEM)
            {
                if (flags & FLG_IMM)
                {                           
                    val += ARG(VAL);        
                    INC(VAL);               
                }                           
                                            
                if (flags & FLG_REG)        
                {                           
                    val += REGS[ARG(REG)];  
                    INC(REG);               
                }                           

                MEM[val]  = pop_val;
            }
            else if (flags & FLG_REG)
            {
                REGS[ARG(REG)] = pop_val;
                INC(REG);
            }
            else
            {
                INC(VAL);
            }
        })

CMD_DEF(CALL,  LBL_ARG,
        {
            INC(CMD_FLAGS);
            LBL_TYPE lbl = ARG(LBL);
            INC(LBL);
            PUSH_CALLSTK(IP);
            IP = lbl;
        })
CMD_DEF(RET,   ZERO_ARG,
        {
            INC(CMD_FLAGS);
            IP = POP_CALLSTK;
        })

JMP_DEF(JMP,   true)
JMP_DEF(JA,    ZF == 0 && CF == 0)
JMP_DEF(JAE,   ZF == 1 || CF == 0)
JMP_DEF(JB,    ZF == 0 && CF == 1)
JMP_DEF(JBE,   ZF == 1 || CF == 1)
JMP_DEF(JE,    ZF == 1)
JMP_DEF(JNE,   ZF == 0)

