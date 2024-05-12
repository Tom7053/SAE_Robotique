#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "MPU9250.h" // Ajout de la bibliothèque pour le capteur MPU9250

#define PI 3.14159

MPU9250 IMU(Wire, 0x68); // Déclaration de l'objet MPU9250
int status;

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
  ADCSRA |= (1 << ADSC) | (1 << ADEN);// lancer une conversion (ADSC) / mettre en route CAN (ADEN)

  // Lire la valeur convertie
  uint16_t valeur = ADC;

  // Convertir la valeur lue en tension (entre 0V et Vcc)
  double tension = (valeur * 5.0) / 1023.0;

  return tension;
}


int main(void) {
  #define F_CPU 16000000UL              // Fréquence du CPU
  #define BAUD 9600                     // Débit en bauds
  #define MYUBRR F_CPU / 16 / BAUD - 1  // Calcul de la valeur d'UBRR

  DDRC &= ~(1 << PC0); // PC0 en entrée
  DDRC &= ~(1 << PC1); // PC1 en entrée
  DDRC &= ~(1 << PC2); // PC2 en entrée

  USART_Init(MYUBRR); // Initialisation de l'USART
  sei();              // Activation des interruptions globales

  // Initialisation du capteur MPU9250
  Wire.begin();
  while (!Serial)
    ;

  status = IMU.begin();
  if (status < 0) {
    Serial.println("ERREUR");
    while (1)
      ;
  }

  while (1) {
    // Lecture des valeurs d'angle du capteur MPU9250
    IMU.readSensor();
    angle = mpu.getYaw(); // valeur de l'angle autour de Z

    // Lecture des tensions des capteurs
    double tensionCapteur1 = lireTensionCapteur(PC0);
    double tensionCapteur2 = lireTensionCapteur(PC1);
    double tensionCapteur3 = lireTensionCapteur(PC2);

    // Logique de contrôle en fonction des tensions des capteurs
    char direction;
    if (tensionCapteur2 > 2.5) { // Si le capteur du milieu détecte le signal fort
      direction = "avancer"; // Avancer tout droit
    } else if (tensionCapteur1 > 2.5) { // Si le capteur de gauche détecte le signal fort
      direction = "gauche"; // Tourner à gauche
    } else if (tensionCapteur3 > 2.5) { // Si le capteur de droite détecte le signal fort
      direction = "droite"; // Tourner à droite
    } else {
      direction = "avancer"; // Par défaut, avancer tout droit
    }

    // Transmission de l'angle calculé et de la direction via USART
    USART_Transmit((unsigned char)angle); // Convertir l'angle 
    USART_Transmit(direction); 

    // Attente
    _delay_ms(500);
  }
}
