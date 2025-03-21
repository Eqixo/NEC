/*************************************************************************************************************************************/
// Sujet :   Definition de la bibliotheque de fonctions NEC                                                                          //
// Auteurs :  AUGEREAU F. JOBARD L.                                                                                                  //
// Date :    14/03/2025                                                                                                              //
// Version : 1.5.0                                                                                                                   //
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
#include <arduino.h>
#include "NEC.h"

// definition des fonctions d'emission de la bibliotheque NEC
void GenererTrameNEC(int Broche, uint8_t Adresse, uint8_t Donnee)   // GenererTrameNEC : fonction d'action
{
  GenererEnteteNEC(Broche);               // generation du header NEC
  GenererOctetNEC(Broche, Adresse);       // generation de l'adresse NEC
  GenererOctetNEC(Broche, ~Adresse);      // generation de l'adresse_barre NEC
  GenererOctetNEC(Broche, Donnee);        // generation de la donnee NEC
  GenererOctetNEC(Broche, ~Donnee);       // generation de la donnee_barre NEC
  GenererBurst562usNEC(Broche);           // burst final d'une trame NEC
  delayMicroseconds(40500);               // delai inter-trame de 40,5ms
}

void GenererEnteteNEC(int Broche)                                   // GenererEnteteNEC : fonction d'action
{
  GenererBurst9000usNEC(Broche);
  delayMicroseconds(4500);                // temps a l'etat bas apres entete du protocole NEC
}


void GenererBurst9000usNEC(int Broche)                              // GenererBurst9000usNEC : fonction d'action
{
  register uint16_t i = 0;
  do {
    GenererImpulsion38kHzNEC(Broche);
  } while (i++ < 342);                    // impulsions du protocole NEC pendant 9000 us
}

void GenererOctetNEC(int Broche, uint8_t Octet)               // GenererOctetNEC : fonction d'action
{
  uint8_t mask = 0x80;                  // masque de 8 bits
  do {                                  // boucle de construction de l'octet NEC
    if (Octet & mask)                   // test du bit
      GenererBit1NEC(Broche);           // generation du bit à l'état 1
    else
      GenererBit0NEC(Broche);           // generation du bit à l'état 0
    mask >>= 1;                         // decalage du masque
  } while (mask != 0);                  // tant que le masque n'est pas nul
}

void GenererBit0NEC(int Broche)                                     // GenererBit0NEC : fonction d'action
{
  GenererBurst562usNEC(Broche);
  delayMicroseconds(562);           // temps a l'etat bas pour un bit 0 du protocole NEC
}

void GenererBit1NEC(int Broche)                                     // GenererBit1NEC : fonction d'action
{
  GenererBurst562usNEC(Broche);
  delayMicroseconds(1687);          // temps a l'etat bas pour un bit 1 du protocole NEC
}

void GenererBurst562usNEC(int Broche)                               // GenererBurst562usNEC : fonction d'action
{
  register uint8_t i = 0;
  do {
    GenererImpulsion38kHzNEC(Broche);
  } while (i++ < 22);               // 22 impulsions du protocole NEC
}

void GenererImpulsion38kHzNEC(int Broche)                           // GenererImpulsion38kHzNEC : fonction d'action
{
  register uint8_t i = 0;
  // on allume la diode infrarouge
  AllumerDiodeInfrarouge(Broche);
  // nous utilisons une boucle en assembleur pour attendre environ 8.8 us
  // nous utilisons l'instruction "sbis" (skip if bit in I/O register set) pour
  // attendre que le registre de 8 bits passe de la valeur 35 a la valeur 0
  // ce qui prend environ 8.8 us (le quartz fonctionne a 16 MHz)
  // nous utilisons l'instruction "brne" (branch if not equal) pour reboucler
  // sur la premiere instruction de la boucle tant que le registre n'est pas a 0
  __asm__ __volatile__ (
    "1: sbiw %0, 1\n\t"       // decremente le registre de 8 bits (2 clk cycles par itération)
    "brne 1b"                 // si le registre n'est pas a 0, reboucle sur la premiere instruction de la boucle (2 clk cycles par itération)
    : "=w" (i)                // le registre de 8 bits est stocke dans la variable i
    : "0" (35)                // le registre de 8 bits est initialisé a 35
  );                          // temps total = (35 itérations * 4 clk cycles)/(16 * 10^6 Hz) = 8.75 us
  // on eteint la diode infrarouge
  EteindreDiodeInfrarouge(Broche);
  // nous utilisons une autre boucle en assembleur pour attendre environ 17.5 us
  __asm__ __volatile__ (
    "1: sbiw %0, 1\n\t"       // decremente le registre de 8 bits
    "brne 1b"                 // si le registre n'est pas a 0, reboucle sur la premiere instruction de la boucle
    : "=w" (i)                // le registre de 8 bits est stocke dans la variable i
    : "0" (70)                // le registre de 8 bits est initialisé a 70
  );                          // temps total = (70 itérations * 4 clk cycles)/(16 * 10^6 Hz) = 17.5 us
}

inline void AllumerDiodeInfrarouge(int Broche)                             // AllumerDiodeInfrarouge : fonction d'action
{
  if (Broche >= 0 && Broche <= 7) {
    PORTD |= (1 << Broche);
  } else if (Broche >= 8 && Broche <= 13) {
    PORTB |= (1 << (Broche - 8));
  } else if (Broche >= A0 && Broche <= A5) {
    PORTC |= (1 << (Broche - A0));
  }
}

inline void EteindreDiodeInfrarouge(int Broche)                            // EteindreDiodeInfrarouge : fonction d'action
{
  if (Broche >= 0 && Broche <= 7) {
    PORTD &= ~(1 << Broche);  
  } else if (Broche >= 8 && Broche <= 13) {
    PORTB &= ~(1 << (Broche - 8));
  } else if (Broche >= A0 && Broche <= A5) {
    PORTC &= ~(1 << (Broche - A0));
  }
}


// definition des fonctions de reception de la bibliotheque NEC
int8_t AcquerirTrameNEC(int Broche, uint8_t* ptr_Adresse, uint8_t* ptr_Donnee) // AcquerirTrameNEC : fonction d'acquisition
{
  int8_t entete;
  int16_t adresse, adresse_barre, donnee, donnee_barre;
  int32_t temps;
  *ptr_Adresse = 0x00;                                                         // Initialisation de l'adresse
  *ptr_Donnee = 0x00;                                                          // Initialisation de la donnee
  entete = AcquerirEnteteNEC(Broche);                                          // Acquisition de l'entete
  if (entete < 0) return entete;                                               // Erreur de timing
  adresse = AcquerirOctetNEC(Broche);                                          // Acquisition de l'adresse
  if (adresse < 0) return adresse;                                             // Erreur de timing
  adresse_barre = ~adresse;                                                    // Inversion de l'adresse
  if (AcquerirOctetNEC(Broche) != adresse_barre) return -1;                    // Erreur de timing
  donnee = AcquerirOctetNEC(Broche);                                           // Acquisition de la donnee
  if (donnee < 0) return donnee;                                               // Erreur de timing
  donnee_barre = ~donnee;                                                      // Inversion de la donnee
  if (AcquerirOctetNEC(Broche) != donnee_barre) return -1;                     // Erreur de timing
  if ((temps = AcquerirFrontDescendantNEC(Broche)) < 0) return temps;          // Erreur de timing
  *ptr_Adresse = adresse;                                                      // Stockage de l'adresse
  *ptr_Donnee = donnee;                                                        // Stockage de la donnee
  return 0;                                                                    // Trame correcte
}

int8_t AcquerirEnteteNEC(int Broche)                                   // AcquerirEnteteNEC : fonction d'acquisition
{
  int32_t temps;
  if ((temps = AcquerirFrontMontantNEC(Broche)) < 0)    // Acquisition du front montant
    return temps;                                       // Erreur de timing
  if ((temps = AcquerirFrontDescendantNEC(Broche)) < 0) // Acquisition du front descendant
    return temps;                                       // Erreur de timing
  if((temps < 8100) || (temps > 9900))                  // Controle de timing       9000us du protocole NEC (-/+10%)
    return -1;                                          // Erreur de timing
  temps = AcquerirFrontMontantNEC(Broche);              // Acquisition du front montant
  if((temps < 4050) || (temps > 4950))                  // Controle de timing       4500us du protocole NEC (-/+10%)
    return -1;                                          // Erreur de timing
  return 0;                                             // Entête correcte
}

int16_t AcquerirOctetNEC(int Broche)                                    // AcquerirOctetNEC : fonction d'acquisition
{
  int16_t octet = 0;
  for (int8_t i = 7; i >= 0; i--) {                     // boucle de construction de l'octet NEC
    int8_t bit = AcquerirBitNEC(Broche);                // Acquisition bit NEC
    if (bit < 0)                                        // Controle de timing
      return bit;                                       // Erreur de timing
    octet |= bit << i;                                  // Construction de l'octet NEC
  }
  return octet;
}

int8_t AcquerirBitNEC(int Broche)                                       // AcquerirBitNEC : fonction d'acquisition
{
  int32_t Temps = AcquerirFrontDescendantNEC(Broche);   // Acquisition du front descendant
  if (Temps < 281 || Temps > 843)                       // Controle de timing
    return (Temps < 0) ? Temps : -1;                    // Erreur de timing

  Temps = AcquerirFrontMontantNEC(Broche);              // Acquisition du front montant
  if (Temps > 281 && Temps < 843)                       // Controle de timing
    return 0;                                           // Bit 0
  if (Temps > 1405)                                     // Controle de timing
    return (Temps < 1967) ? 1 : -1;                     // Bit 1
  return -1;                                            // Sinon erreur de timing
}

int32_t AcquerirFrontMontantNEC(int Broche)                              // AcquerirFrontMontantNEC : fonction d'acquisition
{
  uint32_t TempsFrontPrecedent = micros();              // Mesure du temps de front montant
  while (micros() - TempsFrontPrecedent < 500000)       // Attente du front montant
  {
    if (AcquerirInfrarouge(Broche) == 1)                // Detection du front montant
    {
      uint8_t Temps = micros() - TempsFrontPrecedent;   // Mesure du temps de front montant
      return (Temps < 200) ? -1 : (int32_t)Temps;       // Controle de timing
    }
  }
  return -2;                                            // Trame absente
}


int32_t AcquerirFrontDescendantNEC(int Broche)                           // AcquerirFrontDescendantNEC : fonction d'acquisition
{
  uint32_t Temps = micros();
  while (AcquerirInfrarouge(Broche) && (micros() - Temps < 500000));
  int32_t DeltaTemps = micros() - Temps;
  return DeltaTemps < 200 ? -1 : DeltaTemps;
}

uint8_t AcquerirInfrarouge(int Broche)
{
  uint8_t val = 0;
  if (Broche >= 0 && Broche <= 7) {
    val = PIND & (1 << Broche) ? 0 : 1;
  } else if (Broche >= 8 && Broche <= 13) {
    val = PINB & (1 << (Broche - 8)) ? 0 : 1;
  } else if (Broche >= A0 && Broche <= A5) {
    val = PINC & (1 << (Broche - A0)) ? 0 : 1;
  }
  return val;
}
