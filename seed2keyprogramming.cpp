#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <cstring>
#include <vector>

int seed2keyprogramming(char *rawString, std::vector<uint8_t> &outputKey);
unsigned int key_programming(const std::vector<char>& seedValues, int seedCount, std::vector<uint8_t> &outputKey, unsigned int &keyLength);
int LCGRandom(int seed);

int seed2keyprogramming(char *rawString, std::vector<uint8_t> &outputKey) {
    
    unsigned int seedCount = 0;
    char *token;
    std::vector<char> seedValues(20);
	unsigned int keyLength;

    token = strtok(rawString, " \t\n");
    while (token && seedCount < 20) {
        seedValues[seedCount] = static_cast<char>(atoi(token));
        seedCount++;
        token = strtok(NULL, " \t\n");
    }
    
    key_programming(seedValues, seedCount, outputKey, keyLength);
    return keyLength;
}

unsigned int key_programming(const std::vector<char>& seedValues, int seedCount, std::vector<uint8_t> &outputKey, unsigned int &keyLength) {
    if (seedCount == 5) {
		int processedValue = LCGRandom(seedValues[4] + (seedValues[3] << 8) + (seedValues[2] << 16) + (seedValues[1] << 24));
        outputKey.resize(4);
		outputKey[0] = static_cast<uint8_t>(processedValue >> 24);
		outputKey[1] = static_cast<uint8_t>(processedValue >> 16);
		outputKey[2] = static_cast<uint8_t>(processedValue >> 8); 
		outputKey[3] = static_cast<uint8_t>(processedValue);
        keyLength = 4;
    } else {
        keyLength = 0;
    }
    return 1;
}

int LCGRandom(int seed) {
    return ~(0x41C64E6D * seed + 0x3039);
}

int main() {
    char testString[] = "127 223 123 254 145"; 
    std::vector<uint8_t> outputKey;

    int keyLength = seed2keyprogramming(testString, outputKey);

    std::cout << "Output Key (Hex): ";
    for (int i = 0; i < keyLength; ++i) {
        std::cout << "0x" << std::hex << static_cast<unsigned int>(outputKey[i]);
        if (i < keyLength - 1) std::cout << ", ";
    }

    std::cout << "\nKey Length (Decimal): " << std::dec << keyLength << std::endl;

    return 0;
}
