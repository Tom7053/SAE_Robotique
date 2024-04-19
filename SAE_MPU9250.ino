
#include "MPU9250.h"

MPU9250 IMU(Wire, 0x68);
#define FOSC 16000000  
#define BAUD 115200
#define MYUBRR FOSC / 16 / BAUD - 1

/*! \brief Fonction Main
 */
int main(void) {
  USART_Init(MYUBRR);
  sei();
  int status = IMU.begin();
  if (status < 0) {
    while (1) {}
  }
  while (1) {
    _delay_ms(200);
    SensorData();
  }
}

/*! \brief Fonction Initialisation de la communication
 */
void USART_Init(unsigned int ubrr) {
  /*Set baud rate */
  UBRR0H = (unsigned char)(ubrr >> 8);
  UBRR0L = (unsigned char)ubrr;
  //Enable receiver and transmitter * /
  UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
  /* Set frame format: 8data, 2stop bit */
  UCSR0C = (1 << USBS0) | (3 << UCSZ00);
}

/*! \brief Fonction Transmission de la communication
 */
void USART_Transmit(unsigned char data) {
  /* Attendre que toutes les données soient chargées */
  while (!(UCSR0A & (1 << UDRE0)))
    ;
  UDR0 = data;
}

/*! \brief Fonction de lecture et envoi des données du capteur
 */
void SensorData() {
  IMU.readSensor();
  USART_Transmit(IMU.getAccelX_mss(), IMU.getAccelY_mss(), IMU.getAccelZ_mss(),
          IMU.getGyroX_rads(), IMU.getGyroY_rads(), IMU.getGyroZ_rads());
  }
}
