#include "format.h"

int main(){
    printf("This is a program designed to encrypt and decrypt ciphers.");
    bool loop = true;
    char choice;

    do{
        printf("\nWould you like to decode a message (D), encode a message (E), crack the code (C), or exit the program? (X)\n");
        printf("Enter your choice (D/E/C/X): ");
        scanf(" %c", &choice);

        if(choice == 'D' || choice == 'E' || choice == 'C'){
            const int size = 1024;
            const int blockSize = 5;
            char message[size];
            printf("Enter a message: "); 
            getchar();
            gets(message);
            char* formatText = block(plaintext(message), blockSize);
            
            if(choice == 'E'){
                char eChoice;
                bool eLoop; 
                char* encoded;
                do{
                    eLoop = false;
                    printf("Caesar, Vigenere, or Rail? (C/V/R): ");
                    scanf("%c", &eChoice);
                    if(eChoice == 'C'){
                        int key;
                        printf("Enter the key: ");
                        scanf("%d", &key);
                        encoded = caesarCipher(formatText, key);
                    }else if(eChoice == 'V'){
                        printf("Enter the keyword: ");
                        //message is no longer used
                        getchar();
                        gets(message);

                        char* formatKey = plaintext(message);
                        encoded = vigenereEncode(formatText, formatKey);
                        free(formatKey);
                    }else if(eChoice == 'R'){
                        int lines;
                        printf("Enter the number of lines: ");
                        scanf("%d", &lines);
                        encoded = railEncode(formatText, lines);
                    }
                    else{
                        printf("Invalid.\n");
                        eLoop = true;
                    }
                }while(eLoop);
                encoded = block(encoded, blockSize);
                printf("The encoded message is: %s\n", encoded);
                free(encoded);
            }else if(choice == 'D'){
                char dChoice;
                bool dLoop;
                char* decoded;
                do{
                    dLoop = false;
                    printf("Caesar, Vigenere, or Rail? (C/V/R): ");
                    scanf("%c", &dChoice);
                    if(dChoice == 'S'){
                        int key;
                        printf("Enter the key: ");
                        scanf("%d", &key);
                        decoded = caesarCipher(formatText, -key);

                    }else if(dChoice == 'V'){
                        printf("Enter the keyword: ");
                        getchar();
                        gets(message);

                        //this is not working
                        char* formatKey = plaintext(message);
                        decoded = vigenereDecode(formatText, formatKey), blockSize;
                        free(formatKey);
                    }else if(dChoice == 'R'){
                        int lines;
                        printf("Enter the number of lines: ");
                        scanf("%d", &lines);
                        decoded = railDecode(formatText, lines);
                    }else{
                        printf("Invalid.\n");
                        dLoop = true;
                    }
                }while(dLoop);

                decoded = block(decoded, blockSize);

                printf("The decoded message is: %s\n", decoded);
                free(decoded);
            }else if(choice == 'C'){
                char cChoice;
                bool cLoop;
                char* cracked;
                do{
                    cLoop = false;
                    printf("Caesar cipher or Vigenere cipher? (C/V): ");
                    scanf("%c", &cChoice);
                    if(cChoice == 'C'){
                        cracked = frequencyAnalysis(formatText);
                    }else if(cChoice == 'V'){
                        cracked = vigenereCrack(formatText);
                    }else if(cChoice == 'R'){
                        cracked = railCrack(formatText);
                    }else{
                        printf("Invalid.\n");
                        cLoop = true;
                    }
                }while(cLoop);

                cracked = block(cracked, blockSize);
                printf("The cracked message is: %s\n", cracked);
                free(cracked);
            }
            free(formatText);
        }else if(choice == 'X'){
            loop = false;
        }else{
            printf("Invalid.\n");
        }

    }while(loop == true);

    return 0;
}
