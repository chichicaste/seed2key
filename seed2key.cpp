#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>

class KeyGenerator {
private:
    static int LCGRandom(int seed) {
        return ~(0x41C64E6D * seed + 0x3039);
    }

    static std::pair<unsigned int, std::vector<char>> tokenize(char *rawString) {
        unsigned int seedCount = 0;
        std::string str(rawString); // Create a local copy of the rawString
        char *token;
        char *strtokState;
        std::vector<char> seedValues(20);

        token = strtok_s(&str[0], " \t\n", &strtokState); // Operate on the local copy (strtok_s is not standard only on Windows, replace with strtok_r if needed)
        while (token && seedCount < 20) {
            seedValues[seedCount] = static_cast<char>(atoi(token));
            seedCount++;
            token = strtok_s(NULL, " \t\n", &strtokState);
        }

        return std::make_pair(seedCount, seedValues);
    }

    static unsigned int unifiedKey(const std::vector<char>& seedValues, int seedCount, std::vector<uint8_t> &outputKey, unsigned int &keyLength, unsigned char xorKey[], bool programming) {
        if (programming) {
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
                return 0;
            }
            return 1;
        } else {
            if (xorKey == NULL) {
                keyLength = 0;
                return 0;
            }
            else {
                unsigned int processStatus = 0;
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
                }

                return processStatus;
            }
        }
        return 0;
    }
public:
    static int seed2key(char *rawString, std::vector<uint8_t> &outputKey, unsigned char xorKey[], bool programming) {
        std::pair<unsigned int, std::vector<char>> tokenizedString = tokenize(rawString);
        unsigned int keyLength;
        unifiedKey(tokenizedString.second, tokenizedString.first, outputKey, keyLength, xorKey, programming);
        return keyLength;
    }
};

void printKey(const std::vector<uint8_t>& outputKey, unsigned int keyLength) {
    std::cout << "Output Key (Hex): ";
    for (size_t i = 0; i < keyLength; ++i) {
        std::cout << "0x" << std::hex << static_cast<int>(outputKey[i]);
        if (i < keyLength - 1) std::cout << ", ";
    }
    std::cout << "\nKey Length (Decimal): " << std::dec << keyLength << std::endl; 
}

// Ejemplo de uso
int main() {
    char rawString[] = "127 223 123 254 145";

    unsigned char xorKeyPerkins[] = {0xE1, 0x66, 0xDF, 0x46}; // Perkins
    unsigned char xorKeyCAT[] = {0xBD, 0x5E, 0x3E, 0x73}; // CAT

    std::vector<uint8_t> outputKey1, outputKey2, outputKey3;

    std::cout << "Perkins Key:\n";
    int keyLength1 = KeyGenerator::seed2key(rawString, outputKey1, xorKeyPerkins, false);
    printKey(outputKey1, keyLength1);
    std::cout << std::endl;

    std::cout << "CAT Key:\n";
    int keyLength2 = KeyGenerator::seed2key(rawString, outputKey2, xorKeyCAT, false);
    printKey(outputKey2, keyLength2);
    std::cout << std::endl;

    // Ejemplo en modo programación
    std::cout << "Programming key:\n";
    int keyLength3 = KeyGenerator::seed2key(rawString, outputKey3, NULL, true);
    printKey(outputKey3, keyLength3);
    std::cout << std::endl;

    return 0;
}