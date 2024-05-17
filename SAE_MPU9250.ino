#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 16000000UL              // Fréquence du CPU
#define BAUD 9600                     // Débit en bauds
#define MYUBRR F_CPU / 16 / BAUD - 1  // Calcul de la valeur d'UBRR

// Initialisation de l'USART
void USART_Init(unsigned int ubrr) {
  UBRR0H = (unsigned char)(ubrr >> 8);
  UBRR0L = (unsigned char)ubrr;
  UCSR0B = (1 << TXEN0);                          // Activer le transmetteur
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);         // Format de trame : 8 bits de données, 1 bit de stop
}

// Transmission d'un caractère via USART
void USART_Transmit(unsigned char data) {
  while (!(UCSR0A & (1 << UDRE0)));  // Attente du buffer de transmission vide
  UDR0 = data;                       // Envoi des données dans le buffer
}

// Transmission d'une chaîne de caractères via USART
void USART_Transmit_String(const char* str) {
  while (*str) {
    USART_Transmit(*str);
    str++;
  }
}

// Lecture de la tension du capteur sur la broche spécifiée
double lireTensionCapteur(uint8_t broche) {
  // Configuration du AVCC comme référence de tension pour l'ADC
  ADMUX = (1 << REFS0) | (broche & 0b00000111);

  // Démarrer la conversion analogique-numérique
  ADCSRA |= (1 << ADSC) | (1 << ADEN);  // lancer une conversion (ADSC) / mettre en route CAN (ADEN)

  // Attendre que la conversion soit terminée (ADSC devient 0)
  while (ADCSRA & (1 << ADSC));

  // Lire la valeur convertie
  uint16_t valeur = ADC;

  // Convertir la valeur lue en tension (entre 0V et Vcc)
  int tension = (valeur * 5.0) / 1023.0;

  return tension;
}

int main(void) {
  USART_Init(MYUBRR);  // Initialisation de l'USART

  while (1) {
    // Lecture des tensions des capteurs
    int tensionCapteur1 = lireTensionCapteur(PC0);
    int tensionCapteur2 = lireTensionCapteur(PC1);
    int tensionCapteur3 = lireTensionCapteur(PC2);
    // Logique de contrôle en fonction des tensions des capteurs
    char* direction;

    if (tensionCapteur2 > tensionCapteur1 and tensionCapteur2 > tensionCapteur3) {
      direction = "avancer"; // Avancer tout droit si le capteur du milieu détecte le signal le plus fort
    } else if (tensionCapteur1 > tensionCapteur2 and tensionCapteur1 > tensionCapteur3) {
      direction = "droite"; // Tourner à droite si le capteur de gauche détecte le signal le plus fort
    } else if (tensionCapteur3 > tensionCapteur1 and tensionCapteur3 > tensionCapteur2) {
      direction = "gauche"; // Tourner à gauche si le capteur de droite détecte le signal le plus fort
    } else {
      direction = "avancer"; // Par défaut, avancer tout droit
    }

    // Transmission de l'angle calculé et de la direction via USART
    USART_Transmit_String(direction);
    USART_Transmit(10);
    USART_Transmit(13);
    _delay_ms(500);  // Attendre une seconde avant d'envoyer à nouveau
  }

  return 0;
}
