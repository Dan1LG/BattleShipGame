#ifndef BARCO_HPP
#define BARCO_HPP

#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>

// Definición de la estructura de Casillas para representar las coordenadas y el color.
typedef struct Casilla {
    uint8_t x : 4;    // Utiliza 4 bits para coordenadas (por ejemplo, máximo 16x16 = 256)
    uint8_t y : 4;
    uint8_t color; // Usa 2 bits para representar los colores (hasta 4 colores)
} Casillas;

// Dimensiones de la matriz LED
const uint8_t ANCHO_MATRIZ = 16;
const uint8_t ALTO_MATRIZ = 16;

class Barco {
public:
    Barco(uint8_t forma,uint8_t pantalla = 0, uint8_t color = 0);
    void cambiarForma(uint8_t pantalla = 0);
    void establecerFormaCrucero(uint8_t pantalla = 0);
    void establecerFormaLinea(uint8_t pantalla = 0);
    void establecerFormaSubmarino(uint8_t pantalla);
    void establecerFormaPortaaviones(uint8_t pantalla);
    void establecerFormaLinea3(uint8_t pantalla);
    void limpiarForma();
    void rotar(uint8_t pantalla);
    void rotarLinea(uint8_t pantalla);
    void rotarPortaAviones(uint8_t pantalla, uint8_t &numRotar);
    void rotarCrucero(uint8_t pantalla, uint8_t &numRotar);
    void dibujar(Adafruit_NeoPixel &matriz, uint32_t pantalla = 0);
    bool puedeFijarse();
    void fijar(uint8_t color);
    void mover(uint8_t desplazamientoX, uint8_t desplazamientoY, uint8_t pantalla);
    bool recibeAtaque(Adafruit_NeoPixel &matriz, uint8_t x, uint8_t y, uint8_t &num_derribado);
    bool verificarSolapamiento(Barco barco);
    bool get_estaFijo();
    bool get_estaVivo();
    uint8_t get_formaActual();
    uint8_t get_size();
    uint8_t get_color();
    void set_formaActual(uint8_t forma);
    void set_color(uint8_t color, uint8_t pos);

private:
    Casillas casillas[10];  // Limitado a 4 casillas por barco (puede ajustarse si necesario)
    bool estaVivo;
    bool estaFijo;
    uint8_t formaActual;
    uint8_t numGolpes;
    uint8_t size;
    uint8_t colorfijado;
    bool compararCoordenadas(uint8_t x, uint8_t y);
    bool fijados[5];
};

#endif
