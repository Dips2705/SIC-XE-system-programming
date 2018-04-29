sicsim.out : main.o proc_command.o base.o memory.o opcode_table.o symbol_table.o assembler.o external_symbol_table.o linkig_loader.o processor.o debugger.o
	gcc -W -o sicsim.out main.o proc_command.o base.o memory.o opcode_table.o symbol_table.o assembler.o external_symbol_table.o linking_loader.o processor.o debugger.o
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
symbol_table.o : symbol_table.c symbol_table.h
	gcc -W -c symbol_table.c
assembler.o : assembler.c assembler.h
	gcc -W -c assembler.c
external_symbol_table.o: external_symbol_table.c external_symbol_table.h
	gcc -W -c external_symbol_table.c
linking_loader.o: linking_loader.c linking_loader.h
	gcc -W -c linking_loader.c
processor.o: processor.c processor.h
	gcc -W -c processor.c
debugger.o: debugger.c debugger.h
	gcc -W -c debugger.c
clean:
	rm sicsim.out main.o proc_command.o base.o memory.o opcode_table.o symbol_table.o assembler.o external_symbol_table.o linking_loader.o processor.o debugger.o
