#define F_CPU 16000000UL              // Fréquence du CPU
#define BAUD 9600                     // Débit en bauds
#define MYUBRR F_CPU / 16 / BAUD - 1  // Calcul de la valeur d'UBRR

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "MPU9250.h"  // Ajout de la bibliothèque pour le capteur MPU9250

#define PI 3.14159

volatile int flag = 0;          // Variable pour indiquer la réception de données
volatile char received_data;    // Variable pour stocker les données reçues

MPU9250 IMU(Wire,0x68);  // Déclaration de l'objet MPU9250
int status;

// Initialisation de l'USART
void USART_Init(unsigned int ubrr) {
  /* Configuration du taux de bauds */
  UBRR0H = (unsigned char)(ubrr >> 8);
  UBRR0L = (unsigned char)ubrr;
  // Activation du récepteur et du transmetteur, et activation de l'interruption de réception
  UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
  /* Configuration du format de trame : 8 bits de données, 1 bit de stop */
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Transmission via USART
void USART_Transmit(unsigned char data) {
  /* Attente du buffer de transmission vide */
  while (!(UCSR0A & (1 << UDRE0)))
    ;
  /* Envoi des données dans le buffer */
  UDR0 = data;
}

// Interruption pour la réception sur USART
ISR(USART_RX_vect) {
  received_data = UDR0;
  flag = 1;
}

int main(void) {
  USART_Init(MYUBRR);  // Initialisation de l'USART
  sei();               // Activation des interruptions globales

  // Initialisation du capteur MPU9250
  Wire.begin();
  while(!Serial) {}

  status = IMU.begin();
  if (status < 0) {
    Serial.println("ERREUR");
    while(1) {}
  }

  while (1) {
    // Lecture des valeurs d'angle du capteur MPU9250
    IMU.readSensor();
    double x = IMU.getMagX_uT();
    double y = IMU.getMagY_uT();
    double angle;

    if (x <= 0.1 || y <= 0.1) {
      angle = 0;
    }
    else {
      angle = atan2(x, y) * (180 / PI);
    }

    // Transmission de l'angle calculé via USART
    USART_Transmit(angle);

    // Transmission des données reçues via USART si disponible
    if (flag) {
      USART_Transmit(received_data);
      flag = 0;
    }

    // Attente
    _delay_ms(500);
  }
}
