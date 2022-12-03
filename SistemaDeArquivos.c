#include "SistemaDeArquivos.h"
#include <stdlib.h> //malloc
#include <stdio.h>  //prinf
#include <string.h> //prinf
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h> //dirent

TVertice *CriarRaiz(const char *id)
{
    TVertice *v = (TVertice *)malloc(sizeof(TVertice));
    strcpy(v->id, id);
    v->tipo = 'd';
    v->dir_ant = v;
    v->filhos = NULL;
    v->prox = NULL;
    v->size = 0;
    return v;
}

TVertice *AdicionarPasta(TVertice *dir, const char *id)
{
    if (dir->tipo != 'd')
        return NULL;
    TVertice *v = (TVertice *)malloc(sizeof(TVertice));
    strcpy(v->id, id);
    v->tipo = 'd';
    v->dir_ant = dir;
    v->filhos = NULL;
    v->prox = NULL;
    v->size = 0;

    TVertice *irmao = dir->filhos;
    if (irmao == NULL)
        dir->filhos = v;
    else
    {
        while (irmao->prox != NULL)
        {
            irmao = irmao->prox;
        }
        irmao->prox = v;
    }
    return v;
}

TVertice *AdicionarArquivo(TVertice *dir, const char *id, int size)
{
    if (dir->tipo != 'd')
        return NULL;
    TVertice *v = (TVertice *)malloc(sizeof(TVertice));
    strcpy(v->id, id);
    v->tipo = 'a';
    v->dir_ant = dir;
    v->filhos = NULL;
    v->prox = NULL;
    v->size = size;

    TVertice *irmao = dir->filhos;
    if (irmao == NULL)
        dir->filhos = v;
    else
    {
        while (irmao->prox != NULL)
        {
            irmao = irmao->prox;
        }
        irmao->prox = v;
    }
    TVertice *pai = dir;
    do
    {
        pai->size += size;
        pai = pai->dir_ant;
    } while (pai->dir_ant != pai);

    return v;
}

int RemoverDoDiretorio(TVertice *dir, const char *nome)
{
    if (dir->filhos == NULL)
        return ERRO_VERTICE_NAO_ENCONTRADO;

    TVertice *atual = dir->filhos;

    // se o primogenito é o procurado
    if (!strcmp(atual->id, nome))
    {
        dir->filhos = atual->prox;
        dir->size -= atual->size;
        atual->prox = NULL;
        Remover(atual);
        return 0;
    }
    TVertice *irmao = atual->prox;

    while (irmao != NULL)
    {
        if (!strcmp(irmao->id, nome))
        {
            // remove
            atual->prox = irmao->prox;
            dir->size -= irmao->size;
            irmao->prox = NULL;
            Remover(irmao);
            return 0;
        }
        atual = irmao;
        irmao = irmao->prox;
    }
    return ERRO_VERTICE_NAO_ENCONTRADO;
}

int GetByteSize(TVertice *node)
{
    return node->size;
}

int Remover(TVertice *node)
{
    if (node->prox != NULL)
    {
        printf("Erro de implementação: perda de referencia\n");
        return ERRO_PERDENDO_REFERENCIA;
    }
    TVertice *filho = node->filhos;
    while (filho != NULL)
    {
        TVertice *atual = filho;
        filho = filho->prox;
        atual->prox = NULL;
        Remover(atual);
    }
    free(node);
    return 0;
}

void PrintTree(TVertice *node, int profundidade)
{
    for (int i = 0; i < profundidade - 1; i++)
        printf(PIPE);
    if (profundidade > 0)
        printf(QUINA);
    printf(node->id);
    if (node->tipo == 'd')
        printf("/");
    printf("\n");
    TVertice *filho = node->filhos;
    while (filho != NULL)
    {
        PrintTree(filho, profundidade + 1);
        filho = filho->prox;
    }
}

TVertice *ProcurarNoDiretorio(TVertice *dir, const char *nome)
{
    if (!strcmp(".", nome))
        return dir;
    if (!strcmp("..", nome))
        return dir->dir_ant;
    if (!strcmp("~", nome))
    {
        while (dir->dir_ant != dir)
            dir = dir->dir_ant;
        return dir;
    }

    TVertice *filho = dir->filhos;
    while (filho != NULL)
    {
        if (!strcmp(filho->id, nome))
            return filho;
        filho = filho->prox;
    }
    return NULL;
}

TVertice *ProcurarNosDiretoriosAbaixo(TVertice *dir, const char *nome)
{
    TVertice *filho = dir->filhos;
    while (filho != NULL)
    {
        if (!strcmp(filho->id, nome))
            return filho;
        TVertice *procuraInterna = ProcurarNosDiretoriosAbaixo(filho, nome);
        if (procuraInterna)
            return procuraInterna;
        filho = filho->prox;
    }
    return NULL;
}

void PrintBytes(int b)
{
    int n = b / 1073741824;
    if (n > 0)
    {
        printf("%dGB", n);
        return;
    }
    n = b / 1048576;
    if (n > 0)
    {
        printf("%dMB", n);
        return;
    }
    n = b / 1024;
    if (n > 0)
    {
        printf("%dKB", n);
        return;
    }
    printf("%dB", b);
}

TVertice *Carregar(TVertice *dir, char *path)
{
    struct dirent *entry;
    DIR *dp;

    dp = opendir(path);
    if (dp == NULL)
    {
        perror("opendir");
        return NULL;
    }

    int pathSize = strlen(path);
    entry = readdir(dp); // lê .
    entry = readdir(dp); // lê ..
    while ((entry = readdir(dp)))
    {
        strcat(path, "/");
        strcat(path, entry->d_name);

        struct stat statbuf;
        if (stat(path, &statbuf) == 0)
        {
            // se é um diretorio
            if (S_ISDIR(statbuf.st_mode))
                AdicionarPasta(dir, entry->d_name);
            // se é arquivo
            else
                AdicionarArquivo(dir, entry->d_name, statbuf.st_size);
        }
        else
        {
            puts("Erro de leitura do arquivo: ");
            puts(path);
            puts("\n");
        }

        // apaga o que foi adicionado ao path
        path[pathSize] = 0;
    }
    closedir(dp);

    // avança por todos as entradas adicionadas
    TVertice *node = dir->filhos;
    while (node != NULL)
    {
        if (node->tipo == 'd')
        {
            strcat(path, "/");
            strcat(path, node->id);
            Carregar(node, path);
            path[pathSize] = 0;
        }
        node = node->prox;
    }

    return dir;
}

void PrintList(TVertice *dir)
{
    TVertice *node = dir->filhos;
    while (node != NULL)
    {
        printf(node->id);
        if (node->tipo == 'd')
        {
            printf("/");
        }
        printf(" --- ");
        PrintBytes(node->size);
        printf("\n");
        node = node->prox;
    }
}

void ImprimirEmProfundidade(TVertice *dir)
{
    printf(dir->id);
    if (dir->tipo == 'a')
    {
        printf(" --- ");
        PrintBytes(dir->size);
    }
    else
        printf("/");
    printf("\n");
    TVertice *filho = dir->filhos;
    while (filho != NULL)
    {
        ImprimirEmProfundidade(filho);
        filho = filho->prox;
    }
}

void ImprimirEmLarguraInternal(TVertice *dir, int nivel, int nivelDestino, int *profMax)
{
    TVertice *filho = dir->filhos;
    while (filho != NULL)
    {
        if (nivel == nivelDestino)
        {
            printf(filho->id);
            if (filho->tipo == 'a')
            {
                printf(" --- ");
                PrintBytes(filho->size);
            }
            else
                printf("/");
            printf("\n");
            if (filho->filhos && nivel + 1 > *profMax)
                *profMax = nivel + 1;
        }
        else if (nivel < nivelDestino)
        {
            ImprimirEmLarguraInternal(filho, nivel + 1, nivelDestino, profMax);
        }
        filho = filho->prox;
    }
}

void ImprimirEmLargura(TVertice *dir)
{
    int profMax = 0;
    int nivel = 0;
    while (nivel <= profMax)
    {
        ImprimirEmLarguraInternal(dir, 0, nivel++, &profMax);
    }
}

void PrintFullPath(TVertice *dir)
{
    char path[_MAX_PATH];
    char temp[_MAX_PATH];
    strcpy(path, dir->id);
    while (dir->dir_ant != dir)
    {
        dir = dir->dir_ant;
        strcpy(temp, path);
        strcpy(path, dir->id);
        strcat(path, "/");
        strcat(path, temp);
    }
    printf(path);
}

int InterpretarComando(TVertice **dir, const char *cmd)
{
    if (cmd[0] == 'x')
        return 1;
    else if (cmd[0] == 'h')
    {
        puts("m - criar diretório");
        puts("c - mudar diretório corrente");
        puts("d - remover um arquivo");
        puts("p – Apresentar a árvore realizando um percurso em profundidade");
        puts("l - Apresentar a árvore realizando um percurso em largura");
        puts("t - Apresentar a árvore estruturada");
        puts("h - Imprimir todos os comandos disponíveis");
        puts("x - sair");
    }
    else if (cmd[0] == 't')
    {
        PrintTree(*dir, 0);
        printf("\n");
    }
    else if (cmd[0] == 'l')
        ImprimirEmLargura(*dir);
    else if (cmd[0] == 'p')
        ImprimirEmProfundidade(*dir);
    else if ((cmd[0] == 'd' || cmd[0] == 'c' || cmd[0] == 'm') && cmd[1] == ' ')
    {
        TVertice *wd = *dir;
        // conta niveis no endereço
        int niveis = 1;
        for (int c = 2; cmd[c] != 0; c++)
            if (cmd[c] == '/')
                niveis++;

        char **nomes = malloc(sizeof(char *) * niveis);
        for (int i = 0; i < niveis; i++)
            nomes[i] = malloc(sizeof(char) * IDNAME);

        int iLetra = 0;
        int iPalavra = 0;
        // para cada letra
        for (int c = 2; cmd[c] != 0; c++)
        {
            if (cmd[c] == '/')
            {
                nomes[iPalavra][iLetra] = 0;
                iLetra = 0;
                iPalavra++;
            }
            else
            {
                nomes[iPalavra][iLetra] = cmd[c];
                iLetra++;
            }
        }
        nomes[iPalavra][iLetra] = 0;

        if (cmd[0] == 'm')
        {
            for (int i = 0; i < niveis; i++)
            {
                TVertice *achado = ProcurarNoDiretorio(*dir, nomes[i]);
                if (!achado)
                {
                    AdicionarPasta(*dir, nomes[i]);
                }
                *dir = achado;
            }
            *dir = wd;
        }
        else if (cmd[0] == 'c')
        {
            for (int i = 0; i < niveis; i++)
            {
                TVertice *achado = ProcurarNoDiretorio(*dir, nomes[i]);
                if (!achado)
                {
                    printf("Diretório não encontrado: %s\n", nomes[i]);
                    *dir = wd;
                    return ERRO_VERTICE_NAO_ENCONTRADO;
                }
                *dir = achado;
            }
        }
        else if (cmd[0] == 'd')
        {
            for (int i = 0; i < niveis; i++)
            {
                TVertice *achado = ProcurarNoDiretorio(*dir, nomes[i]);
                if (!achado)
                {
                    printf("Diretório não encontrado: %s em %s\n", nomes[i], (*dir)->id);
                    return ERRO_VERTICE_NAO_ENCONTRADO;
                }
                *dir = achado;
            }
            RemoverDoDiretorio((*dir)->dir_ant, (*dir)->id);
            *dir = wd;
        }

        for (int i = 0; i < niveis; i++)
            free(nomes[i]);
        free(nomes);
    }

    return 0;
}