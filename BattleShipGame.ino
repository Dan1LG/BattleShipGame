#include "BattleShip.hpp"
#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>  //vfinal
#include <Adafruit_NeoPixel.h>
// #include <Fonts/FreeMonoBold9pt7b.h>
#include <DFRobotDFPlayerMini.h>

// Definición de pines y configuración de la matriz LED
#define PIN 8             // Pin al que está conectada la matriz LED
#define numRows 16        // Número de filas en la matriz
#define numCols 16        // Número de columnas en la matriz
#define NUM_PIXELES 1024  // Total de LEDs
#define BRILLO 4          // Brillo de la matriz (0-255)

#define PANTALLA1 0
#define PANTALLA2 256
#define PANTALLA3 512
#define PANTALLA4 768
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;  // Tiempo de debounce de 50ms
uint32_t matPuntero = PANTALLA1;
uint32_t matBarco = PANTALLA1;
unsigned long currentTime;

// Maquina de estados
#define INICIO 0
#define COLOCACION 1
#define TURNO_J1 2
#define TURNO_J2 3
#define FIN_PARTIDA 4

unsigned char estado;

uint16_t casillaPuntero[2][256];
uint8_t matrizPuntero[2][16][16];

uint16_t contCas[2] = { 0, 0 };
bool hit[2][256];
uint8_t barcosDerribados[2] = { 0, 0 };

// Configuración de la matriz LED
//Adafruit_NeoPixel matriz(NUM_PIXELES, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoMatrix matriz(16, 64, PIN, NEO_MATRIX_TOP + NEO_MATRIX_RIGHT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG, NEO_GRB + NEO_KHZ800);

const uint32_t ROJO = matriz.Color(255, 0, 0);
const uint32_t VERDE = matriz.Color(0, 255, 0);
const uint32_t AZUL = matriz.Color(0, 0, 255);
const uint32_t AMARILLO = matriz.Color(255, 255, 0);
const uint32_t NARANJA = matriz.Color(255, 128, 0);
const uint32_t ROSA = matriz.Color(255, 51, 153);
const uint32_t MORADO = matriz.Color(178, 102, 255);
const uint32_t CIAN = matriz.Color(0, 255, 255);
const uint32_t BLANCO = matriz.Color(255, 255, 255);
const uint32_t MARRON = matriz.Color(161, 130, 98);

int x, y, altura, ancho;
int r = 0;
int g = 0;
int b = 0;

// Definición de botones del controlador NES
const uint8_t BOTON_A = 0;
const uint8_t BOTON_B = 1;
const uint8_t BOTON_SELECCION = 2;
const uint8_t BOTON_INICIO = 3;
const uint8_t BOTON_ARRIBA = 4;
const uint8_t BOTON_ABAJO = 5;
const uint8_t BOTON_IZQUIERDA = 6;
const uint8_t BOTON_DERECHA = 7;

// Configuración de pines del controlador NES
uint8_t pinDatosNES[2] = { 4, 7 };  // Pin para leer datos del controlador  //blanco
uint8_t pinRelojNES[2] = { 2, 5 };  // Pin de reloj del controlador         //amarillo
uint8_t pinLatchNES[2] = { 3, 6 };  // Pin de latch del controlador         //verde

uint8_t puntero[2] = { 7, 7 };  // puntero[0] --> Eje x, puntero[1] --> Eje y
bool estaFijado = false;
int pixelIndex;

int x_atacar[2] = { 0, 0 };  //1 por jugador
int y_atacar[2] = { 0, 0 };

//Concurrencia
bool ocupado[2] = { false, false };
uint8_t turno;

byte botonesNES[2];
uint8_t seleccion[2] = { 0, 0 };  //puntero que representa la opción sobre la que apunta (0=play, 1=settings)
bool listo[2] = { false, false };
bool options[2] = { false, false };

uint8_t x1 = ANCHO_MATRIZ;
uint8_t pass = 0;

uint8_t numRotarCrucero[2] = {0,0}, numRotarPA[2] = {0,0};  // veces rotados ambos barcos

uint8_t pantalla[2] = { 0, 0 };
const uint32_t PANTALLA[2] = { PANTALLA1, PANTALLA4 };

//Imagen almacenada en memoria flash
const uint8_t pc2[16][16][3] PROGMEM =
#include "TextoInicio.h"  //Battleship
  ;
// const uint8_t pc[16][16][3] PROGMEM =
// #include "PlayOptions.h"  // Nombre del archivo de imagen. [Play y Options]
//   ;
// const uint8_t pc3[16][16][3] PROGMEM =
// #include "barco_menu.h"  // Nombre del archivo de imagen. [Play y Options]
//   ;
const bool MENU[16][16] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  //Play y Options
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 },
  { 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0 },
  { 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0 },
  { 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
  { 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0 },
  { 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

// const bool TITULO[16][16] = {
//   { 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1 },  //BATTLESHIP
//   { 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0 },
//   { 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1 },
//   { 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0 },
//   { 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1 },
//   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
//   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
//   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
//   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
//   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
//   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
//   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
//   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
//   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
//   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
//   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
// };

const uint8_t OPTIONS[16][16] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  //A para brillo y B para volumen
  { 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  //0(negro), 1(blanco), 2(amarillo), 3(azul), 4(verde, 5(morado))
  { 0, 0, 2, 0, 2, 0, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0 },
  { 0, 2, 2, 2, 0, 2, 2, 2, 0, 0, 0, 1, 1, 1, 0, 0 },
  { 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0 },
  { 0, 0, 2, 0, 2, 0, 2, 0, 0, 0, 0, 1, 0, 1, 0, 0 },
  { 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 3, 0, 0, 3, 0, 0, 0, 0, 1, 1, 0, 0, 0 },
  { 0, 0, 3, 3, 0, 0, 0, 3, 0, 0, 0, 1, 0, 1, 0, 0 },
  { 0, 3, 3, 3, 0, 3, 0, 3, 0, 0, 0, 1, 1, 0, 0, 0 },
  { 0, 3, 3, 3, 0, 3, 0, 3, 0, 0, 0, 1, 0, 1, 0, 0 },
  { 0, 0, 3, 3, 0, 0, 0, 3, 0, 0, 0, 1, 1, 0, 0, 0 },
  { 0, 0, 0, 3, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

const uint8_t OPTIONS2[16][16] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  //START para volver a la pantalla de inicio
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },  //0(negro), 1(blanco), 2(amarillo), 3(azul), 4(verde, 5(morado))
  { 0, 5, 5, 4, 4, 4, 0, 5, 0, 0, 4, 4, 0, 5, 5, 5 },
  { 5, 0, 0, 0, 4, 0, 5, 0, 5, 4, 0, 0, 4, 0, 5, 0 },
  { 0, 5, 0, 0, 4, 0, 5, 5, 5, 4, 4, 4, 4, 0, 5, 0 },
  { 0, 0, 5, 0, 4, 0, 5, 0, 5, 4, 0, 4, 0, 0, 5, 0 },
  { 5, 5, 0, 0, 4, 0, 5, 0, 5, 4, 0, 0, 4, 0, 5, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
  { 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
  { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

//Audio y brillo
DFRobotDFPlayerMini audio;
uint8_t brillo;
uint8_t volumen;

#define MAX_BRILLO 50
#define MAX_VOL 30

//Fin Audio
bool tocado[2] = { false, false };
unsigned i = 1, j = 1;
bool Parpadeo = true;

// Configuración del juego
const uint8_t MAX_BARCOS = 5;  // Número máximo de barcos en el juego
Barco barcos[2][MAX_BARCOS] = { { Barco(0, pantalla[0], 4), Barco(1, pantalla[0], 5), Barco(2, pantalla[0], 6), Barco(3, pantalla[0], 7), Barco(4, pantalla[0], 8) },
                                { Barco(0, pantalla[1], 4), Barco(1, pantalla[1], 5), Barco(2, pantalla[1], 6), Barco(3, pantalla[1], 7), Barco(4, pantalla[1], 8) } };

// const uint8_t MAX_BARCOS = 3;  // Número máximo de barcos en el juego
// Barco barcos[2][MAX_BARCOS] = { { Barco(4, pantalla[0], 4) , Barco(3, pantalla[0], 5), Barco(2, pantalla[0], 6)},
//                                 { Barco(4, pantalla[1], 4) , Barco(3, pantalla[1], 5),Barco(2, pantalla[1], 6)} };
uint8_t barcoActual[2] = { 0, 0 };  // Índice del barco que el jugador está controlando
bool haAtacado[2] = { false, false };

// Configuración de parpadeo
unsigned long ultimoTiempoParpadeo = 0;     // Guarda el tiempo del último parpadeo
bool mostrarBarcoNoFijo = true;             // Determina si se muestra o no el barco no fijado
const unsigned long tiempoEncendido = 400;  // Tiempo que el barco está visible (en milisegundos)
const unsigned long tiempoApagado = 100;    // Tiempo que el barco está invisible


const String text[2] = { "WAITING FOR P2", "WAITING FOR P1" };
const String text1 = "PLAY";     // Primer texto
const String text2 = "OPTIONS";  // Segundo texto
const String winner = "WINNER! :)";
const String looser = "LOOSER! :(";
// const char* text3 = "WAITING FOR P2";
String text3 = "WAITING  F O R  P 2";
// const char* text4 = "WAITING FOR P1";
int posText[2] = { matriz.width(), matriz.width() };   // Posición inicial para el primer texto (play)
int posText2[2] = { matriz.width(), matriz.width() };  //pal options
int posText3[2] = { matriz.width(), matriz.width() };  //pal waiting
int posText4[2] = { matriz.width(), matriz.width() };  //pal winner
int posText5[2] = { matriz.width(), matriz.width() };  //pal looser

//int pass = 0
// Configuración de parpadeo
unsigned long ultimoTiempoParpadeoMenu = 0;     // Guarda el tiempo del último parpadeo
bool mostrarBarcoNoFijoMenu = true;             // Determina si se muestra o no el barco no fijado
const unsigned long tiempoEncendidoMenu = 100;  // Tiempo que el barco está visible (en milisegundos)
const unsigned long tiempoApagadoMenu = 100;    // Tiempo que el barco está invisible


// Función para limpiar la matriz LED (apaga todos los LEDs)
void limpiarMatriz() {
  for (int i = 0; i < NUM_PIXELES; i++) {
    matriz.setPixelColor(i, 0);  // Establece el color negro en cada LED
  }
}

bool fijados[2][MAX_BARCOS];

// Configuración inicial del programa
void setup() {
  matriz.begin();  // Inicializa la matriz LED
  matriz.clear();
  matriz.setTextWrap(false);
  matriz.setFont(0);
  matriz.setTextColor(AZUL);
  matriz.setTextSize(1);
  matriz.setBrightness(BRILLO);  // Configura el brillo de la matriz
  matriz.show();                 // Aplica los cambios en la matriz LED
  Serial.begin(19200);
  Serial1.begin(9600);
  for (uint8_t i = 0; i < MAX_BARCOS; ++i) {
    fijados[0][i] = false;
    fijados[1][i] = false;
  }

  // matriz.setPixelColor(600,ROJO);
  x = 0;
  y = 7;
  ancho = 8;
  altura = 9;

  if (!audio.begin(Serial1)) {  // Usar Serial1 para comunicar con el DFPlayer         //se comunica con el pin R0
    //Serial.println(F("Error al iniciar el DFPlayer. Verifica la conexión o el pendrive."));
    while (true)
      ;
  }
  audio.outputDevice(DFPLAYER_DEVICE_U_DISK);  // Configura para usar el pendrive

  pinMode(pinDatosNES[0], INPUT);   // Configura el pin de datos del controlador como entrada
  pinMode(pinRelojNES[0], OUTPUT);  // Configura el pin de reloj del controlador como salida
  pinMode(pinLatchNES[0], OUTPUT);  // Configura el pin de latch del controlador como salida
  pinMode(pinDatosNES[1], INPUT);   // Configura el pin de datos del controlador como entrada
  pinMode(pinRelojNES[1], OUTPUT);  // Configura el pin de reloj del controlador como salida
  pinMode(pinLatchNES[1], OUTPUT);  // Configura el pin de latch del controlador como salida
  estado = INICIO;
  audio.volume(1);
  // Serial.begin(9600);
}

// Lee los botones presionados del controlador NES
byte leerNES(uint8_t jugador) {
  // byte tempData = 0xFF;
  digitalWrite(pinLatchNES[jugador], HIGH);  // Activa el latch
  delayMicroseconds(12);                     // Espera un momento para la lectura
  digitalWrite(pinLatchNES[jugador], LOW);   // Desactiva el latch
  byte datos = 0;
  for (uint8_t i = 0; i < 8; i++) {
    datos |= (digitalRead(pinDatosNES[jugador]) << i);  // Lee cada botón y construye el byte
    digitalWrite(pinRelojNES[jugador], HIGH);           // Genera un pulso de reloj
    delayMicroseconds(6);
    digitalWrite(pinRelojNES[jugador], LOW);
  }
  return ~datos;  // Invierte los bits para obtener los valores correctos
}

void printWithSpacing(const String text, uint8_t spacing, uint8_t startX, uint8_t startY) {
  uint8_t cursorX = startX;  // Posición inicial en X
  matriz.setCursor(cursorX, startY);

  for (size_t i = 0; i < text.length(); i++) {
    matriz.setCursor(cursorX, startY);  // Ajusta el cursor antes de imprimir
    matriz.print(text[i]);              // Imprime el carácter actual
    cursorX += spacing;                 // Incrementa la posición del cursor según el espaciado
  }
  //matriz.show();  // Actualiza la matriz
}


void drawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint32_t color) {

  // Dibujar las líneas del rectángulo (solo el borde)
  for (uint8_t i = 0; i < width; i++) {
    matriz.drawPixel(x + i, y, color);               // Límite superior
    matriz.drawPixel(x + i, y + height - 1, color);  // Límite inferior
  }
  for (uint8_t i = 0; i < height; i++) {
    matriz.drawPixel(x, y + i, color);              // Límite izquierdo
    matriz.drawPixel(x + width - 1, y + i, color);  // Límite derecho
  }
}

void menu(uint8_t jugador) {

  if (!options[jugador]) {
    //TEXTO BATTLESHIP (TITULO)
    for (int y = 0; y < numRows; y++) {
      for (int x = 0; x < numCols; x++) {
        // Leer colores desde la imagen
        r = int(pgm_read_byte_near(&pc2[y][x][0]));
        g = int(pgm_read_byte_near(&pc2[y][x][1]));
        b = int(pgm_read_byte_near(&pc2[y][x][2]));
        if (((r * i) % 255 < 120) && ((g * i) % 255 < 110) && ((b * i) % 255 < 120)) {
          matriz.drawPixel(x, y + 16 + 16 * jugador, matriz.Color(r, g, b));
        } else {

          matriz.drawPixel(x, y + 16 + 16 * jugador, matriz.Color((r * i) % 255, (g * i) % 255, (b * i) % 255));
        }
      }
    }
    j++;
    if (j == 20) {
      j = 1;
      i++;
    }
    unsigned long tiempoActualMenu = millis();
    unsigned long intervaloParpadeoMenu = mostrarBarcoNoFijoMenu ? tiempoEncendidoMenu : tiempoApagadoMenu;
    // Si mostrarBarcoNoFijo es true, entonces el intervalo será tiempoEncendido, que representa el tiempo durante el cual el barco será visible.
    //Si mostrarBarcoNoFijo es false, el intervalo será tiempoApagado, que representa el tiempo durante el cual el barco será invisible.

    if (tiempoActualMenu - ultimoTiempoParpadeoMenu >= intervaloParpadeoMenu) {
      ultimoTiempoParpadeoMenu = tiempoActualMenu;
      mostrarBarcoNoFijoMenu = !mostrarBarcoNoFijoMenu;  // Alterna entre visible e invisible
    }
    // for (uint8_t i = 0; i < 16; ++i) {
    //   for (uint8_t j = 0; j < 16; ++j) {
    //     if (TITULO[j][i])  //ATENCIÓN, SALE ROTADA, POR ESO SE PONE AL REVÉS LAS COORDENADAS
    //     {
    //       matriz.drawPixel(i, j + 16 + 16 * jugador, ROJO);
    //     }
    //   }
    // }
    // for (int y = 0; y < numRows; y++) {
    //   for (int x = 0; x < numCols; x++) {
    //     // Leer colores desde la imagen
    //     r = int(pgm_read_byte_near(&pc[y][x][0]));
    //     g = int(pgm_read_byte_near(&pc[y][x][1]));
    //     b = int(pgm_read_byte_near(&pc[y][x][2]));

    //     matriz.drawPixel(x, y, matriz.Color(r, g, b));      //Pantalla 1
    //     matriz.drawPixel(x+16*3,y+16*3, matriz.Color(r, g, b)); //Pantalla 4
    //   }
    // }
    for (uint8_t i = 0; i < 16; ++i) {
      for (uint8_t j = 0; j < 16; ++j) {
        if (MENU[j][i])  //ATENCIÓN, SALE ROTADA, POR ESO SE PONE AL REVÉS LAS COORDENADAS
        {
          matriz.drawPixel(i, j + 48 * jugador, ROJO);
        }
      }
    }

    if (botonesNES[jugador] & (1 << BOTON_A))  //PLAY
    {
      // audio.play(2); // Reproduce el segundo archivo MP3 (pop)
      delay(150);
      posText2[jugador] = matriz.width();
      posText[jugador] = matriz.width();
      posText3[jugador] = matriz.width();
      if (seleccion[jugador] == 0) {

        listo[jugador] = true;
      } else {
        options[jugador] = true;
      }
      //ESPERAR A QUE EL OTRO ESTÉ LISTO TAMBIÉN (MOSTRAR UN WAITING...)
    }
    if (botonesNES[jugador] & (1 << BOTON_IZQUIERDA))  //play
    {
      // audio.play(3); // Reproduce el segundo archivo MP3 (pop) //INICIO(1) VICTORY(2) POP(3) BOOM(4)
      posText[jugador] = matriz.width();

      seleccion[jugador] = 0;
    }
    if (botonesNES[jugador] & (1 << BOTON_DERECHA))  //AJUSTES
    {
      //audio.play(3); // Reproduce el segundo archivo MP3 (pop) //
      listo[jugador] = false;
      Parpadeo = true;

      posText2[jugador] = matriz.width();
      seleccion[jugador] = 1;
    }

    if (seleccion[jugador] == 0 && !listo[jugador]) {
      x = 0;
      y = 7 + 48 * jugador;
      ancho = 8;
      altura = 9;
      // Dibuja el rectángulo en la posición (8,15) con tamaño 8x9
      printWithSpacing(text1, 7, posText[jugador], 0 + 48 * jugador);      // Texto, espaciado, posición inicial (X, Y)
      printWithSpacing(text1, 7, posText[jugador] + 1, 0 + 48 * jugador);  // Texto, espaciado, posición inicial (X, Y)


      drawRectangle(x, y, ancho, altura, VERDE);  // Dibuja el rectángulo en la posición (x,y) con tamaño anchoxaltura
                                                  //matriz.show();  // Actualizar la matriz

      int textWidth = strlen(text1.c_str()) * 7;  // 6 píxeles por cada carácter
      if (posText[jugador] < -textWidth) {        // Reseteamos la posición cuando el texto se haya movido fuera de la pantalla
        posText[jugador] = matriz.width();
      }
      // Mover el texto hacia la izquierda
      posText[jugador]--;
      //delay(20);
    }
    if (seleccion[jugador] == 1 && !listo[jugador]) {
      x = 7;
      y = 7 + 48 * jugador;
      ancho = 9;
      altura = 9;  // Dibuja el rectángulo en la posición (8,15) con tamaño 8x9


      drawRectangle(x, y, ancho, altura, VERDE);  // Dibuja el rectángulo en la posición (x,y) con tamaño anchoxaltura
                                                  //matriz.show();  // Actualizar la matriz

      printWithSpacing(text2, 7, posText2[jugador], 0 + 48 * jugador);      // Texto, espaciado, posición inicial (X, Y)  //OPTIONS
      printWithSpacing(text2, 7, posText2[jugador] + 1, 0 + 48 * jugador);  // Texto, espaciado, posición inicial (X, Y)

      int textWidth2 = strlen(text2.c_str()) * 7;  // 6 píxeles por cada carácter
      if (posText2[jugador] < -textWidth2) {       // Reseteamos la posición cuando el texto se haya movido fuera de la pantalla
        posText2[jugador] = 16;
      }

      // Mover el texto hacia la izquierda
      posText2[jugador]--;
      //delay(15);
    }
    if (seleccion[jugador] == 0 && listo[jugador]) {
      x = 0;
      y = 7 + 48 * jugador;
      ancho = 8;
      altura = 9;                                                                   // Dibuja el rectángulo en la posición (8,15) con tamaño 8x9
      drawRectangle(x, y, ancho, altura, AMARILLO);                                 // Dibuja el rectángulo en la posición (x,y) con tamaño anchoxaltura
      printWithSpacing(text[jugador], 7, posText3[jugador], 0 + 48 * jugador);      // Texto, espaciado, posición inicial (X, Y)  WAITING
      printWithSpacing(text[jugador], 7, posText3[jugador] + 1, 0 + 48 * jugador);  // Texto, espaciado, posición inicial (X, Y)
      Parpadeo = false;
      int textWidth = strlen(text[jugador].c_str()) * 7;  // 6 píxeles por cada carácter
      if (posText3[jugador] < -textWidth) {               // Reseteamos la posición cuando el texto se haya movido fuera de la pantalla
        posText3[jugador] = matriz.width();
      }
      // Mover el texto hacia la izquierda
      posText3[jugador]--;
    }


    // if (seleccion[jugador] == 1 && listo[jugador]) {
    //   options[jugador] = true;
    // }


  } else {  //ESTA DENTRO DE OPTIONS

    for (uint8_t i = 0; i < 16; ++i) {
      for (uint8_t j = 0; j < 16; ++j) {
        //0(negro), 1(blanco), 2(amarillo), 3(azul), 4(verde, 5(morado))
        if (OPTIONS[j][i] == 1)  //ATENCIÓN, SALE ROTADA, POR ESO SE PONE AL REVÉS LAS COORDENADAS
        {
          matriz.drawPixel(i, j + 16 + 16 * jugador, BLANCO);
        } else if (OPTIONS[j][i] == 2)  //ATENCIÓN, SALE ROTADA, POR ESO SE PONE AL REVÉS LAS COORDENADAS
        {
          matriz.drawPixel(i, j + 16 + 16 * jugador, AMARILLO);
        } else if (OPTIONS[j][i] == 3)  //ATENCIÓN, SALE ROTADA, POR ESO SE PONE AL REVÉS LAS COORDENADAS
        {
          matriz.drawPixel(i, j + 16 + 16 * jugador, AZUL);
        }
      }
    }

    for (uint8_t k = 0; k < 16; ++k) {
      for (uint8_t l = 0; l < 16; ++l) {
        if (OPTIONS2[l][k] == 1)  //ATENCIÓN, SALE ROTADA, POR ESO SE PONE AL REVÉS LAS COORDENADAS
        {
          matriz.drawPixel(k, l + 48 * jugador, BLANCO);
        } else if (OPTIONS2[l][k] == 4)  //ATENCIÓN, SALE ROTADA, POR ESO SE PONE AL REVÉS LAS COORDENADAS
        {
          matriz.drawPixel(k, l + 48 * jugador, VERDE);
        } else if (OPTIONS2[l][k] == 5)  //ATENCIÓN, SALE ROTADA, POR ESO SE PONE AL REVÉS LAS COORDENADAS
        {
          matriz.drawPixel(k, l + 48 * jugador, MORADO);
        }
      }
    }

    if (botonesNES[jugador] & (1 << BOTON_A)) {
      //audio.play(3); // Reproduce el segundo archivo MP3 (pop)//INICIO(1) VICTORY(2) POP(3) BOOM(4) AGUA (5)
      delay(150);
      ocupado[jugador] = true;
      turno = 0;
      while (ocupado[jugador] && (turno == 1))
        ;
      brillo += 10;
      brillo %= MAX_BRILLO;
      if (brillo == 0)
        brillo = 10;
      ocupado[jugador] = false;
      matriz.setBrightness(brillo);
    }

    if (botonesNES[jugador] & (1 << BOTON_B)) {
      audio.play(3);  // Reproduce el segundo archivo MP3 (pop)
      delay(150);
      ocupado[jugador] = true;
      turno = 1;
      while (ocupado[jugador] && (turno == 0))
        ;
      volumen += 5;
      volumen %= MAX_VOL;
      ocupado[jugador] = false;
      audio.volume(volumen);
    }

    if (botonesNES[jugador] & (1 << BOTON_INICIO)) {
      //audio.play(3); // Reproduce el segundo archivo MP3 (pop)
      options[jugador] = false;
    }
  }
}


bool verificarSolapamiento(Barco& barco, uint8_t jugador) {
  for (uint8_t i = 0; i < MAX_BARCOS; i++) {
    if (i != barcoActual[jugador] && (barcos[jugador][i]).get_estaFijo()) {
      if (barcos[jugador][barcoActual[jugador]].verificarSolapamiento(barcos[jugador][i]))
        return true;
    }
  }
  return false;  // No hay solapamientos
}

bool fin[2] = { false, false };
void colocar_barcos(uint8_t jugador) {
  if (!fin[jugador]) {  //! todos_fijados(fijados) barcoActual[jugador] <= MAX_BARCOS-1
    if (botonesNES[jugador] & (1 << BOTON_IZQUIERDA))
      barcos[jugador][barcoActual[jugador]].mover(-1, 0, pantalla[jugador]);  // Mueve a la derecha
                                                                              // lastDebounceTime = currentTime;                                           // Actualiza el tiempo de debounce
    if (botonesNES[jugador] & (1 << BOTON_DERECHA))
      barcos[jugador][barcoActual[jugador]].mover(1, 0, pantalla[jugador]);  // Mueve a la izquierda
                                                                             // lastDebounceTime = currentTime;                                          // Actualiza el tiempo de debounce
    if (botonesNES[jugador] & (1 << BOTON_ARRIBA))
      barcos[jugador][barcoActual[jugador]].mover(0, -1, pantalla[jugador]);  // Mueve hacia arriba

    // lastDebounceTime = currentTime;                                           // Actualiza el tiempo de debounce
    if (botonesNES[jugador] & (1 << BOTON_ABAJO))
      barcos[jugador][barcoActual[jugador]].mover(0, 1, pantalla[jugador]);  // Mueve hacia abajo
    //lastDebounceTime = currentTime;                                          // Actualiza el tiempo de debounce

    // Cambia la forma del barco con el botón de selección
    if (botonesNES[jugador] & (1 << BOTON_SELECCION)) {
      delay(150);
      //lastDebounceTime = currentTime;  // Actualiza el tiempo de debounce
      //Serial.println(barcos[jugador][barcoActual[jugador]].get_formaActual());
      // if (!fijados[jugador][(barcos[jugador][barcoActual[jugador]].get_formaActual() + 1) % 5]) {
      //   barcos[jugador][barcoActual[jugador]].set_formaActual((barcos[jugador][barcoActual[jugador]].get_formaActual() + 1) % 5);
      //   barcos[jugador][barcoActual[jugador]].cambiarForma(pantalla[jugador]);
      // } else {
      //   while (fijados[jugador][(barcos[jugador][barcoActual[jugador]].get_formaActual() + 1) % 5]) {
      //     barcos[jugador][barcoActual[jugador]].set_formaActual((barcos[jugador][barcoActual[jugador]].get_formaActual() + 2) % 5);
      //   }
      //   //   barcos[barcoActual[0]].set_formaActual((barcos[barcoActual[0]].get_formaActual()+2)%5);
      //   barcos[jugador][barcoActual[jugador]].cambiarForma(pantalla[jugador]);
      // }
      barcos[jugador][barcoActual[jugador]].set_formaActual((barcos[jugador][barcoActual[jugador]].get_formaActual() + 1) % 5);
      barcos[jugador][barcoActual[jugador]].cambiarForma(pantalla[jugador]);
    }

    // Rota el barco con el botón A
    if (botonesNES[jugador] & (1 << BOTON_B)) {
      delay(150);
      //lastDebounceTime = currentTime;  // Actualiza el tiempo de debounce
      if (barcos[jugador][barcoActual[jugador]].get_formaActual() == 0) {
        barcos[jugador][barcoActual[jugador]].rotarCrucero(pantalla[jugador], numRotarCrucero[jugador]);
      }
      // else if(barcos[barcoActual[0]].get_formaActual() == 1)
      // {
      //   barcos[barcoActual[0]].rotarLinea(pantalla);
      // }
      else if (barcos[jugador][barcoActual[jugador]].get_formaActual() == 2) {
        barcos[jugador][barcoActual[jugador]].rotarPortaAviones(pantalla[jugador], numRotarPA[jugador]);
      } else {
        barcos[jugador][barcoActual[jugador]].rotar(pantalla[jugador]);
      }
    }

    // Fija el barco si es posible con el botón B
    bool solapa = (!verificarSolapamiento(barcos[jugador][barcoActual[jugador]], jugador) && barcos[jugador][barcoActual[jugador]].puedeFijarse());
    if (botonesNES[jugador] & (1 << BOTON_A) && solapa) {
      delay(150);
      //lastDebounceTime = currentTime;                             // Actualiza el tiempo de debounce
      if (!barcos[jugador][barcoActual[jugador]].get_estaFijo())  //&& !todos_fijados(fijados)
      {
        fijados[jugador][barcos[jugador][barcoActual[jugador]].get_formaActual()] = true;
        barcos[jugador][barcoActual[jugador]].fijar(barcos[jugador][barcoActual[jugador]].get_color());  // Marca el barco como fijo
        barcos[jugador][barcoActual[jugador]].dibujar(matriz, PANTALLA[jugador]);                        // Lo dibuja en azul                                            // Cambia al siguiente barco
        // fijados[barcoActual[0]] = true;
        if (barcoActual[jugador] == 4) {
          Serial.println(barcos[jugador][barcoActual[jugador]].get_estaFijo());
        }
        if (barcoActual[jugador] < MAX_BARCOS - 1) {

          barcoActual[jugador]++;
        } else {

          fin[jugador] = true;
        }
      }
      // if(todos_fijados(fijados))
      // barcoActual[0] = MAX_BARCOS+2; //para asegurar que termine ahí
    }

    // Dibuja el barco actual en verde si no está fijado
    // if (fijados[jugador][barcos[jugador][barcoActual[jugador]].get_formaActual()])
    // {
    //   barcos[jugador][barcoActual[jugador]].set_formaActual((barcos[jugador][barcoActual[jugador]].get_formaActual() + 1) % 5);
    //   barcos[jugador][barcoActual[jugador]].cambiarForma(pantalla[jugador]);

    // }
    // if (mostrarBarcoNoFijo&&!barcos[jugador][barcoActual[jugador]].get_estaFijo()) {
    //   barcos[jugador][barcoActual[jugador]].dibujar(matriz, PANTALLA[jugador]);  // Verde para barcos no fijados
    // }
    if (!fin[jugador]) {
      if (mostrarBarcoNoFijo && fijados[jugador][barcos[jugador][barcoActual[jugador]].get_formaActual()]) {
        while (fijados[jugador][barcos[jugador][barcoActual[jugador]].get_formaActual()]) {
          barcos[jugador][barcoActual[jugador]].set_formaActual((barcos[jugador][barcoActual[jugador]].get_formaActual() + 1) % 5);
        }
        //barcos[jugador][barcoActual[jugador]].set_formaActual((barcos[jugador][barcoActual[jugador]].get_formaActual()+1)%5);
        barcos[jugador][barcoActual[jugador]].cambiarForma(pantalla[jugador]);
        barcos[jugador][barcoActual[jugador]].dibujar(matriz, PANTALLA[jugador]);  // Verde para barcos no fijados
      } else if (mostrarBarcoNoFijo) {
        barcos[jugador][barcoActual[jugador]].dibujar(matriz, PANTALLA[jugador]);
      }
    }
  }
}
bool atacar(uint8_t jugador) {
  if (jugador == 0) {
    matPuntero = PANTALLA2;
    matBarco = PANTALLA4;
  } else {
    matPuntero = PANTALLA3;
    matBarco = PANTALLA1;
  }
  if (!estaFijado) {
    haAtacado[jugador] = false;
    if (botonesNES[jugador] & (1 << BOTON_IZQUIERDA) && puntero[0] > 0)
      puntero[0]--;
    else if (botonesNES[jugador] & (1 << BOTON_DERECHA) && puntero[0] < ANCHO_MATRIZ - 1)
      puntero[0]++;
    else if (botonesNES[jugador] & (1 << BOTON_ARRIBA) && puntero[1] > 0)
      puntero[1]--;
    else if (botonesNES[jugador] & (1 << BOTON_ABAJO) && puntero[1] < ALTO_MATRIZ - 1)
      puntero[1]++;

    else if (botonesNES[jugador] & (1 << BOTON_A) && matrizPuntero[jugador][puntero[0]][puntero[1]] == 0) {
      delay(150);
      estaFijado = true;
    }
    x_atacar[jugador] = puntero[0];
    y_atacar[jugador] = puntero[1];
    pixelIndex = ((puntero[1] % 2) ? puntero[1] * ANCHO_MATRIZ + puntero[0] : puntero[1] * ANCHO_MATRIZ + (ANCHO_MATRIZ - 1 - puntero[0]));
    matriz.setPixelColor(pixelIndex + matPuntero, matriz.Color(255, 0, 0));  // Convierte las coordenadas (X, Y) al índice en la matriz en formato serpiente
  } else {                                                                   //casilla fijada
    for (uint8_t actual = 0; actual < MAX_BARCOS; actual++) {
      if (barcos[(jugador + 1) % 2][actual].get_estaVivo()){
      if (barcos[(jugador + 1) % 2][actual].recibeAtaque(matriz, puntero[0], puntero[1], barcosDerribados[(jugador + 1) % 2])) {  // TOCADO

        audio.playFolder(4,3);  //boom                                                                                                            //BOOM
        tocado[jugador] = false;
        // if (!(barcos[(jugador + 1) % 2][actual].get_estaVivo()))  //TOCADO Y HUNDIDO (EL BARCO ENTERO HA EXPLOTADO)
        // {
        //   delay(50);
        //   audio.play(4);  //BOOM        //COMPROBAR SI HAY POCO TIEMPO DE ESPACIO ENTRE EXPLOSIONES
        //   delay(250);
        //   audio.play(4);  //BOOM
          
        // }

        matrizPuntero[jugador][x_atacar[jugador]][y_atacar[jugador]] = 1;
        casillaPuntero[jugador][contCas[jugador]] = pixelIndex + matPuntero;
        hit[jugador][contCas[jugador]] = true;
        contCas[jugador]++;
        actual = MAX_BARCOS;
      } else {
        tocado[jugador] = true;
        matrizPuntero[jugador][x_atacar[jugador]][y_atacar[jugador]] = 2;
        casillaPuntero[jugador][contCas[jugador]] = pixelIndex + matPuntero;
        hit[jugador][contCas[jugador]] = false;
        contCas[jugador]++;
        //audio.playFolder(1,1);  //AGUA
      }
      }
    }

    estaFijado = false;
    puntero[0] = 7;
    puntero[1] = 7;
    haAtacado[jugador] = true;
  }
}

void fin_partida(uint8_t jugador) {
  //TEXTO BATTLESHIP (TITULO)
  for (int y = 0; y < numRows; y++) {
    for (int x = 0; x < numCols; x++) {
      // Leer colores desde la imagen
      r = int(pgm_read_byte_near(&pc2[y][x][0]));
      g = int(pgm_read_byte_near(&pc2[y][x][1]));
      b = int(pgm_read_byte_near(&pc2[y][x][2]));
      if (((r * i) % 255 < 120) && ((g * i) % 255 < 110) && ((b * i) % 255 < 120)) {
        matriz.drawPixel(x, y + 16 + 16 * jugador, matriz.Color(r, g, b));
      } else {

        matriz.drawPixel(x, y + 16 + 16 * jugador, matriz.Color((r * i) % 255, (g * i) % 255, (b * i) % 255));
      }
    }
  }
  j++;
  if (j == 20) {
    j = 1;
    i++;
  }

  if (barcosDerribados[jugador] >= MAX_BARCOS) {
    //WINNER
    printWithSpacing(looser, 7, posText4[jugador], 0 + 48 * jugador);      // Texto, espaciado, posición inicial (X, Y)  WAITING
    printWithSpacing(looser, 7, posText4[jugador] + 1, 0 + 48 * jugador);  // Texto, espaciado, posición inicial (X, Y)
    Parpadeo = false;
    int textWidth = strlen(winner.c_str()) * 7;  // 6 píxeles por cada carácter
    if (posText4[jugador] < -textWidth) {        // Reseteamos la posición cuando el texto se haya movido fuera de la pantalla
      posText4[jugador] = matriz.width();
    }
    // Mover el texto hacia la izquierda
    posText4[jugador]--;

    //LOOSER
    printWithSpacing(winner, 7, posText5[(jugador+1)%2], 0 + 48 * (jugador+1)%2);      // Texto, espaciado, posición inicial (X, Y)  WAITING
    printWithSpacing(winner, 7, posText5[(jugador+1)%2] + 1, 0 + 48 * (jugador+1)%2);  // Texto, espaciado, posición inicial (X, Y)
    Parpadeo = false;
    int textWidth5 = strlen(looser.c_str()) * 7;  // 6 píxeles por cada carácter
    if (posText5[!jugador] < -textWidth5) {       // Reseteamos la posición cuando el texto se haya movido fuera de la pantalla
      posText5[!jugador] = matriz.width();
    }
    // Mover el texto hacia la izquierda
    posText5[!jugador]--;
  }
}

// Bucle principal del programa
void loop() {
  currentTime = millis();
  limpiarMatriz();  // Limpia la matriz LED al inicio de cada iteración
  // Serial.println("holaa");
  botonesNES[0] = leerNES(0);  // Lee los botones presionados
  botonesNES[1] = leerNES(1);
  // Control del parpadeo para el barco no fijado
  unsigned long tiempoActual = millis();
  unsigned long intervaloParpadeo = mostrarBarcoNoFijo ? tiempoEncendido : tiempoApagado;
  if (tiempoActual - ultimoTiempoParpadeo >= intervaloParpadeo) {
    ultimoTiempoParpadeo = tiempoActual;
    mostrarBarcoNoFijo = !mostrarBarcoNoFijo;  // Alterna entre visible e invisible
  }



  // Dibuja todos los barcos que ya están fijados
if(estado!=FIN_PARTIDA){
  for (uint8_t i = 0; i <= barcoActual[0]; i++) {
    //  Serial.println(barcoActual[0]);

    if (barcos[0][i].get_estaFijo())
      barcos[0][i].dibujar(matriz, PANTALLA1);  // Los barcos fijados son azules
  }

  for (uint8_t i = 0; i <= barcoActual[1]; i++) {
    if (barcos[1][i].get_estaFijo())
      barcos[1][i].dibujar(matriz, PANTALLA4);
  }
}

  // for (uint8_t jugador = 0; jugador < 2; jugador++) {
  //   for (uint16_t i = 0; i < contCas[jugador]; i++) {
  //     if (!hit[jugador][i])
  //      matriz.drawPixel(x_atacar[jugador],y_atacar[jugador], AZUL);// matriz.setPixelColor(casillaPuntero[jugador][i], matriz.color(255,0,0));  // AGUA
  //     else
  //       matriz.drawPixel(x_atacar[jugador],y_atacar[jugador], NARANJA);//matriz.setPixelColor(casillaPuntero[jugador][i], AMARILLO);
  //   }
  // }
  for (uint8_t jugador = 0; jugador < 2; jugador++) {
    for (uint16_t i = 0; i < 16; ++i) {
      for (uint16_t j = 0; j < 16; ++j) {
        if (matrizPuntero[jugador][i][j] == 1) {

          matriz.drawPixel(i, j + 16 + 16 * jugador, NARANJA);
        }
        if (matrizPuntero[jugador][i][j] == 2) {
          matriz.drawPixel(i, j + 16 + 16 * jugador, AZUL);
        }
      }
    }
  }



  switch (estado) {
    case INICIO:
      menu(0);
      menu(1);
      if (listo[0] && listo[1]) {
        estado = COLOCACION;
      }
      break;
    case COLOCACION:
      colocar_barcos(0);
      colocar_barcos(1);
      if (fin[0] && fin[1])
        estado = TURNO_J1;
      break;
    case TURNO_J1:
      if (barcosDerribados[1] >= MAX_BARCOS) {


        limpiarMatriz();
        estado = FIN_PARTIDA;
        delay(100);
        audio.playFolder(3,2);  //VICTORY//INICIO(1) VICTORY(2) POP(3) BOOM(4)
      } else {

        atacar(0);
        if (haAtacado[0] && tocado[0])
          estado = TURNO_J2;
      }

      break;
    case TURNO_J2:
      if (barcosDerribados[0] >= MAX_BARCOS) {

        estado = FIN_PARTIDA;
        delay(100);
        audio.playFolder(3,2);  //VICTORY//INICIO(1) VICTORY(2) POP(3) BOOM(4)
        limpiarMatriz();
      } else {
        atacar(1);
        if (haAtacado[1] && tocado[1])
          estado = TURNO_J1;
      }

      break;
    case FIN_PARTIDA:
      fin_partida(0);
      fin_partida(1);
      
      break;
    default:
      break;
  }

  matriz.show();  // Actualiza la matriz LED con los cambios
  delay(20);      // Pequeña pausa para suavizar el parpadeo
}
