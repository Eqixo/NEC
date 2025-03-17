/*************************************************************************************************************************************/
// Sujet :   Definition de la bibliotheque de fonctions NEC                                                                          //
// Auteur :  AUGEREAU F.                                                                                                             //
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

// declaration des fonctions de la bibliotheque NEC
void GenererTrameNEC(int Broche, uint8_t Adresse, uint8_t Donnee);                // fonction d'émission d'une trame NEC
void GenererEnteteNEC(int Broche);                                                // sous-fonction d'émission d'une trame NEC utilisee par la fonction GenererTrameNEC
void GenererBurst9000usNEC(int Broche);                                           // sous-fonction d'émission d'une trame NEC utilisee par la fonction GenererTrameNEC
void GenererOctetNEC(int Broche, uint8_t Octet);                                  // sous-fonction d'émission d'une trame NEC utilisee par la fonction GenererTrameNEC
void GenererBit0NEC(int Broche);                                                  // sous-fonction d'émission d'une trame NEC utilisee par la fonction GenererTrameNEC
void GenererBit1NEC(int Broche);                                                  // sous-fonction d'émission d'une trame NEC utilisee par la fonction GenererTrameNEC
void GenererBurst562usNEC(int Broche);                                            // sous-fonction d'émission d'une trame NEC utilisee par la fonction GenererTrameNEC
void GenererImpulsion38kHzNEC(int Broche);                                        // sous-fonction d'émission d'une trame NEC utilisee par la fonction GenererTrameNEC
void AllumerDiodeInfrarouge(int Broche);                                          // sous-fonction d'émission d'une trame NEC utilisee par la fonction GenererTrameNEC
void EteindreDiodeInfrarouge(int Broche);                                         // sous-fonction d'émission d'une trame NEC utilisee par la fonction GenererTrameNEC
int8_t AcquerirTrameNEC(int Broche, uint8_t* Adresse, uint8_t* Donnee);           // fonction d'acquisition d'une trame NEC
int8_t AcquerirEnteteNEC(int Broche);                                             // sous-fonction d'acquisition d'une trame NEC utilisee par la fonction AcquerirTrameNEC
int16_t AcquerirOctetNEC(int Broche);                                             // sous-fonction d'acquisition d'une trame NEC utilisee par la fonction AcquerirTrameNEC
int8_t AcquerirBitNEC(int Broche);                                                // sous-fonction d'acquisition d'une trame NEC utilisee par la fonction AcquerirTrameNEC
int32_t AcquerirFrontMontantNEC(int Broche);                                      // sous-fonction d'acquisition d'une trame NEC utilisee par la fonction AcquerirTrameNEC
int32_t AcquerirFrontDescendantNEC(int Broche);                                   // sous-fonction d'acquisition d'une trame NEC utilisee par la fonction AcquerirTrameNEC
uint8_t AcquerirInfrarouge(int Broche);                                           // sous-fonction d'acquisition d'une trame NEC utilisee par la fonction AcquerirTrameNEC
