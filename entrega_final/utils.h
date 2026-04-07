/*
 * utils.h - Declarações das funções fornecidas pela disciplina.
 *
 * As implementações estão em fornecidas.c (arquivo fornecido).
 */

#ifndef UTILS_H
#define UTILS_H

/*
 * BinarioNaTela(char *arquivo)
 * Lê o arquivo binário e exibe um checksum na tela.
 * Deve ser chamada APÓS fechar o arquivo com fclose().
 */
void BinarioNaTela(char *arquivo);

/*
 * ScanQuoteString(char *str)
 * Lê da entrada padrão uma string que pode vir entre aspas duplas.
 * Trata NULO (sem aspas) copiando "" para str.
 * Trata string entre aspas copiando o conteúdo sem as aspas.
 * Trata outros tokens normalmente.
 */
void ScanQuoteString(char *str);

#endif /* UTILS_H */
