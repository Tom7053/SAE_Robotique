#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "MPU9250.h"  // Ajout de la bibliothèque pour le capteur MPU9250

#define F_CPU 16000000UL              // Fréquence du CPU
#define BAUD 9600                     // Débit en bauds
#define MYUBRR F_CPU / 16 / BAUD - 1  // Calcul de la valeur d'UBRR

MPU9250 mpu;
int status;
double angle;

// Initialisation de l'USART
void USART_Init(unsigned int ubrr) {
  /* Configuration du taux de bauds */
  UBRR0H = (unsigned char)(ubrr >> 8);
  UBRR0L = (unsigned char)ubrr;
  // Activation du transmetteur
  UCSR0B = (1 << TXEN0);
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


// Lecture de la tension du capteur sur la broche spécifiée
double lireTensionCapteur(uint8_t broche) {
  // Configuration du AVCC comme référence de tension pour l'ADC
  ADMUX = (1 << REFS0) | (broche & 0b00000111);

  // Démarrer la conversion analogique-numérique
  ADCSRA |= (1 << ADSC) | (1 << ADEN);  // lancer une conversion (ADSC) / mettre en route CAN (ADEN)

  // Lire la valeur convertie
  uint16_t valeur = ADC;

  // Convertir la valeur lue en tension (entre 0V et Vcc)
  double tension = (valeur * 5.0) / 1023.0;

  return tension;
}

// Transmission d'une chaîne de caractères via USART
void USART_Transmit_String(const char* str) {
  while (*str) {
    USART_Transmit(*str);
    str++;
  }
}

int main(void) {

  DDRC &= ~(1 << PC0);  // PC0 en entrée
  DDRC &= ~(1 << PC1);  // PC1 en entrée
  DDRC &= ~(1 << PC2);  // PC2 en entrée

  USART_Init(MYUBRR);  // Initialisation de l'USART
  sei();               // Activation des interruptions globales

  // Initialisation du capteur MPU9250
  Wire.begin();
  delay(2000);

  if (!mpu.setup(0x68)) {
    while (1) {
      Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
      delay(5000);
    }
  }

  while (1) {
    // Lecture des valeurs d'angle du capteur MPU9250
    /*mpu.update();
    angle = mpu.getYaw();  // valeur de l'angle autour de Z*/

    // Lecture des tensions des capteurs
double tensionCapteur1 = lireTensionCapteur(PC0);
double tensionCapteur2 = lireTensionCapteur(PC1);
double tensionCapteur3 = lireTensionCapteur(PC2);

// Logique de contrôle en fonction des tensions des capteurs
char direction;

if (tensionCapteur2 > tensionCapteur1 && tensionCapteur2 > tensionCapteur3) {
    direction = "avancer"; // Avancer tout droit si le capteur du milieu détecte le signal le plus fort
} else if (tensionCapteur1 > tensionCapteur2 && tensionCapteur1 > tensionCapteur3) {
    direction = "droite"; // Tourner à droite si le capteur de gauche détecte le signal le plus fort
} else if (tensionCapteur3 > tensionCapteur1 && tensionCapteur3 > tensionCapteur2) {
    direction = "gauche"; // Tourner à gauche si le capteur de droite détecte le signal le plus fort
} else {
    direction = "avancer"; // Par défaut, avancer tout droit
}

  // Transmission de l'angle calculé et de la direction via USART
  //USART_Transmit((unsigned char)angle);  // Convertir l'angle
  USART_Transmit_String(direction);
  // Attente
  _delay_ms(500);
}
}
