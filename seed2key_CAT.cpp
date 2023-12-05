#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <cstring>
#include <vector>

int seed2key_CAT(char *rawString, std::vector<uint8_t> &outputKey);
unsigned int key_CAT(const std::vector<char> &seedValues, int seedCount, std::vector<uint8_t> &outputKey, unsigned int &keyLength);

int seed2key_CAT(char *rawString, std::vector<uint8_t> &outputKey) {
    unsigned int seedCount = 0;
    char *token;
    std::vector<char> seedValues(20);

    token = strtok(rawString, " \t\n");
    while (token && seedCount < 20) {
        seedValues[seedCount] = static_cast<char>(atoi(token));
        seedCount++;
        token = strtok(NULL, " \t\n");
    }

    unsigned int keyLength;
    key_CAT(seedValues, seedCount, outputKey, keyLength);
    return keyLength;
}

unsigned int key_CAT(const std::vector<char> &seedValues, int seedCount, std::vector<uint8_t> &outputKey, unsigned int &keyLength) {
    unsigned int processStatus = 0;
    unsigned char xorKey[] = {0xBD, 0x5E, 0x3E, 0x73};

    outputKey.resize(20);

    if (seedCount == 5) {
        for (processStatus = 0; processStatus < 4; ++processStatus) {
            outputKey[processStatus] = static_cast<uint8_t>(seedValues[processStatus + 1]);
        }
		
        for (int i = 35; i > 0; --i) {
            char shiftedRightBit = 0;
            for (int j = 3; j >= 0; --j) {
                uint8_t currentByte = outputKey[j];
                bool isShiftedBitZero = shiftedRightBit == 0;
                shiftedRightBit = currentByte >> 7;
                outputKey[j + 1] = isShiftedBitZero ? 2 * currentByte : 2 * currentByte + 1;
            }
			
            if (shiftedRightBit) {
                for (int k = 0; k < 4; ++k) {
                    outputKey[k] ^= xorKey[k];
                }
            }
        }

        keyLength = 4;
        return 1;
    }

    return processStatus;
}

int main() {
    char testString[] = "234 734 123 876 263";
    std::vector<uint8_t> outputKey;
    int keyLength = seed2key_CAT(testString, outputKey);

    std::cout << "Output Key (Hex): ";
    for (size_t i = 0; i < keyLength; ++i) {
        std::cout << "0x" << std::hex << static_cast<int>(outputKey[i]);
        if (i < keyLength - 1) std::cout << ", ";
    }
    std::cout << "\nKey Length (Decimal): " << std::dec << keyLength << std::endl;

    return 0;
}

