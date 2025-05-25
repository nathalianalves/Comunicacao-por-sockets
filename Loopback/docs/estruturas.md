# Estruturas utilizadas no desenvolvimento

Aqui, esclarecemos o __propósito de cada uma das estruturas de dados__ utilizadas no desenvolvimento, assim como a relação entre elas. 

No geral, os pontos aqui comentados são mais relevantes para aqueles que querem se __aprofundar ou fazer modificações no código-fonte__. O entedimento dessas informações não é necessário para a execução do jogo. 

## Servidor
Os arquivos que englobam funções diretas do __servidor__ são: __mainServidor.c, servidor.c e servidor.h__. Além disso, as principais estruturas de dados criadas e manipuladas pelo servidor são:

* __Grid__: é como chamamos o mapa do jogo, representado por uma matriz __GRID_LINHAS x GRID_COLUNAS__.
* __Tesouros__: a estrutura utilizada para armazenar a posição de cada um dos tesouros é uma __matriz NUM_TESOUROS x 2__, de forma que __Grid[tesouros[i][0]] [tesouros[i][1]] = posição do tesouro i no mapa__
       