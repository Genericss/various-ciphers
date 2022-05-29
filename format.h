#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define ALPHASIZE 26

int gcd(int a, int b){
    if(b == 0){
        return a;
    }
    return gcd(b, a % b);
}

// converts to uppercase and removes spaces
char* plaintext(char* message){

    //finds the length needed for only plaintext characters
    int len = 0;
    for(int i = 0; message[i] != '\0'; i++){
        if((message[i] >= 'A' && message[i] <= 'Z')||(message[i] >= 'a' && message[i] <= 'z')){
            len++;
        }
    }

    char* plain = (char*)malloc((len + 1) * sizeof(char));


    int mIndex = 0, pIndex = 0;
    const int offset = ('a' - 'A');

    //fills the plaintext array
    while(message[mIndex] != '\0'){
        if(message[mIndex] >= 'A' && message[mIndex] <= 'Z'){
            plain[pIndex] = message[mIndex];
            pIndex++;
        }else if(message[mIndex] >= 'a' && message[mIndex] <= 'z'){
            plain[pIndex] = message[mIndex] - offset;
            pIndex++;
        }
        mIndex++;
    }
    plain[pIndex++] = '\0';

    return plain;
}

// blocks messages into chunks of size "chunk"
char* block(char* message, int chunk){
    int mLen = strlen(message);
    char* bMessage = (char*)malloc( (mLen + mLen/chunk + 1) * sizeof(char) );

    int bIndex = 0, mIndex = 0;

    while(message[mIndex] != '\0'){
        
        //after "chunk" charactesr have been filled, insert a space
        if(mIndex != 0 && mIndex % chunk == 0){
            bMessage[bIndex++] = ' ';
        }
        bMessage[bIndex++] = message[mIndex++];
        
    }

    bMessage[bIndex] = '\0';
    return bMessage;
}

//shifts the message by "key" letters
char* caesarCipher(char* message, int key){
    char* ptext = plaintext(message);
    char* encodedMessage = (char*)malloc((strlen(message)+1) * sizeof(char));
    key = key % ALPHASIZE; //just in case
    int i;
    for(i = 0; ptext[i] != '\0'; i++){
        if(ptext[i] >= 'A' && ptext[i] <= 'Z'){
            encodedMessage[i] = ((ptext[i] + key) - 'A')%ALPHASIZE + 'A';
        }else{
            encodedMessage[i] = ptext[i];
        }
    }
    encodedMessage[i] = '\0';
    return encodedMessage;
}

//takes a message as input and returns a plaintext coded message
char* vigenereEncode(char* message, char* keyword){
    char* ptext = plaintext(message);
    char* pkey = plaintext(keyword);
    char* key = (char*)malloc((strlen(ptext) + 1) * sizeof(char));
    int i;

    for(i = 0; ptext[i] != '\0'; i++){
        key[i] = pkey[i%(strlen(pkey))];
    }

    key[i] = '\0';

    char* encodedMessage = (char*)malloc((strlen(ptext) + 1) * sizeof(char));

    int j;
    for(j = 0; ptext[j] != '\0'; j++){
        //hey if it messes up why fix it. if this breaks, add %alphasize to (key[j] - 'A') to make it (key[j] - 'A')%ALPHASIZE
        encodedMessage[j] = (ptext[j] + (key[j] - 'A') - 'A')%ALPHASIZE + 'A';
    }
    encodedMessage[j] = '\0';

    free(ptext);
    free(pkey);
    free(key);

    return encodedMessage;
}

//takes a message as input and returns a plaintext decoded message
char* vigenereDecode(char* message, char* keyword){
    char* ptext = plaintext(message);
    char* pkey = plaintext(keyword);
    char* key = (char*)malloc((strlen(ptext) + 1) * sizeof(char));
    int i;

    for(i = 0; ptext[i] != '\0'; i++){
        key[i] = pkey[i%(strlen(pkey))];
    }
    key[i] = '\0';

    char* decodedMessage = (char*)malloc((strlen(ptext) + 1) * sizeof(char));

    int j;
    int t;
    //reasonably confident there is a better way somehow
    for(j = 0; ptext[j] != '\0'; j++){
        t = ((ptext[j] - key[j])%ALPHASIZE < 0 ) ? ('Z' - key[j] + ptext[j] - 'A' + 1)%ALPHASIZE : (ptext[j] - key[j])%ALPHASIZE;
        decodedMessage[j] = 'A' + t;
    }
    decodedMessage[j] = '\0';

    free(ptext);
    free(pkey);
    free(key);
    return decodedMessage;
}

//this is by far the function i am the proudest of (doesn't really work well for small messages)
char* vigenereCrack(char* message){
    char* ptext = plaintext(message);
    //runs through each possible length of the key and divides the message into keyLength columns
    int len = strlen(ptext), keyLen = 0, secondKeyLen = 0;
    long double DIC, maxDIC = 0, secondMaxDIC = 0; //delta index of coincidence
    //assume keyLength is at least 2, since a single letter keylength is just a monoalphabetical cipher, which can be hacked at with frequency analysis
    for(int keyLength = 2; keyLength <= len;  keyLength++){
        //for each column, calculate the delta Index of Coincidence and then take the aggregate IC
        DIC = 0;
        //runs through each column?
        //will probably need to rename column at some point in my life
        for(int column = 0; column < keyLength; column++){
            double freq[ALPHASIZE] = {0};

            for(int letter = 0; letter + column < len; letter += keyLength){
                //if i had to guess, the nan shows up once they get into single columns
                freq[ptext[column + letter] - 'A']++;
            }

            //i am actually quite proud of this bit
            int extension = (column < len % keyLength) ? 1 : 0;
            int columnSize = len / keyLength + extension;
            //calculating the delta index of coincidence for each column, and then adding to create the aggregate
            long double colDIC = 0;
            for(int i = 0; i < ALPHASIZE; i++){
                colDIC += freq[i] * (freq[i] - 1);
            }
            //i don't really believe i understand the math, since this may lead to division by zero
            int size = (columnSize * (columnSize - 1) == 0 )? 1 : (columnSize * (columnSize - 1));
            colDIC /= size;
            DIC += colDIC;
        }

        //this will output either the keylength or a multiple of the keylength
        //perhaps to find the keyLength, i get the second highest and pull a gcd on it
        if(DIC > maxDIC){
            secondKeyLen = keyLen;
            secondMaxDIC = maxDIC;

            keyLen = keyLength;
            maxDIC = DIC;
        }else if(DIC > secondMaxDIC){
            secondKeyLen = keyLength;
            secondMaxDIC = DIC;
        }
    }

    //should be better than before, but not the best...
    keyLen = (gcd(keyLen, secondKeyLen) == 1) ? keyLen : gcd(keyLen, secondKeyLen);

    //english letter frequencies
    long double engFreq[] = {0.081165, 0.015645, 0.025837, 0.042943, 0.126906, 0.022181, 0.019891, 0.067081, 0.070046, 0.001910, 0.006432, 0.039801, 0.025678, 0.068158, 0.074639, 0.016572, 0.001028, 0.058526, 0.063412, 0.090909, 0.027783, 0.010016, 0.023100, 0.001583, 0.018069, 0.000687};
    char* key = (char*)malloc((keyLen + 1) * sizeof(char));

    //for each letter of the key
    int kLetter;
    for(kLetter = 0; kLetter < keyLen; kLetter++){
        //for each column of the thingy
        double maxCorrelation = 0;
        int l;

        //calculates the frequency of the letters in each column and the corresponding correlation
        double colFreq[ALPHASIZE] = {0};
        double correlation = 0; 

        for(int cLetter = 0; cLetter + kLetter < len; cLetter += keyLen){
            colFreq[ (ptext[cLetter + kLetter] - 'A') % ALPHASIZE]++;
        }
        int extension = (kLetter < len % keyLen) ? 1 : 0;
        int columnSize = len / keyLen + extension;
        for(int i = 0; i < ALPHASIZE; i++){
            colFreq[i] /= columnSize; //reasonablly confident about this part
        }

        //the above code generates the frequency distribution for the column. 
        //This code should caesar shift the column and find the shift that gives the highest correlation
        for(int alpha = 0; alpha < ALPHASIZE; alpha++){
            correlation = 0;
            for(int i = 0; i < ALPHASIZE; i++){
                correlation += engFreq[i] * colFreq[(alpha + i) %ALPHASIZE];
            }
            if(correlation > maxCorrelation){
                maxCorrelation = correlation;
                l = alpha;
            }
        }
        key[kLetter] = 'A' + l;
    }
    key[kLetter] = '\0';
    printf("Keyword is %s\n", key);

    char* cracked = vigenereDecode(ptext, key);
    free(key);
    free(ptext);
    return cracked;
}

//takes a message as input and returns a decoded message
char* frequencyAnalysis(char* message){
    char * decodedMessage = (char*)malloc((strlen(message)+1) * sizeof(char));
    //obtained from hacking a bunch of books together and analyzing the frequency.
    double letterFrequency[26] = {0.081165, 0.015645, 0.025837, 0.042943, 0.126906, 0.022181, 0.019891, 0.067081, 0.070046, 0.001910, 0.006432, 0.039801, 0.025678, 0.068158, 0.074639, 0.016572, 0.001028, 0.058526, 0.063412, 0.090909, 0.027783, 0.010016, 0.023100, 0.001583, 0.018069, 0.000687};


    //finding the key shift
    int key;
    double score = 0, maxScore = 0;
    for(int i = 0; i < ALPHASIZE; i++){
        score = 0;
        for(int j = 0; message[j] != '\0'; j++){
            if(message[j] >= 'A' && message[j] <= 'Z'){
                score += letterFrequency[ (message[j] + i - 'A')%ALPHASIZE ];
            // }else if(message[j] >= 'a' && message[j] <= 'z'){
            //     score += letterFrequency[ (message[j] + i - 'a')%ALPHASIZE ];
            }
        }
        if(score > maxScore){
            key = i;
            maxScore = score;
        }
    }

    //decoding it
    int j;
    for(j = 0; message[j] != '\0'; j++){
        if(message[j] >= 'A' && message[j] <= 'Z'){
            decodedMessage[j] = (message[j] + key - 'A')%ALPHASIZE + 'A';
        // }else if(message[j] >= 'a' && message[j] <= 'z'){
        //     decodedMessage[j] = (message[j] + key - 'a')%ALPHASIZE + 'a';
        }else{
            decodedMessage[j] = message[j];
        }
    }
    decodedMessage[j] = '\0';

    return decodedMessage;
}

//encodes a rail cipher
char* railEncode(char* message, int numLines){
    char* ptext = plaintext(message);
    int len = strlen(ptext);
    char* encoded = (char*)malloc((len + 1) * sizeof(char));
    int index = 0;
    for(int i = 0; i < numLines; i++){
        for(int j = i; j < len; j += numLines){
            encoded[index++] = ptext[j];
        }
    }
    encoded[index] = '\0';
    free(ptext);
    return encoded;
}

//decodes the rail cipher
char* railDecode(char* message, int numLines){
    char* ptext = plaintext(message);
    int len = strlen(ptext);
    char* decoded = (char*)malloc((len + 1) * sizeof(char));

    int divisions = len / numLines;
    int rem = len % numLines;
    int index = 0;

    for(int i = 0; i < divisions; i++){
        for(int j = 0; j < numLines; j++){
            decoded[index++] = ptext[i + j*(divisions+1)];
        }
    }

    for(int i = 0; i < rem; i++){
        decoded[index++] = ptext[divisions + i * divisions];
    }
    decoded[index] = '\0';

    free(ptext);
    return decoded;
}

//honestly i didn't put much effort into this one
char* railCrack(char* message){
    char* ptext = plaintext(message);
    int len = strlen(ptext);
    for(int i = 2; i < len / 2; i++){
        printf("%s\n", railDecode(ptext, i));
    }
    free(ptext);
    return "";
}

