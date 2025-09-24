#include <stdio.h>
#include <string.h>
#include <math.h>

int main() {
    // --- Variaveis de Definicao ---
    float A = 0, B = 0;    // A e B sao os angulos de rotacao em torno dos eixos x e z.
    float i, j;            // i e j sao os angulos utilizados para acompanhar a superficie do toroide
    int k;
    float z[1760];         // O z buffer armazena a profundidade de cada caractere na tela
    char b[1760];          // O buffer de caracteres armazena o ASCII de cada posicao

    // --- Inicio da Animacao ---
    printf("\x1b[2J");     // Codigo ANSI que limpa a tela do terminal
    
    // Loop Infinito para desenhar os frames
    for(;;) {
        // --- Preparar novo frama ---
        // Limpar os buffers para o novo frama.
        memset(b, 32, 1760);       // Preencher o buffer de caracteres com espacos (ASCII 32)
        memset(z, 0, 7040);        // Preencher o z-buffer com 0s. 7040 = 1760 * sizeof(float)
  
        // --- Loops de Calculo (A Matematica Principal) ---
        // Os loops aninhados percorrem todos os pontos da superficie do toroide
        // j corresponde ao maior circulo do toroide (theta)
        for(j = 0; j < 6.28; j += 0.07) {
            // i corresponde ao menor circulo (phi). i vai de 0 a 6.28 (2Pi)
            for(i = 0; i < 6.28; i += 0.02) {
                // Pre calcula senos e cossenos para os angulos atuais
                float c = sin(i);
                float d = cos(j);
                float e = sin(A);
                float f = sin(j);
                float g = cos(A);
                float l = cos(i);
                float m = cos(B);
                float n = sin(B);

                // --- Rotacao 3D e matematica de projecao ---
                float h = d + 2; // Parte da equacao do toroide, relacionado a R1 + R2
                
                // D eh o inverso da coordenada z do ponto (1/z). Isso eh a chave para a perspectiva
                // O +5 distancia o donut do observador
                float D = 1 / (c * h * e + f * g + 5); 

                // t eh o termo intermediario para a multiplicacao da matriz de rotacao
                float t = c * h * g - f * e;

                // --- Calcular Coordenadas na Tela ---
                // x e z sao as coordenadas finais da tela
                // A formula combina matrizes de rotacao para angulos A e B, e projecao usando D
                int x = 40 + 30 * D * (l * h * m - t * n);
                int y = 12 + 15 * D * (l * h * n + t * m);

                // --- Calcular indice de Buffer e Iluminancia ---
                // o eh o indice 1D nos buffer 1D (b e z) a partir das coordenadas (x, y)
                int o = x + 80 * y;


                // N eh o indice de luminancia. Ele eh o produto escalar da superficie normal com o vetor luz, escalado por 8
                int N = 8 * ((f * e - c * d * g) * m - c * d * e - f * g - l * d * n);

                // --- Z-Buffering e Desenho ---
                // Checar se o ponto esta dentro dos limites de tele E se esta mais perto do que esta no z-buffer
                if(22 > y && y > 0 && 80 > x && x > 0 && D > z[o]) {
                    z[o] = D; // Atualizar o z-buffer com a nova profundidade
                    // Selecionar o caractere baseado no indice de iluminancia N
                    b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0];
                }
            }
        }

        // --- Renderizar o Frame ---z
        printf("\x1b[H"); // Codigo ANSI para mover o cursor para o canto superior esquerdo
        
        // Percorrer o buffer de caracteres e imprimir na tela 
        for(k = 0; k < 1761; k++) {
            // Um truque para imprimir novas linhas. Se k eh multiplo de 80, imprime nova linha (ASCII 10), caso contrario, imprima o caractere
            putchar(k % 80 ? b[k] : 10);

            // Incremente os angulos de rotacao dentro do loop para o proximo frame
            A += 0.00001;
            B += 0.000005;
        }
    }
    return 0;
}