CXX       = g++
CXX_FLAGS = -I ./include -g -ggdb3 -std=c++2a -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-check -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -fPIE -pie -Wlarger-than=8192 -Wstack-usage=8192

all: asm cpu disasm

asm: asm.out

disasm: disasm.out

cpu: cpu.out

asm.out: obj/main_asm.o obj/asm.o obj/iostr.o obj/cmds.o
	$(CXX) $(CXX_FLAGS) $^ -o $@

disasm.out: obj/main_disasm.o obj/cpu.o obj/stack.o obj/cmds.o obj/iostr.o
	$(CXX) $(CXX_FLAGS) $^ -o $@

cpu.out: obj/main_cpu.o obj/cpu.o obj/stack.o obj/iostr.o obj/cmds.o
	$(CXX) $(CXX_FLAGS) $^ -o $@

obj/%.o: src/%.cpp
	$(CXX) $(CXX_FLAGS) -c $^ -o $@

clean:
	rm -rf obj/* *.o *.d *.out 
