/*
 * funcionalidades.h - Protótipos das 4 funcionalidades do trabalho.
 *
 * Cada funcionalidade corresponde a uma operação sobre o arquivo binário:
 *   [1] Criar arquivo binário a partir de CSV (CREATE TABLE)
 *   [2] Listar todos os registros ativos (SELECT *)
 *   [3] Buscar registros por critérios de campo (SELECT WHERE)
 *   [4] Buscar registro por RRN (SELECT por posição direta)
 */

#ifndef FUNCIONALIDADES_H
#define FUNCIONALIDADES_H

/*
 * func_criar_tabela(arquivo_csv, arquivo_bin)
 * Lê todos os registros do CSV e grava no arquivo binário.
 * Ao final, chama BinarioNaTela para exibir o conteúdo binário.
 * Em caso de erro: imprime "Falha no processamento do arquivo.\n"
 */
void func_criar_tabela(const char *arquivo_csv, const char *arquivo_bin);

/*
 * func_select_todos(arquivo_bin)
 * Exibe todos os registros não removidos do arquivo binário.
 * Se nenhum for encontrado: imprime "Registro inexistente.\n"
 * Em caso de erro: imprime "Falha no processamento do arquivo.\n"
 */
void func_select_todos(const char *arquivo_bin);

/*
 * func_select_where(arquivo_bin, n)
 * Realiza n buscas sequenciais com critérios de campo lidos de stdin.
 * Para cada busca sem resultado: imprime "Registro inexistente.\n"
 * Em caso de erro: imprime "Falha no processamento do arquivo.\n"
 *
 * Formato de entrada esperado para cada busca:
 *   m nomeCampo1 valorCampo1 [nomeCampo2 valorCampo2 ...]
 * Strings vêm entre aspas duplas; NULO sem aspas.
 */
void func_select_where(const char *arquivo_bin, int n);

/*
 * func_select_rrn(arquivo_bin, rrn)
 * Exibe o registro localizado no RRN informado.
 * Se removido ou inválido: imprime "Registro inexistente.\n"
 * Em caso de erro: imprime "Falha no processamento do arquivo.\n"
 */
void func_select_rrn(const char *arquivo_bin, int rrn);

#endif /* FUNCIONALIDADES_H */
