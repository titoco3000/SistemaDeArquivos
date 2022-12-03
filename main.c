/*
Sistema de arquivos virtual.
Carrega a estrutura de pastas do endereço especificado, e permite navegar, deletar e criar novas
pastas, sem alterar em disco.
Não tem nenhuma utilidade real, é somente uma representação de como pode ser organizado um
sistema do tipo

Tito Guidotti

11/2022
*/

#include <stdlib.h> //malloc
#include <stdio.h>  //prinf
#include <string.h> //strcpy
#include "SistemaDeArquivos.h"

int main(int argc, char *argv[])
{
    char path[_MAX_PATH];
    if (argc > 1)
    {
        strcpy(path, argv[1]);
    }
    else
    {
        printf("Diretorio a ser carregado: ");
        gets(path);
    }

    // cria o working directory
    TVertice *wd = CriarRaiz("~");
    Carregar(wd, path);

    // o max que um comando pode atingir é identificador + espaço + endereço
    char comando[_MAX_PATH + 2];
    int len = 0;
    while (1)
    {
        PrintFullPath(wd);
        printf(" > ");
        gets(comando);
        if (InterpretarComando(&wd, comando) == 1)
            break;
    }

    // vai para a raiz
    while (wd->dir_ant != wd)
        wd = wd->dir_ant;
    Remover(wd);
}