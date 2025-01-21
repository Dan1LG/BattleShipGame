
#include "BattleShip.hpp"

// uint8_t contRotarLinea = 1;
// Matriz para marcar qué LEDs están ocupados por barcos fijados
//bool leds_ocupados[ALTO_MATRIZ][ANCHO_MATRIZ] = { false };

// Constructor: Inicializa un barco con una posición inicial y configura su estado inicial
Barco::Barco(uint8_t forma, uint8_t pantalla, uint8_t color) {
  estaVivo = true;   // El barco comienza en estado "vivo"
  estaFijo = false;  // El barco no está fijado inicialmente
  // for(uint8_t i = 0; i < size; i++)
  // {
  //   casillas[i].golpeado = false;
  // }
  formaActual = forma;  // Por defecto, el barco tiene la primera forma (cubo)
  numGolpes = 0;
  // ar[0]=0;
  // ar[1]=0;
  for (uint8_t i = 0; i < size; i++)
    casillas[i].color = color;
  colorfijado = color;
  cambiarForma(pantalla);  // Configura la forma inicial
}

// Cambia la forma del barco según el índice formaActual
void Barco::cambiarForma(uint8_t pantalla) {
  if (formaActual == 0) {
    establecerFormaCrucero(pantalla);
  } else if (formaActual == 1) {
    establecerFormaLinea(pantalla);
  } else if (formaActual == 2) {
    establecerFormaPortaaviones(pantalla);
  } else if (formaActual == 3) {
    establecerFormaSubmarino(pantalla);
  } else if (formaActual == 4) {
    establecerFormaLinea3(pantalla);
  }
}

// Define la forma del barco como un cubo
void Barco::establecerFormaCrucero(uint8_t pantalla) {
  //Primera linea del barco
  size = 6;
  casillas[0].x = 1 + 16 * pantalla + 7;
  casillas[0].y = 1 + 16 * pantalla + 7;
  casillas[1].x = 0 + 16 * pantalla + 7;  //esquina izda
  casillas[1].y = 1 + 16 * pantalla + 7;

  casillas[2].x = 2 + 16 * pantalla + 7;
  casillas[2].y = 0 + 16 * pantalla + 7;
  casillas[3].x = 1 + 16 * pantalla + 7;
  casillas[3].y = 0 + 16 * pantalla + 7;
  casillas[4].x = 2 + 16 * pantalla + 7;
  casillas[4].y = 1 + 16 * pantalla + 7;
  casillas[5].x = 3 + 16 * pantalla + 7;  //esquina dcha
  casillas[5].y = 1 + 16 * pantalla + 7;
}

// Define la forma del barco como una línea horizontal
void Barco::establecerFormaLinea(uint8_t pantalla) {
  //limpiarForma();  // Limpia la matriz de forma
  size = 5;
  casillas[0].x = 0 + 16 * pantalla + 7;
  casillas[0].y = 2 + 16 * pantalla + 7;
  casillas[1].x = 0 + 16 * pantalla + 7;
  casillas[1].y = 0 + 16 * pantalla + 7;
  casillas[2].x = 0 + 16 * pantalla + 7;
  casillas[2].y = 1 + 16 * pantalla + 7;
  for (uint8_t i = 3; i < size; i++) {
    casillas[i].x = 0 + 16 * pantalla + 7;
    casillas[i].y = i + 16 * pantalla + 7;
  }
  // (0,0) (0,1) (0,2) (0,3)
}

// Define la forma del barco como una "L"
void Barco::establecerFormaPortaaviones(uint8_t pantalla) {
  size = 6;

  casillas[0].x = 1 + 16 * pantalla + 7;
  casillas[0].y = 1 + 16 * pantalla + 7;
  casillas[1].x = 0 + 16 * pantalla + 7;  //esquina izda
  casillas[1].y = 0 + 16 * pantalla + 7;
  casillas[2].x = 1 + 16 * pantalla + 7;
  casillas[2].y = 0 + 16 * pantalla + 7;
  casillas[3].x = 2 + 16 * pantalla + 7;
  casillas[3].y = 0 + 16 * pantalla + 7;
  casillas[4].x = 2 + 16 * pantalla + 7;
  casillas[4].y = 1 + 16 * pantalla + 7;
  casillas[5].x = 3 + 16 * pantalla + 7;  //esquina dcha
  casillas[5].y = 1 + 16 * pantalla + 7;
}

void Barco::establecerFormaSubmarino(uint8_t pantalla) {
  size = 4;
  casillas[0].x = 1 + 16 * pantalla + 7;
  casillas[0].y = 1 + 16 * pantalla + 7;
  casillas[1].x = 0 + 16 * pantalla + 7;
  casillas[1].y = 1 + 16 * pantalla + 7;
  casillas[2].x = 1 + 16 * pantalla + 7;
  casillas[2].y = 2 + 16 * pantalla + 7;
  casillas[3].x = 1 + 16 * pantalla + 7;
  casillas[3].y = 0 + 16 * pantalla + 7;
}

void Barco::establecerFormaLinea3(uint8_t pantalla) {
  //limpiarForma();  // Limpia la matriz de forma
  size = 3;
  casillas[0].x = 0 + 16 * pantalla + 7;
  casillas[0].y = 1 + 16 * pantalla + 7;
  casillas[1].x = 0 + 16 * pantalla + 7;
  casillas[1].y = 0 + 16 * pantalla + 7;
  casillas[2].x = 0 + 16 * pantalla + 7;
  casillas[2].y = 2 + 16 * pantalla + 7;

  // (0,0) (0,1) (0,2) (0,3)
}

// Rota la forma del barco 90 grados en sentido horario
void Barco::rotar(uint8_t pantalla) {

  int anclaX = casillas[0].x;
  int anclaY = casillas[0].y;

  // Variables temporales para simular las nuevas posiciones
  int nuevaX[size];
  int nuevaY[size];

  // Simula la rotación respecto al punto ancla
  for (uint8_t i = 0; i < size; i++) {
    nuevaX[i] = anclaX - ((casillas[i].y % ALTO_MATRIZ) - anclaY);
    nuevaY[i] = anclaY + ((casillas[i].x % ANCHO_MATRIZ) - anclaX);

    // Verifica si alguna coordenada está fuera de los límites
    if (nuevaX[i] < 0 || nuevaX[i] >= ANCHO_MATRIZ || nuevaY[i] < 0 || nuevaY[i] >= ALTO_MATRIZ) {
      return;  // Cancela la rotación sin modificar las coordenadas originales
    }
  }

  // Si todas las posiciones son válidas, aplica las nuevas coordenadas
  for (uint8_t i = 0; i < size; i++) {
    casillas[i].x = nuevaX[i];
    casillas[i].y = nuevaY[i];
  }
}

void Barco::rotarLinea(uint8_t pantalla) {
  // contRotarLinea++;
  // if(contRotarLinea%2)  //impar
  // {
  //   casillas[0].x = 0 + 16 * pantalla + 7;
  //   casillas[0].y = 2 + 16 * pantalla + 7;
  //   casillas[2].x = 0 + 16 * pantalla + 7;
  //   casillas[2].y = 1 + 16 * pantalla + 7;
  // }
  // else  //si es par, cambio el eje de rotación
  // {
  //   casillas[0].x = 0 + 16 * pantalla + 7;
  //   casillas[0].y = 0 + 16 * pantalla + 7;
  //   casillas[1].x = 0 + 16 * pantalla + 7;
  //   casillas[1].y = 1 + 16 * pantalla + 7;
  // }
  // rotar(pantalla);
}

void Barco::rotarPortaAviones(uint8_t pantalla, uint8_t &numRotar) {
  // Coordenadas iniciales de las esquinas relevantes del portaaviones
  uint8_t x1 = casillas[1].x;  // Esquina izquierda
  uint8_t y1 = casillas[1].y;
  uint8_t x5 = casillas[5].x;  // Esquina derecha
  uint8_t y5 = casillas[5].y;

  // Cálculos para rotación según el estado actual (par o impar)
  if (numRotar % 2 == 0) {  // Rotación horizontal a vertical
    // Calcula las nuevas posiciones
    uint8_t nuevoX1 = x1 + 2;
    uint8_t nuevoY1 = y1 - 1 ;
    uint8_t nuevoX5 = x5 - 2;
    uint8_t nuevoY5 = y5 + 1 ;

    // Verifica los límites
    if (nuevoX1 >= 16 * pantalla && nuevoX1 < 16 * (pantalla + 1) && nuevoY1 >= ALTO_MATRIZ * pantalla && nuevoY1 < ALTO_MATRIZ * (pantalla + 1) && nuevoX5 >= 16 * pantalla && nuevoX5 < 16 * (pantalla + 1) && nuevoY5 >= ALTO_MATRIZ * pantalla && nuevoY5 < ALTO_MATRIZ * (pantalla + 1)) {

      // Aplica las nuevas posiciones
      casillas[1].x = nuevoX1;
      casillas[1].y = nuevoY1;
      casillas[5].x = nuevoX5;
      casillas[5].y = nuevoY5;
    } else {
      return;  // No se puede rotar, límites excedidos
    }
  } else {  // Rotación vertical a horizontal
    // Calcula las nuevas posiciones
    uint8_t nuevoX1 = x1 - 2;
    uint8_t nuevoY1 = y1 + 1 ;
    uint8_t nuevoX5 = x5 + 2;
    uint8_t nuevoY5 = y5 - 1 ;

    // Verifica los límites
    if (nuevoX1 >= 16 * pantalla && nuevoX1 < 16 * (pantalla + 1) && nuevoY1 >= ALTO_MATRIZ * pantalla && nuevoY1 < ALTO_MATRIZ * (pantalla + 1) && nuevoX5 >= 16 * pantalla && nuevoX5 < 16 * (pantalla + 1) && nuevoY5 >= ALTO_MATRIZ * pantalla && nuevoY5 < ALTO_MATRIZ * (pantalla + 1)) {

      // Aplica las nuevas posiciones
      casillas[1].x = nuevoX1;
      casillas[1].y = nuevoY1;
      casillas[5].x = nuevoX5;
      casillas[5].y = nuevoY5;
    } else {
      return;  // No se puede rotar, límites excedidos
    }
  }

  // Incrementa el contador de rotaciones
  ++numRotar;
}

void Barco::rotarCrucero(uint8_t pantalla, uint8_t &numRotar) {
    // Coordenadas relevantes (extremos)
    uint8_t xIzquierda = casillas[1].x; // Extremo izquierdo
    uint8_t yIzquierda = casillas[1].y;
    uint8_t xDerecha = casillas[5].x;   // Extremo derecho
    uint8_t yDerecha = casillas[5].y;

    // Calcula las nuevas posiciones para los extremos en base a la rotación
    uint8_t nuevoXIzquierda, nuevoYIzquierda;
    uint8_t nuevoXDerecha, nuevoYDerecha;

    switch (numRotar % 4) {
        case 0: // Posición inicial (horizontal)
            nuevoXIzquierda = xIzquierda + 1;
            nuevoYIzquierda = yIzquierda - 2 +16*pantalla;
            nuevoXDerecha = xDerecha - 2;
            nuevoYDerecha = yDerecha + 1 +16*pantalla;
            break;

        case 1: // 90° en sentido horario
            nuevoXIzquierda = xIzquierda - 1; 
            nuevoYIzquierda = yIzquierda + 1 +16*pantalla; 
            nuevoXDerecha = xDerecha  + 2;
            nuevoYDerecha = yDerecha - 2 +16*pantalla;
            break;

        case 2: // 180° (invertido)
            nuevoXIzquierda = xIzquierda + 2;
            nuevoYIzquierda = yIzquierda - 1 +16*pantalla;
            nuevoXDerecha = xDerecha  - 1;
            nuevoYDerecha = yDerecha  + 2 +16*pantalla;
            break;

        case 3: // 270° en sentido horario
            nuevoXIzquierda = xIzquierda - 2; 
            nuevoYIzquierda = yIzquierda + 2 +16*pantalla; 
            nuevoXDerecha = xDerecha + 1; 
            nuevoYDerecha = yDerecha - 1 +16*pantalla;
            break;
    }

    // Verifica que las nuevas posiciones estén dentro de los límites
    if (nuevoXIzquierda < 16 * pantalla || nuevoXIzquierda >= 16 * (pantalla + 1) ||
        nuevoYIzquierda < ALTO_MATRIZ * pantalla || nuevoYIzquierda >= ALTO_MATRIZ * (pantalla + 1) ||
        nuevoXDerecha < 16 * pantalla || nuevoXDerecha >= 16 * (pantalla + 1) ||
        nuevoYDerecha < ALTO_MATRIZ * pantalla || nuevoYDerecha >= ALTO_MATRIZ * (pantalla + 1)) {
        return; // Cancela la rotación si alguna coordenada está fuera de los límites
    }

    // Actualiza las posiciones de los extremos
    casillas[1].x = nuevoXIzquierda;
    casillas[1].y = nuevoYIzquierda +16*pantalla;
    casillas[5].x = nuevoXDerecha;
    casillas[5].y = nuevoYDerecha +16*pantalla;

    // Incrementa el contador de rotaciones
    ++numRotar;
}

// Dibuja el barco en la matriz LED con un color específico
void Barco::dibujar(Adafruit_NeoPixel &matriz, uint32_t pantalla) {

  for (uint8_t i = 0; i < size; i++) {
    uint32_t index = ((casillas[i].y % 2) ? casillas[i].y * ANCHO_MATRIZ + (casillas[i].x % ANCHO_MATRIZ) : casillas[i].y * ANCHO_MATRIZ + (ANCHO_MATRIZ - 1 - (casillas[i].x) % ANCHO_MATRIZ));
    index += pantalla;
    // matriz.setPixelColor(index, color);
    switch ((casillas[i].color)) {
      case 0:
        matriz.setPixelColor(index, matriz.Color(0, 255, 0));
        break;
      case 1:
        matriz.setPixelColor(index, matriz.Color(0, 0, 255));
        break;
      case 2:
        matriz.setPixelColor(index, matriz.Color(255, 0, 0));
        break;
      case 3:
        matriz.setPixelColor(index, matriz.Color(255, 255, 0));
        break;
              case 4:
        matriz.setPixelColor(index, matriz.Color(255, 51, 153)); //ROSA
        break;
              case 5:
        matriz.setPixelColor(index, matriz.Color(178, 102, 255)); //MORADO
        break;
              case 6:
        matriz.setPixelColor(index, matriz.Color(255, 255, 255)); //BLANCO
        break;
              case 7:
        matriz.setPixelColor(index, matriz.Color(161, 130, 98));  //MARRON
        break;
              case 8:
        matriz.setPixelColor(index, matriz.Color(0, 255, 255)); //CIAN
        break;
      default:
        break;
    }
  }
}
// void Barco::dibujar(Adafruit_NeoMatrix &matriz, uint32_t pantalla) {

//   for (uint8_t i = 0; i < size; i++) {
//     // uint32_t index = ((casillas[i].y % 2) ? casillas[i].y * ANCHO_MATRIZ + (casillas[i].x % ANCHO_MATRIZ) : casillas[i].y * ANCHO_MATRIZ + (ANCHO_MATRIZ - 1 - (casillas[i].x) % ANCHO_MATRIZ));
//     // index += pantalla;
//     // matriz.setPixelColor(index, color);
//     switch ((casillas[i].color)) {
//       case 0:
//         matriz.drawPixel(casillas[i].x,casillas[i].y+48*pantalla, matriz.Color(0, 255, 0));
//         break;
//       case 1:
//       matriz.drawPixel(casillas[i].x,casillas[i].y+48*pantalla, matriz.Color(0, 255, 0));
//         //matriz.setPixelColor(index, matriz.Color(0, 0, 255));
//         break;
//       case 2:
//       matriz.drawPixel(casillas[i].x,casillas[i].y+48*pantalla, matriz.Color(0, 255, 0));
//         //matriz.setPixelColor(index, matriz.Color(255, 0, 0));
//         break;
//       case 3:
//       matriz.drawPixel(casillas[i].x,casillas[i].y+48*pantalla, matriz.Color(0, 255, 0));
//         //matriz.setPixelColor(index, matriz.Color(255, 255, 0));
//         break;
//       default:
//         break;
//     }
//   }
// }
// Verifica si el barco puede fijarse en su posición actual
bool Barco::puedeFijarse() {
  for (uint8_t i = 0; i < size; i++) {
    uint8_t x = casillas[i].x;
    uint8_t y = casillas[i].y;

    // Verifica que las coordenadas estén dentro de los límites y no ocupadas
    if (x < 0 || x >= ANCHO_MATRIZ || y < 0 || y >= ALTO_MATRIZ) {
      return false;  // Fuera de los límites o LED ya ocupado
    }
  }
  return true;  // Todas las posiciones son válidas
}

// Fija el barco en la posición actual y actualiza la matriz de LEDs ocupados
void Barco::fijar(uint8_t color) {
  for (uint8_t i = 0; i < size; i++) {
    uint8_t x = casillas[i].x;
    uint8_t y = casillas[i].y;
    casillas[i].color = colorfijado;
    //leds_ocupados[y][x] = true;  // Marca la posición como ocupada
  }
  estaFijo = true;  // Marca el barco como fijado
}

// Mueve el barco a una nueva posición si es válida
void Barco::mover(uint8_t desplazamientoX, uint8_t desplazamientoY, uint8_t pantalla) {
  // Verifica si el movimiento es válido en el eje X
  if (desplazamientoX != 0) {
    for (uint8_t i = 0; i < size; i++) {
      uint8_t nuevaX = (casillas[i].x % ANCHO_MATRIZ) + desplazamientoX;
      if (nuevaX < 0 || nuevaX >= ANCHO_MATRIZ) return;  // Sal de la función si el movimiento excede los límites
    }
    for (uint8_t i = 0; i < size; i++) {
      casillas[i].x += desplazamientoX;  // Aplica el desplazamiento en X
    }
  }

  // Verifica si el movimiento es válido en el eje Y
  if (desplazamientoY != 0) {
    for (uint8_t i = 0; i < size; i++) {
      uint8_t nuevaY = (casillas[i].y % ALTO_MATRIZ) + desplazamientoY;
      if (nuevaY < 0 || nuevaY >= ALTO_MATRIZ) return;  // Sal de la función si el movimiento excede los límites
    }
    for (uint8_t i = 0; i < size; i++) {
      casillas[i].y += desplazamientoY;  // Aplica el desplazamiento en Y
    }
  }
}


bool Barco::recibeAtaque(Adafruit_NeoPixel &matriz, uint8_t x, uint8_t y, uint8_t &num_derribado) {
  bool acertado = false;
  for (uint8_t i = 0; i < size; i++) {
    if (casillas[i].x == x && casillas[i].y == y) {
      acertado = true;  //TOCADO
      numGolpes++;
      casillas[i].color = 3;
    }
  }

  if (numGolpes >= size) {  //TOCADO Y HUNDIDO
    estaVivo = false;
    num_derribado++;
    for (uint8_t i = 0; i < size; i++)
      casillas[i].color = 2;
  }
  return acertado;
}

bool Barco::verificarSolapamiento(Barco barco) {
  for (uint8_t i = 0; i < barco.get_size(); i++) {
    if (compararCoordenadas(barco.casillas[i].x, barco.casillas[i].y)) {
      for (uint8_t j = 0; j < size; j++) {
        casillas[j].color = 2;
      }
      return true;
    } else {
      for (uint8_t j = 0; j < size; j++) {
        casillas[j].color = 0;
      }
    }
  }
  return false;
}


bool Barco::compararCoordenadas(uint8_t x, uint8_t y) {
  for (uint8_t i = 0; i < size; i++) {
    if (casillas[i].x == x && casillas[i].y == y) {
      return true;  // Coordenada coincidente encontrada
    }
  }
  return false;  // No hay coincidencias
}

bool Barco::get_estaFijo() {
  return estaFijo;
}

bool Barco::get_estaVivo() {
  return estaVivo;
}
uint8_t Barco::get_formaActual() {
  return formaActual;
}
uint8_t Barco::get_size() {
  return size;
}

uint8_t Barco::get_color() {
  return colorfijado;
}

void Barco::set_formaActual(uint8_t forma) {
  formaActual = forma;
}

void Barco::set_color(uint8_t color, uint8_t pos) {
  casillas[pos].color = color;
}
