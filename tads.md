Perfeito. Não consigo gravar arquivos diretamente neste modo de execução, mas já deixo abaixo o conteúdo completo do markdown pronto para você colar no repositório e compartilhar com o grupo.

Baseado principalmente em GUIA_IMPLEMENTACAO.md e alinhado com o contexto de README.md.

# Documento para equipe (conteúdo markdown)

## Projeto SCC0503 - Guia de Implementação com TADs em C

### 1. Objetivo deste documento
Este material padroniza a arquitetura do projeto para que toda a equipe implemente de forma consistente, modular e compatível com a especificação da disciplina.

A proposta é usar C com mentalidade de POO por meio de TADs (Tipos Abstratos de Dados), isolando responsabilidades por módulo e evitando lógica de baixo nível espalhada pelo código.

### 2. Resumo técnico do projeto
O arquivo binário final possui:
- 1 cabeçalho de 17 bytes
- N registros de dados de 80 bytes

Estrutura geral em disco:
- [Cabeçalho 17 bytes][Registro 0 80 bytes][Registro 1 80 bytes]...

Regras críticas:
- Escrita em modo binário
- Escrita campo a campo com chamadas individuais de fwrite
- Não gravar struct inteira com um único fwrite
- Strings sem terminador nulo no arquivo
- Preenchimento com cifrão no espaço restante do registro
- Controle de consistência via campo status no cabeçalho

### 3. Decisão de arquitetura: TADs
A equipe vai implementar o projeto em módulos independentes. Cada módulo encapsula dados e operações.

TADs principais:
- TAD Cabecalho
- TAD Registro
- TAD CSV
- TAD Funcionalidades (orquestração)
- TAD Utils (funções fornecidas, leitura de strings com aspas, mensagens padrão)

Benefícios:
- Menor acoplamento
- Código mais testável
- Menos bugs de offset e serialização
- Melhor paralelização entre membros da equipe

### 4. Regra de ouro sobre structs
Podemos e devemos usar struct para organizar dados em memória.

Mas é proibido depender do layout da struct para persistência no binário.
Motivo: padding e alinhamento de compilador podem quebrar offsets e tamanho fixo do registro.

Portanto:
- Em memória: struct
- Em disco: leitura e escrita campo a campo na ordem exata da especificação

### 5. TAD Registro (principal)
Responsável por representar o registro de dados e por serializar e desserializar corretamente.

Campos lógicos do registro:
- removido
- proximo
- codEstacao
- codLinha
- codProxEstacao
- distProxEstacao
- codLinhaIntegra
- codEstIntegra
- tamNomeEstacao
- nomeEstacao
- tamNomeLinha
- nomeLinha

Observação:
Em memória, strings devem ter terminador nulo para facilitar uso em C.
No arquivo, strings devem ser gravadas sem terminador nulo.

### 6. Operações essenciais do TAD Registro
Operações recomendadas para a API do módulo:

1. Inicialização
- registro_inicializar
- Define valores padrão seguros:
  removido igual a 0
  proximo igual a -1
  campos inteiros anuláveis igual a -1
  nomeLinha vazio e tamanho 0

2. Parse e construção a partir do CSV
- registro_from_csv_line
- Converte linha CSV em Registro
- Trata campo vazio como nulo conforme regra do projeto

3. Normalização
- registro_normalizar
- Recalcula tamNomeEstacao e tamNomeLinha a partir das strings
- Garante coerência antes de gravar

4. Validação
- registro_validar
- Checa invariantes:
  codEstacao obrigatório
  nomeEstacao obrigatório
  tamanhos válidos
  limites de buffer respeitados

5. Escrita binária
- registro_escrever_bin
- Escreve campo a campo com fwrite na ordem oficial
- Calcula bytes de lixo e completa com cifrão até 80 bytes

6. Leitura binária
- registro_ler_bin
- Lê campo a campo
- Reconstrói strings em memória com terminador nulo
- Pode indicar estado:
  leitura ok
  fim de arquivo
  registro removido

7. Impressão
- registro_imprimir
- Exibe exatamente no formato exigido
- Mostra NULO para inteiros iguais a -1 e string nula

8. Match para busca
- registro_match_filtro
- Verifica se o registro atende aos critérios de busca da funcionalidade 3

### 7. TAD Cabecalho
Responsável pelo controle global do arquivo binário.

Campos:
- status
- topo
- proxRRN
- nroEstacoes
- nroParesEstacao

Operações essenciais:
- cabecalho_inicializar
- cabecalho_escrever
- cabecalho_ler
- cabecalho_marcar_inconsistente
- cabecalho_marcar_consistente
- cabecalho_atualizar_contadores

Regra operacional:
- Abrir para escrita: status deve virar 0 imediatamente
- Finalizar escrita com sucesso: status deve virar 1 antes do fechamento

### 8. TAD CSV
Responsável por parsing do arquivo de entrada.

Operações essenciais:
- csv_abrir
- csv_pular_cabecalho
- csv_ler_linha
- csv_parse_campos
- csv_fechar

Regras:
- Primeira linha do CSV é cabeçalho e deve ser descartada
- Campo vazio representa nulo
- Colunas devem seguir a ordem definida na especificação

### 9. TAD Funcionalidades
Camada de aplicação, sem lógica de serialização interna.
Ela apenas orquestra TADs.

Funcionalidades:
- funcionalidade 1: CSV para binário
- funcionalidade 2: listar todos
- funcionalidade 3: busca por campos
- funcionalidade 4: busca por RRN

Cada funcionalidade:
- valida abertura de arquivo
- valida status de consistência quando necessário
- delega parsing e acesso ao binário para os TADs corretos
- imprime mensagens padronizadas de erro

### 10. Fórmulas oficiais a centralizar em constantes
- tamanho do cabeçalho: 17
- tamanho do registro: 80
- bytes fixos no registro: 37
- bytes de lixo: 43 menos tamNomeEstacao menos tamNomeLinha
- offset por RRN: 17 mais RRN vezes 80

### 11. Checklist de implementação segura
- Nunca gravar struct inteira no arquivo
- Nunca gravar terminador nulo de string no binário
- Sempre preencher lixo com cifrão
- Sempre manter ordem oficial dos campos
- Sempre atualizar proxRRN em inserções
- Sempre validar status em leituras
- Sempre restaurar status para 1 ao concluir escrita com sucesso

### 12. Estratégia de desenvolvimento em etapas
Etapa 1:
- Implementar TAD Registro básico
- Fazer leitura de CSV e impressão em tela para depuração

Etapa 2:
- Implementar TAD Cabecalho
- Implementar funcionalidade 1 completa

Etapa 3:
- Implementar leitura sequencial e funcionalidade 2

Etapa 4:
- Implementar filtros e funcionalidade 3

Etapa 5:
- Implementar acesso direto por RRN e funcionalidade 4

### 13. Divisão sugerida de tarefas da equipe
Pessoa 1:
- TAD Cabecalho
- Regras de status e consistência

Pessoa 2:
- TAD Registro
- Escrita e leitura binária campo a campo

Pessoa 3:
- TAD CSV
- Parsing e validação de entrada

Pessoa 4:
- Funcionalidades e integração dos módulos

Todos:
- Revisão cruzada de offsets e mensagens de erro
- Testes de regressão por funcionalidade

### 14. Convenções recomendadas
- Nomes de funções com prefixo do módulo
- Constantes de layout em um arquivo único
- Nenhuma lógica de layout hardcoded fora dos TADs de persistência
- Mensagem de erro padronizada:
  Falha no processamento do arquivo.
- Mensagem de ausência:
  Registro inexistente.

### 15. Observação sobre o README atual
O README menciona pacientes em alguns trechos, enquanto o projeto trata de estações e linhas.
Para evitar confusão da equipe, considerar alinhar esse texto com o domínio real do trabalho quando possível.

---

Se quiser, no próximo passo eu já te entrego uma versão 2 desse mesmo documento com uma seção de interface sugerida de arquivos e assinaturas de funções por módulo, pronta para virar padrão interno da equipe.