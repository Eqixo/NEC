/*************************************************************************************************************************************/
// Sujet   :  Definition de la bibliotheque de fonctions NEC                                                                         //
// Auteurs :  AUGEREAU F. JOBARD L.                                                                                                  //
// Date    :  14/03/2025                                                                                                             //
// Version :  1.5.3                                                                                                                  //
/*************************************************************************************************************************************/
//                                                                                                                                   //
// 2 fonctions sont fournies dans cette bibliotheque NEC :                                                                           //
//                                                                                                                                   //
//   => fonction d'emission d'une trame NEC                                                                                          //
//     syntaxe : void GenererTrameNEC(int Broche, uint8_t Adresse, uint8_t Donnee);                                                  //
//          Broche est la valeur de la broche Arduino sur laquelle est connectee la LED d'émission infrarouge                        //
//          Adresse est la valeur de l'adresse NEC à transmettre dans la trame NEC                                                   //
//          Donnee est la valeur de la donnee NEC à transmettre dans la trame NEC                                                    //
//                                                                                                                                   //
//   => fonction de reception d'une trame NEC                                                                                        //
//     syntaxe : int8_t AcquerirTrameNEC(int Broche, uint8_t* Adresse, uint8_t* Donnee);                                             //
//          AcquerirTrameNEC retourne une valeur d'erreur (0 : trame NEC valide ; -1 : erreur de reception NEC ; -2 : trame absente) //
//          Broche est la valeur de la broche Arduino sur laquelle est connectee le recepteur infrarouge                             //
//          *Adresse retourne la valeur de l'adresse NEC incluse dans la trame NEC recue (passage de la valeur par pointeur)         //
//          *Donnee retourne la valeur de la donnee NEC incluse dans la trame NEC recue (passage de la valeur par pointeur)          //
//                                                                                                                                   //
/*************************************************************************************************************************************/

// inclusion des fichiers header des bibliothèques de fonctions Arduino
#include <stdint.h>
#include <stdbool.h>
#include <arduino.h>
#include "NEC.h"

// definition des fonctions assembly
void asmDelay(uint32_t cycles) {
  // nous utilisons une boucle en assembleur car l'assembleur est plus rapide que le C++ pour faire des boucles
  // la boucle est volatile car nous utilisons des registres qui ne sont pas censees etre utilisees par le compilateur
  // nous utilisons l'instruction "sbis" (skip if bit in I/O register set) pour
  // attendre que le registre de n bits passe de la valeur 'cycles' a 0
  // nous utilisons l'instruction "brne" (branch if not equal) pour reboucler
  // sur la premiere instruction de la boucle tant que le registre n'est pas égal a 0
  register uint32_t i = 0;
  __asm__ __volatile__ ( 
    "1: sbiw %0, 1\n\t"             // decremente le registre de n bits (2 clk cycles par itération)
    "brne 1b"                       // le registre de n bits est stocké dans la variable i
    : "=w" (i)                      // le registre de n bits est initialisé a 'cycles'
    : "0" (cycles)                  // temps d'execution = ('cycles' itérations * 4 clk cycles) - 1/(16 * 10^6 Hz)
  );
}

// definition des fonctions d'emission de la bibliotheque NEC
void GenererTrameNEC(int Broche, uint8_t Adresse, uint8_t Donnee) {// GenererTrameNEC : fonction d'action
  register uint32_t trame = (Adresse << 24) | (~Adresse << 16) | (Donnee << 8) | ~Donnee & 0xFF; // register pour booster la vitesse
  GenererBurstNEC(Broche, 342);       // generation du header (en-tête) NEC
  asmDelay(18000);                    // temps a l'état bas apres entête du protocole NEC : (18000 itérations * 4 clk cycles) - 1/(16 * 10^6 Hz) = 4.4999375 ms
  for (uint8_t i = 0; i < 32; ++i) {
    GenererBurstNEC(Broche, 21);
    asmDelay((trame >> (31 - i)) & 1 ? 6748 : 2248); // (6748 itérations * 4 clk cycles) - 1/(16 * 10^6 Hz) = 1686.9375 µs pour 1
  }                                                  // (2248 itérations * 4 clk cycles) - 1/(16 * 10^6 Hz) = 561.9375 µs pour 0
  GenererBurstNEC(Broche, 21);        // burst final d'une trame NEC
  asmDelay(162000);                   // delai inter-trame de (162000 * 4) - 1 / (16 * 10^6 Hz) = 40.4999375 ms
}

void GenererBurstNEC(int Broche, uint16_t pulses) { // GenererBurstNEC : fonction d'action
  for (register uint16_t i = 0; i < pulses; i++) {
    GenererImpulsion38kHzNEC(Broche); // generation de l'impulsion du protocole NEC
  }
}

void GenererImpulsion38kHzNEC(int Broche) { // GenererImpulsion38kHzNEC : fonction d'action
  controlerDiodeIR(Broche, true);
  asmDelay(35); // temps a l'etat haut apres l'impulsion du protocole NEC : (35 itérations * 4 clk cycles) - 1/(16 * 10^6 Hz) = 8.6875 µs
  controlerDiodeIR(Broche, false);
  asmDelay(71); // temps a l'etat bas apres l'impulsion du protocole NEC : (71 itérations * 4 clk cycles) - 1/(16 * 10^6 Hz) = 17.6875 µs
}

inline void controlerDiodeIR(int Broche, bool etat) { // controlerDiodeIR : fonction d'action
  if (Broche >= 0 && Broche <= 7)                                         // la broche doit etre comprise entre 0 et 7
      PORTD = (PORTD & ~(1 << Broche)) | (etat << Broche);                // on change l'etat de la broche
  if (Broche >= 8 && Broche <= 13)                                        // la broche doit etre comprise entre 8 et 13
      PORTB = (PORTB & ~(1 << (Broche - 8))) | (etat << (Broche - 8));    // on change l'etat de la broche
  if (Broche >= A0 && Broche <= A5)                                       // la broche doit etre comprise entre A0 et A5
      PORTC = (PORTC & ~(1 << (Broche - A0))) | (etat << (Broche - A0));  // on change l'etat de la broche
}

// Définition des fonctions de reception de la bibliotheque NEC
int8_t AcquerirTrameNEC(int Broche, uint8_t *ptr_Adresse, uint8_t *ptr_Donnee) { // AcquerirTrameNEC : fonction d'acquisition
  int16_t octet = AcquerirOctetNEC(Broche);  // Acquisition de l'adresse
  if (AcquerirEnteteNEC(Broche) < 0 || octet < 0 || AcquerirOctetNEC(Broche) != ~octet)
    return -1;                               // Erreur de timing
  *ptr_Adresse = octet;                      // Stockage de l'adresse
  octet = AcquerirOctetNEC(Broche);          // Acquisition de la donnée
  if (octet < 0 || AcquerirOctetNEC(Broche)  != ~octet || AcquerirFrontDescendantNEC(Broche) < 0)
    return -1;                               // Erreur de timing
  *ptr_Donnee = octet;                       // Stockage de la donnée
  return 0;                                  // Trame correcte
}

int8_t AcquerirEnteteNEC(int Broche) {          // AcquerirEnteteNEC : fonction d'acquisition
  int32_t t = AcquerirFrontNEC(Broche, true);   // Acquisition du front montant
  if (t < 4050 || t > 4950) return -1;          // Controle de timing       4500us du protocole NEC (-/+10%)
  t = AcquerirFrontNEC(Broche, false);          // Acquisition du front descendant
  if (t < 8100 || t > 9900) return -1;          // Controle de timing       9000us du protocole NEC (-/+10%)
  return 0;                                     // Entête correcte
}

int16_t AcquerirOctetNEC(int Broche) { // AcquerirOctetNEC : fonction d'acquisition
  uint8_t octet = 0;
  for (uint8_t i = 7; i >= 0; i--) {     // boucle de construction de l'octet NEC
    int32_t t = AcquerirFrontNEC(Broche, false);    // Acquisition du front descendant
        if (t < 281 || t > 843) return -1;          // Erreur de timing
        t = AcquerirFrontNEC(Broche, true);         // Acquisition du front montant
        int8_t bit = (t > 1405 && t < 1967) ? 1 : (t > 281 && t < 843) ? 0 : -1; // Bit 1, 0 ou erreur de timing
        if (bit < 0) return -1;                     // Erreur de timing
        octet |= bit << i;                          // Construction de l'octet NEC
  }
  return octet;
}

int32_t AcquerirFrontNEC(int Broche, bool front) { // AcquerirFrontNEC: fonction d'acquisition
  uint32_t t = micros();                         // Mesure du temps de front
  while (AcquerirInfrarouge(Broche) != front) {  // Attente du front (montant: 1, descendant: 0)
      if (micros() - t >= 500000)                // Test de timing
          return -2;                             // Trame absente
  }
  int32_t dt = micros() - t;                     // Mesure du temps de front
  return (dt < 200) ? -1 : dt;                   // Controle de timing
}

bool AcquerirInfrarouge(int Broche) {
  bool out = 0;
  if (Broche >= 0 && Broche <= 7)
    out = PIND & (1 << Broche) ? 0 : 1;
  if (Broche >= 8 && Broche <= 13)
    out = PINB & (1 << (Broche - 8)) ? 0 : 1;
  if (Broche >= A0 && Broche <= A5)
    out = PINC & (1 << (Broche - A0)) ? 0 : 1;
  return out;
}
