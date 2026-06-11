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

/*
 * func_criar_indice(arquivo_bin, arquivo_indice)   [Funcionalidade 5]
 * Lê o arquivo de dados e gera o arquivo de índice primário, ordenado
 * por codEstacao. Ao final, chama BinarioNaTela para exibir o índice.
 * Em caso de erro: imprime "Falha no processamento do arquivo.\n"
 */
void func_criar_indice(const char *arquivo_bin, const char *arquivo_indice);

/*
 * func_select_where_indexado(arquivo_bin, arquivo_indice, n)  [Funcionalidade 6]
 * Realiza n buscas. Quando codEstacao é um dos critérios, usa o índice
 * primário em RAM (busca binária) para acesso direto; caso contrário faz
 * busca sequencial. Não modifica os arquivos.
 * Sem resultado: "Registro inexistente.\n". Erro: "Falha...\n".
 */
void func_select_where_indexado(const char *arquivo_bin,
                                const char *arquivo_indice, int n);

/*
 * func_remover(arquivo_bin, arquivo_indice, n)     [Funcionalidade 7]
 * Remove logicamente os registros que satisfazem cada um dos n critérios,
 * encadeando-os na pilha de removidos do cabeçalho e removendo-os do índice.
 * Ao final exibe o arquivo de dados e o índice via BinarioNaTela.
 */
void func_remover(const char *arquivo_bin, const char *arquivo_indice, int n);

/*
 * func_inserir(arquivo_bin, arquivo_indice, n)     [Funcionalidade 8]
 * Insere n novos registros, reaproveitando espaços da pilha de removidos
 * quando houver, ou ao final do arquivo. Insere as chaves no índice.
 * Ao final exibe o arquivo de dados e o índice via BinarioNaTela.
 */
void func_inserir(const char *arquivo_bin, const char *arquivo_indice, int n);

/*
 * func_atualizar(arquivo_bin, arquivo_indice, n)   [Funcionalidade 9]
 * Para cada uma das n atualizações, localiza os registros pelo critério de
 * busca e aplica os campos a atualizar (SET) in-place. Mantém o índice
 * coerente quando codEstacao muda.
 * Ao final exibe o arquivo de dados e o índice via BinarioNaTela.
 */
void func_atualizar(const char *arquivo_bin, const char *arquivo_indice, int n);

#endif /* FUNCIONALIDADES_H */
