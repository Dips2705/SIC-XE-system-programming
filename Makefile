sicsim.out : main.o proc_command.o base.o memory.o opcode_table.o
	gcc -W -o sicsim.out main.o proc_command.o base.o memory.o opcode_table.o
main.o : main.c main.h
	gcc -W -c main.c
proc_command.o : proc_command.c proc_command.h
	gcc -W -c proc_command.c
base.o : base.c base.h
	gcc -W -c base.c
memory.o : memory.c memory.h
	gcc -W -c memory.c
opcode_table.o : opcode_table.c opcode_table.h
	gcc -W -c opcode_table.c
clean:
	rm sicsim.out main.o proc_command.o base.o memory.o opcode_table.o
