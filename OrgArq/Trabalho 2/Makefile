all: binarioNaTela.o execution.o main.o
	gcc binarioNaTela.o execution.o main.o -o programaTrab
binarioNaTela.o: binarioNaTela.c
	gcc binarioNaTela.c -c
execution.o: execution.c
	gcc execution.c -c
main.o: main.c
	gcc main.c -c
run:
	./programaTrab
clean:
	rm -rf programaTrab *.o
