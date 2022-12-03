#ifndef __TVERTICE_H__
#define __TVERTICE_H__

#define IDNAME 100 // Tamanho máximo de um nome de diretório ou arquivo

//#define ASCII_COMPATIBLE
#ifdef ASCII_COMPATIBLE
#define QUINA "\\_"
#define PIPE "| "
#else
#define QUINA "├─"
#define PIPE "│ "
#endif

// define os erros possiveis
#define ERRO_NOME_USADO 1
#define ERRO_ARQUIVO_COMO_DIRETORIO 2
#define ERRO_VERTICE_NAO_ENCONTRADO 3
#define ERRO_PERDENDO_REFERENCIA 4

// Estrutura para os vértices ou seja os diretórios
typedef struct TVertice
{
    char id[IDNAME + 1];      // Nome do diretório ou arquivo
    char tipo;                // Se é Arquivo ‘a’ ou Diretório ‘d’
    struct TVertice *dir_ant; // Diretório anterior..
    struct TVertice *filhos;  // Lista de diretórios ou arquivos a partir do atual
    struct TVertice *prox;    // Lista de diretórios ou arquivos a partir do atual
    int size;                 // só usado se for arquivo
} TVertice;

TVertice *CriarRaiz(const char *id);
TVertice *AdicionarPasta(TVertice *dir, const char *id);
TVertice *AdicionarArquivo(TVertice *dir, const char *id, int size);
int RemoverDoDiretorio(TVertice *dir, const char *nome);
int GetByteSize(TVertice *node);
int Remover(TVertice *node);
void PrintTree(TVertice *node, int profundidade);
TVertice *ProcurarNoDiretorio(TVertice *dir, const char *nome);
TVertice *ProcurarNosDiretoriosAbaixo(TVertice *dir, const char *nome);

TVertice *Carregar(TVertice *dir, char *path);
void PrintList(TVertice *dir);

void ImprimirEmProfundidade(TVertice *dir);
void ImprimirEmLargura(TVertice *dir);

void PrintFullPath(TVertice *dir);

/*
m <endereço relativo> - criar diretório
c <endereço relativo> - mudar diretório
d <endereço relativo> - remover um arquivo
p – Apresentar a árvore realizando um percurso em profundidade
l - Apresentar a árvore realizando um percurso em largura
t - Apresentar a árvore estruturada
h - Imprimir todos os comandos disponíveis
x - sair
*/
int InterpretarComando(TVertice **dir, const char *cmd);

#endif