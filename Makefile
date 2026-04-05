# Makefile - SCC0503 Trabalho Prático 1
#
# Compila todos os arquivos .c da raiz do projeto.
# A flag -lmd é necessária para a função BinarioNaTela fornecida pela disciplina.
#
# Uso:
#   make       → compila o executável 'programaTrab'
#   make run   → executa o programa
#   make clean → remove executável e arquivos .bin gerados

all:
	gcc -o programaTrab *.c -lmd

run:
	./programaTrab

clean:
	rm -f programaTrab *.bin
