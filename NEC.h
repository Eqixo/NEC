/*************************************************************************************************************************************/
// Sujet :   Definition de la bibliotheque de fonctions NEC                                                                          //
// Auteurs :  AUGEREAU F. JOBARD L.                                                                                                  //
// Date :    14/03/2025                                                                                                              //
// Version : 1.5.3                                                                                                                   //
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

// declaration des fonctions de la bibliotheque NEC
void asmDelay(uint32_t cycles);                                                   // sous-fonction d'une trame NEC utilisee par la fonction GenererTrameNEC & AcquerirTrameNEC
void GenererTrameNEC(int Broche, uint8_t Adresse, uint8_t Donnee);                // fonction d'émission d'une trame NEC
void GenererBurstNEC(int Broche, uint16_t pulses);                                // sous-fonction d'émission d'une trame NEC utilisee par la fonction GenererTrameNEC
void GenererImpulsion38kHzNEC(int Broche);                                        // sous-fonction d'émission d'une trame NEC utilisee par la fonction GenererTrameNEC
void controlerDiodeIR(int Broche, bool state);                                    // sous-fonction d'émission d'une trame NEC utilisee par la fonction GenererTrameNEC
int8_t AcquerirTrameNEC(int Broche, uint8_t* Adresse, uint8_t* Donnee);           // fonction d'acquisition d'une trame NEC
int8_t AcquerirEnteteNEC(int Broche);                                             // sous-fonction d'acquisition d'une trame NEC utilisee par la fonction AcquerirTrameNEC
int16_t AcquerirOctetNEC(int Broche);                                             // sous-fonction d'acquisition d'une trame NEC utilisee par la fonction AcquerirTrameNEC
int32_t AcquerirFrontNEC(int Broche, bool front);                                 // sous-fonction d'acquisition d'une trame NEC utilisee par la fonction AcquerirTrameNEC
bool AcquerirInfrarouge(int Broche);                                              // sous-fonction d'acquisition d'une trame NEC utilisee par la fonction AcquerirTrameNEC
