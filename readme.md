# Comunicação por sockets
O objetivo do projeto é criar um jogo de **caça ao tesouro** para duas maquinas diferentes com o modelo **cliente-servidor** e comunicação baseada em **raw sockets**. 

## Regras do jogo
### Servidor
Controla o **mapa** e os **tesouros**. Na tela, são mostrados a **posição dos tesouros**, **lista dos movimentos realizados pelo usuário** e a **posição atual do usuário**.

### Cliente
Se movimenta pelo **mapa** em busca dos **tesouros**. Na tela, é mostrado um **mapa 8x8**.

### Tesouros
São lidos de arquivos .jpg, .mp4 ou .txt.

## Estrutura do repositório


O repositório está dividido em três pastas, cujas funções são:
* **src**: código-fonte **principal** do programa. 
* **Loopback**: testes feitos pensando em comunicação via porta loopback em uma só máquina.
* **Ethernet**: testes feitos pensando em comunicação via cabo de rede entre dois computadores. 