SIC/XE Machine Shell, Assembler, Loader and Linker
==================================================
SIC/XE Machine의 Assembly Code에 대한 Assembler, Linking loader입니다.
Assemble 명령어를 포함한 몇가지 명령어를 수행하는 Shell이 동작합니다.
최종적으로 SIC/XE 가상머신을 구현하여, 가상 메모리에 올라간 기계어를 실행합니다.

## How to compile the code

	$ make

## How to run

	$ ./sicsim.out

## How to clean the file

	$ make clean

## Shell Command
	
	h[elp]
	d[ir]
	q[uit]
	hi[story]
	du[mp] [start, end]
	e[dit] address, value
	f[ill] start, end, value
	reset
	opcode mnemonic
	opcodelist
	assemble filename
	type filename
	symbol
	progaddr [address]
	loader [object filename1] [object filename2] [...]
	run
	bp [address/clear]

## Reference

* Leland L. Beck, System Software: An Introduction to Systems Programming (3rd ed), Addison-Wesley
