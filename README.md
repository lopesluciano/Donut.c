# Donut.c - O Donut Giratório em ASCII

Este repositório contém o código-fonte e uma análise matemática detalhada do famoso donut.c, um programa clássico que renderiza um donut tridimensional giratório usando apenas caracteres ASCII no terminal.

Criado originalmente por [Andy Sloane](https://www.a1k0n.net/2011/07/20/donut-math.html), este pequeno programa é um exemplo fascinante de computação gráfica, física e otimização, tudo compactado em um único arquivo C. Ele demonstra como conceitos complexos como rotações 3D, projeção de perspectiva e iluminação podem ser implementados de forma elegante para criar uma animação impressionante em um ambiente tão limitado quanto um terminal de texto.

Este README serve como um guia para entender a "mágica" por trás do código.
Como Compilar e Executar

Para compilar o código, você precisará de um compilador C como o GCC. A maioria dos sistemas baseados em Linux e macOS já o possui instalado.

Compile o código:
``` bash
    gcc Donut.c -o donut -lm
```
(A flag -lm é necessária para vincular a biblioteca math.h.)

Execute o programa:
``` bash
    ./donut
```
Pressione Ctrl + C para encerrar a animação.

# Entendendo a Matemática do Donut.c

Este documento detalha a teoria matemática e os conceitos de computação gráfica por trás do clássico código `donut.c`, que renderiza um donut ASCII giratório no terminal.

A animação pode ser dividida em quatro etapas principais:

1.  **Criação do Toro (a forma de Donut)**
2.  **Rotação do objeto em 3D**
3.  **Projeção do objeto 3D em uma tela 2D**
4.  **Iluminação e sombreamento**

---

### 1. A Matemática do Toro

Um toro é uma superfície de revolução gerada pela rotação de um círculo em um espaço tridimensional em torno de um eixo. Podemos descrever qualquer ponto na superfície de um toro usando dois ângulos. No código, esses ângulos são `i` e `j`.

-   **`j`**: Percorre o círculo maior do toro (revolução em torno do eixo principal).
-   **`i`**: Percorre o círculo menor, que forma a "seção transversal" do toro.

A lógica principal para percorrer cada ponto da superfície está encapsulada nestes loops:

```c
// j corresponde ao círculo grande do toro (theta).
for(j = 0; j < 6.28; j += 0.07) {
    // i corresponde ao círculo pequeno e transversal (phi).
    for(i = 0; i < 6.28; i += 0.02) {
        // ... cálculos para cada ponto (i, j) ...
    }
}
```

### 2. Rotação 3D com Matrizes

Para fazer o donut girar, aplicamos matrizes de rotação aos pontos (x, y, z) do toro. O código rotaciona o donut em torno de dois eixos (x e z), controlados pelos ângulos A e B.

A matriz de rotação em torno do eixo x (ângulo A) é:

    | 1    0        0     |
    | 0  cos(A)  -sin(A)  |
    | 0  sin(A)   cos(A)  |

E a matriz de rotação em torno do eixo z (ângulo B) é:

    | cos(B)  -sin(B)   0  |
    | sin(B)   cos(B)   0  |
    |   0        0      1  |

Para otimização, o código não constrói essas matrizes explicitamente. Em vez disso, ele pré-calcula os senos e cossenos dos ângulos A e B e aplica as transformações de forma combinada e direta nos cálculos das coordenadas.

```C

float c = sin(i);
float d = cos(j);
float e = sin(A);
float f = sin(j);
float g = cos(A);
float l = cos(i);
```

### 3. Projeção de Perspectiva em 2D

Após rotacionar os pontos 3D, eles precisam ser projetados em uma tela 2D. Para criar a ilusão de perspectiva (objetos mais distantes parecem menores), dividimos as coordenadas x e y pela coordenada z (a profundidade).

O código otimiza isso calculando D, o inverso da profundidade (1/z). A multiplicação por D é mais rápida que a divisão por z.
```C

// D é o inverso da coordenada z do ponto (1/z). Esta é a chave para a perspectiva.
// O +5 afasta o donut do observador para que não fique colado na "câmera".
float D = 1 / (c * h * e + f * g + 5);
```
As coordenadas finais da tela, x e y, são então calculadas, escalonadas e transladadas para o centro da tela.
```C

// x e y são as coordenadas 2D finais na tela.
int x = 40 + 30 * D * (l * h * m - t * n);
int y = 12 + 15 * D * (l * h * n + t * m);
```
### 4. Iluminação e o Z-Buffer

Para dar volume ao donut, o código simula uma fonte de luz. O brilho de cada ponto na superfície é determinado pelo produto escalar entre o vetor normal da superfície (um vetor que aponta "para fora") e a direção da fonte de luz.

* Se a superfície está virada para a luz, o produto escalar é positivo (ponto iluminado).

* Se a superfície está de costas para a luz, o produto escalar é negativo (ponto escuro).

A variável N no código representa essa intensidade de luz (luminância).
```C

// N é o índice de luminância. É o produto escalar da normal da superfície e o vetor de luz.
int N = 8 * ((f * e - c * d * g) * m - c * d * e - f * g - l * d * n);
```
Com base no valor de N, um caractere ASCII é escolhido de uma string para representar o brilho. Caracteres mais "densos" como @ são usados para áreas brilhantes, e caracteres mais "esparsos" como . para áreas escuras.
```C

// Seleciona o caractere com base no índice de luminância N.
b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0];
```
Por fim, o z-buffer (o array float z[]) garante a oclusão correta (partes da frente escondem as de trás). Ele armazena o valor de D (inverso da profundidade) para cada pixel na tela. Um novo caractere só é desenhado se estiver mais perto do observador (D > z[o]) do que o caractere que já estava lá.
```C

// Verifica se o ponto está dentro da tela E se está mais próximo do que o que já está no z-buffer.
if(22 > y && y > 0 && 80 > x && x > 0 && D > z[o]) {
    z[o] = D; // Atualiza o z-buffer com a nova profundidade, mais próxima.
    b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0]; // Desenha o caractere.
}
float m = cos(B);
float n = sin(B);
```
