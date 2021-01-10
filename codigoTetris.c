#include "defPrincipais.h"

// Variáveis de controle de estado do programa
volatile bool refresh = false, redraw = false, end = false;
// Variáveis ativadas pela ativação do botão
volatile bool up = false, left = false, right = false, down = false, start = false;

// Representação em array da matriz de LEDs
int matrizLEDs [11][6] = {{0, 0, 0, 0, 0, 0},
						  {0, 0, 0, 0, 0, 0},
						  {0, 0, 0, 0, 0, 0},
						  {0, 0, 0, 0, 0, 0},
						  {0, 0, 0, 0, 0, 0},
						  {0, 0, 0, 0, 0, 0},
						  {0, 0, 0, 0, 0, 0},
						  {0, 0, 0, 0, 0, 0},
						  {0, 0, 0, 0, 0, 0},
						  {0, 0, 0, 0, 0, 0},
						  {0, 0, 0, 0, 0, 0}};

// Variáveis de controle, para ativação de LEDs
volatile int linha = 0, coluna = 0;
// Variaveis para controle de Dificuldade e Pontuação
volatile int ciclosRefresh = 0, pontuacao = 0;

// Função que checa as Linhas para detectar se alguma será excluída
bool checaLinhas () {
	int j;
	bool retorno = false;

	// Corre as linhas do Array de LEDs
	for (int i = 0; i < 11; i++) {
	    
	    // Checa se a linha está preenchida
		for (j = 0; j < 6; j++) {
			if (matrizLEDs[i][j] == 0) {
				j = 6;
			}
		}
		
		// Caso haja, marca-a e aumenta a pontuação
		if (j == 6) {
			pontuacao += 100;
			for (int k = 0; k < 6; k++) {
				matrizLEDs[i][k] = 2;
			}
			retorno = true;
		}
	}

	return retorno;
}

// Função que ativa a gravidade caso haja uma linha marcada
void checaGravidade () {
    //Corre as linhas
	for (int i = 10; i >= 0; i--) {
	    // Corre as colunas
		for (int j = 0; j < 6; j++) {
		    // Caso o pixel esteja marcado, atua, substintuindo todos, a partir dele, pelo de cima
			if (matrizLEDs[i][j] == 2) {
				for (int k = i; k >= 0; k--) {
					if (k != 0) {
						matrizLEDs[k][j] = matrizLEDs[k-1][j];
					} else {
						matrizLEDs[k][j] = 0;
					}
				}
			}
		}
	}
}

// Checa se há uma peça na primera linha do display
void checaDerrota () {
	for (int i = 0; i < 6; i++) {
		if (matrizLEDs[3][i] == 1) {
			end = true;
			start = true;
		}
	}
}

// Checa se há um bloco caindo em jogo, retorna o resultado booleano
bool checaBloco () {
	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 6; j++) {
			if (matrizLEDs[i][j] == 3) {
				return true;
			}
		}
	}
	
	return false;
}

// Move o bloco (parâmetro d==0 move para a esquerda, d!=0 move para a direita)
void moveBloco (bool d) {
	bool move = true;

	if (d) {
		// Move para a Direita
		
		// Checa se o movimento é possível
		for (int i = 0; i < 11; i++) {
			for (int j = 0; j < 6; j++) {
				if (matrizLEDs[i][j] == 3) {
					if (j == 5 || matrizLEDs[i][j+1] == 1) {
						move = false;
					}
				}
			}
		}

        // Realiza o Movimento
		if (move) {
			for (int i = 0; i < 11; i++) {
				for (int j = 5; j >= 0; j--) {
					if (matrizLEDs[i][j] == 3) {
						matrizLEDs[i][j] = 0;
						matrizLEDs[i][j+1] = 3;
					}
				}
			}
		}
	} else {
		// Move para a Esquerda
		
		// Checa se o movimento é possível
		for (int i = 0; i < 11; i++) {
			for (int j = 0; j < 6; j++) {
				if (matrizLEDs[i][j] == 3) {
					if (j == 0 || matrizLEDs[i][j-1] == 1) {
						move = false;
					}
				}
			}
		}

        // Realiza o Movimento
		if (move) {
			for (int i = 0; i < 11; i++) {
				for (int j = 0; j < 6; j++) {
					if (matrizLEDs[i][j] == 3) {
						matrizLEDs[i][j] = 0;
						matrizLEDs[i][j-1] = 3;
					}
				}
			}
		}
	}
}

// Função que checa se o Bloco pode cair e o faz, caso possível. Retorna se realizou a queda
bool caiBloco () {
	bool cai = true;
    
    // Checa se é possível cair
	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 6; j++) {
			if (matrizLEDs[i][j] == 3) {
				if (i == 10 || matrizLEDs[i+1][j] == 1) {
					cai = false;
				}
			}
		}
	}

    // Realiza a queda, caso seja possível
    // Caso o contrário. Integra o Bloco ao cenário
	if (cai) {
		for (int j = 0; j < 6; j++) {
			for (int i = 10; i >= 0; i--) {
				if (matrizLEDs[i][j] == 3) {
					matrizLEDs[i][j] = 0;
					matrizLEDs[i+1][j] = 3;
				}
			}
		}
	} else {
	    for (int j = 0; j < 6; j++) {
			for (int i = 10; i >= 0; i--) {
			    if (matrizLEDs[i][j] == 3) {
			        matrizLEDs[i][j] = 1;
			    }
			}
	    }
	}

	return cai;
}

// Função para girar o bloco
void giraBloco() {
    // Variáveis utilizadas para descobrir a localização do bloco para girar
	int ultimaLinha = 0, ultimaColuna = 0;

    // Corre a matriz para achar a última Linha
	for (int i = 10; i >= 0; i--) {
		for (int j = 0; j < 6; j++) {
			if (matrizLEDs[i][j] == 3) {
				ultimaLinha = i;
				i = -1;
				j = 6;
			}
		}
	}	

    // Corre a matriz para achar a última Coluna
	for (int j = 5; j >= 0; j--) {
		for (int i = 0; i < 11; i++) {
			if (matrizLEDs[i][j] == 3) {
				ultimaColuna = j;
				i = 11;
				j = -1;
			}
		}
	}

    // Matriz para gerar o bloco novo
	int blocoVirado[3][3] = {{0, 0, 0},
							 {0, 0, 0},
							 {0, 0, 0}};

    // Preenche a matriz com o bloco virado
	for (int k = ultimaLinha; k > ultimaLinha-3; k--) {
		for (int l = ultimaColuna; l > ultimaColuna-3; l--) {
			if (k >= 0 && l >= 0) {
			    if (matrizLEDs[k][l] != 1) {
				    blocoVirado[2-(ultimaColuna-l)][2-(ultimaLinha-k)] = matrizLEDs[k][l];
			    } else {
			        blocoVirado[2-(ultimaColuna-l)][2-(ultimaLinha-k)] = 0;
			    }
			} else {
				blocoVirado[2-(ultimaColuna-l)][2-(ultimaLinha-k)] = 0;
			}
		}
	}

    //Retorna o Bloco Virado à Matriz
	for (int k = 0; k < 3; k++) {
		for (int l = 0; l < 3; l++) {
			if ((ultimaLinha-k) >= 0 && (ultimaColuna-l) >= 0) {
				matrizLEDs[ultimaLinha-k][ultimaColuna-l] = blocoVirado[2-k][2-l];
			}
		}
	}
}

// Função que gera um novo Bloco. Escolhe o tipo de bloco e a orientação pseudoaleatoriamente utilizando o timer
void geraBloco () {
    // Matriz com os blocos possíveis
    int blocos[7][3][3] = {{{3, 3, 0},
						    {3, 0, 0},
						    {3, 0, 0}},
						    
						   {{3, 3, 0},
						    {0, 3, 0},
						    {0, 3, 0}},
						    
						   {{0, 0, 0},
						    {3, 3, 0},
						    {3, 3, 0}},
						    
						   {{3, 0, 0},
						    {3, 0, 0},
						    {3, 0, 0}},
						    
						   {{3, 0, 0},
						    {3, 3, 0},
						    {0, 3, 0}},
						    
						   {{0, 3, 0},
						    {3, 3, 0},
						    {3, 0, 0}},
						    
						   {{3, 0, 0},
						    {3, 3, 0},
						    {3, 0, 0}}};
    
    // Variável bloco será utilizada para Selecionar um bloco dentre os existentes
	int bloco = TCNT0%7;
	
	// Insere o Bloco nas primeiras linhas da matriz
	for (int k = 0; k < 3; k++) {
		for (int l = 0; l < 3; l++) {
		    matrizLEDs[l][k+2] = blocos[bloco][l][k];
		}
	}
	
	// Giros que dará (utilizado para modificar a posição)
	int giros = TCNT0%4;
	
	// Gira o bloco um número pseudoaleatório de vezes
	for (int i = 0; i < giros; i++) {
	    giraBloco();
	}
	
}

// Rotina principal do Jogo
int main(void) {
    // Define o PORTD como saída em HIGH
	DDRD = 255;
	PORTD = 255;
	
	// Define o PORTC como saída em LOW
	DDRC = 255;
	PORTC = 0;
	
	// Define o PORTB como entrada e habilita o Pull-Up
	DDRB = 0;
	changeBit (DDRB, 6);
	PORTB = 255;
	
	
	setBit (PCICR, 0);	// Habilita as Interrupções no PORTB (0 = B, 1 = C e 2 = D)
	setBit (PCMSK0, 0);	// Habilita as Interrupções no PB0
	setBit (PCMSK0, 1);	// Habilita as Interrupções no PB1
	setBit (PCMSK0, 2);	// Habilita as Interrupções no PB2
	setBit (PCMSK0, 3);	// Habilita as Interrupções no PB3
	setBit (PCMSK0, 4);	// Habilita as Interrupções no PB4
	
	// Abaixo configurando o timer 0
	// Testouro*clock = prescaler*(TOP+1)
	OCR0A = 99;			// Setando o top do estouro = 99
	setBit (TCCR0B, 3);	// Habilitando o Modo Comparação (para o modo estouro bastaria deixar todos em 0)
	TCCR0B |= 2;		// Prescaler = 64 (0 = pausa, 1 = 1, 2 = 8, 3 = 64, 4 = 256 e 5 = 1024)
	setBit (TIMSK0, 1);	// Habilita Interrupção A do Timer 0
	
	// Abaixo configurando o timer 1
	// Testouro*clock = prescaler*(TOP+1)
	OCR1A = 15624;		// Setando o top do estouro = 99
	setBit (TCCR1B, 3);	// Habilitando o Modo Comparação (para o modo estouro bastaria deixar todos em 0)
	TCCR1B |= 4;		// Prescaler = 256 (0 = pausa, 1 = 1, 2 = 8, 3 = 64, 4 = 256 e 5 = 1024)
	setBit (TIMSK1, 1);	// Habilita Interrupção A do Timer 1
	
	sei();              // Habilita as Interrupções
	
    // Variáveis de Jogo
	bool blocoCaindo = false, dificuldade = 0;

    // Loop Principal
    while(1) {
        // Atualização lógica do Jogo
		if (refresh) {
		    if (blocoCaindo) {
				dificuldade = (pontuacao-(pontuacao%20000))/20000;
				if (dificuldade > 5) {
					pontuacao += (dificuldade - 5);
					dificuldade = 5;
				}
				if (ciclosRefresh%(6-dificuldade) == 0) {
					if (end) {
						for (int i = 0; i < 11; i++) {
							for (int j = 0; j < 6; j++) {
								matrizLEDs [i][j] = 0;
							}
						}
						pontuacao = 0;
						end = false;
					}

					if (checaBloco()) {
						if (up) {
							giraBloco();
							up = false;
						} else if (left) {
							moveBloco (0);
							left = false;
						} else if (right) {
							moveBloco (1);
							right = false;
						}
						
						if (!caiBloco()) {
							blocoCaindo = false;
						}
					} else {
						geraBloco();
					}

					pontuacao++;
				}
			} else {
				if (checaLinhas()) {
					checaGravidade();
				} else {
					checaDerrota ();
					blocoCaindo = true;
				}
			}

			refresh = false;
		}
		
		//Atualização do Display
        if (redraw) {
        	PORTC = 0;
        	if (matrizLEDs[linha+3][coluna] == 1 || matrizLEDs[linha+3][coluna] == 3) {
        		setBit (PORTC, coluna);
        	}
			redraw = false;
        }
    }
}

// Mudança de estado nos botões gerará interrupção
ISR (PCINT0_vect) {
	if (!testBit(PINB, 0)) {
		up = true;
	} else if (!testBit(PINB, 1)) {
		left = true;
	}else if (!testBit(PINB, 2)) {
		right = true;
	} else if (!testBit(PINB, 3)) {
		down = true;
	} else if (!testBit(PINB, 4)) {
		if (start) {
			start = false;
		} else {
			start = true;
		}
	}
	changeBit (PORTB, 6);
}

// Interrupção do Timer 0, utilizada para ativar o display
ISR (TIMER0_COMPA_vect) {	
	coluna++;

	if (coluna == 6) {
		coluna = 0;
		setBit (PORTD, linha);
		linha++;
		if (linha == 8) {
			linha = 0;
		}
		clearBit (PORTD, linha);
	}

	redraw = true;
}

// Interrupção do Timer 1, configura a passagem do tempo
ISR (TIMER1_COMPA_vect) {
	if (!start) {
		ciclosRefresh++;
		
		refresh = true;
	}
}