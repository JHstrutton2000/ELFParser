// PS2ELFParser.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <mutex>

#include "Header.hpp"
#include "Blocks.hpp"

constexpr const char* ELF_FILE = "C:\\Users\\redst\\Documents\\GitHub\\Transformers2004\\Mods\\SLUS_206.68";

#define PRINT_ALL
#define LOG_ASCII
#define THREAD_COUNT 30

std::ofstream ofile("output.txt");

#if (THREAD_COUNT == 0)
    #undef THREAD_COUNT
#endif

#ifdef THREAD_COUNT
    std::mutex logMutex[THREAD_COUNT];
    #define OUTPUT threadedOutput
#else
    #define OUTPUT ofile
#endif

char* entryAddr;

void parseProg(uint32_t* pos, uint32_t* lastPos, int threadIndex = 1) {
    std::condition_variable cv;
    size_t size = lastPos - pos;
    std::stringstream threadedOutput;

    int lastUnknownCount = -1;
    int lastprog = -1;
    int unknownCount = 0;

    for (size_t i = 0; i <= size; i++) {
        const char* instName = "Unknown";
        std::string resultString;
        bool pass = false;

        for (const auto& pair : opCodes) {
            blockResult result = parseBlocks(&pos[i], pair.second);
            if (result.pass) {
                pass = true;
                instName = pair.first;
                resultString = result.title;

                break;
            }
        }

        int prog = (100 * i) / size;
        if (prog != lastprog || unknownCount != lastUnknownCount) {
            printf("\033[%d;1HThread #%d \033[%d;12H[\033[%d;113H] %d%% (%d Failed)", threadIndex, threadIndex, threadIndex, threadIndex, prog, unknownCount);
            if (prog != lastprog) {
                printf("\033[%d;%dH\033[32m=\033[0m", threadIndex, 12 + prog);//20 + prog
            }

            lastprog = prog;
            lastUnknownCount = unknownCount;
        }

#ifdef PRINT_ALL

        uint32_t rawaddr = (char*)(&pos[i]) - entryAddr + 0x8;
        uint32_t ps2Addr = rawaddr + 0x100000;

        OUTPUT << " 0x" << std::setfill('0') << std::setw(8) << std::hex << ps2Addr << ":"
           << " 0x" << std::setfill('0') << std::setw(8) << std::hex << pos[i]
           << " " << instName;

#ifdef LOG_ASCII
        if (pass == false) {
            std::string asciiRep;
            for (int j = 0; j < 4; ++j) {
                char c = static_cast<char>((pos[i] >> (8 * (3 - j))) & 0xFF);
                if (std::isprint(static_cast<unsigned char>(c))) {
                    asciiRep += c;
                }
                else {
                    asciiRep += '.'; // Placeholder for non-printable characters
                }
            }

            // Log the ASCII representation
            OUTPUT << " [" << asciiRep << "]";
        }
#endif

        OUTPUT << " " << resultString;
        OUTPUT << std::endl;

#else
        if (pass == false) {
            uint32_t rawaddr = (char*)(&pos[i]) - entryAddr + 0x108;
            uint32_t ps2Addr = rawaddr + 0x100000;

            OUTPUT << " 0x" << std::setfill('0') << std::setw(8) << std::hex << ps2Addr << ":"
                  << " 0x" << std::setfill('0') << std::setw(8) << std::hex << pos[i];
#ifdef LOG_ASCII
            std::string asciiRep;
            for (int j = 0; j < 4; ++j) {
                char c = static_cast<char>((pos[i] >> (8 * (3 - j))) & 0xFF);
                if (std::isprint(static_cast<unsigned char>(c))) {
                    asciiRep += c;
                }
                else {
                    asciiRep += '.'; // Placeholder for non-printable characters
                }
            }

            // Log the ASCII representation
            OUTPUT << " [" << asciiRep << "]" << std::endl;

#else
            OUTPUT << std::endl;
#endif
        }

#endif

        if (pass == false) {
            unknownCount++;
        }
    }
#ifdef THREAD_COUNT
    printf("\033[%d;1H\033[KThread #%d Waiting to log", threadIndex, threadIndex);

RETRY:
    if (logMutex[threadIndex - 1].try_lock() == false) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        goto RETRY;
    }

    ofile << OUTPUT.str();
#endif


    printf("\033[%d;1H\033[K\033[32mThread #%d\033[0m: %d Failed", threadIndex, threadIndex, unknownCount);

    std::cout << " 0x" << std::setfill('0') << std::setw(8) << std::hex << (uint32_t)pos << " - 0x" << std::setfill('0') << std::setw(8) << std::hex << (uint32_t)lastPos << "\n";

#ifdef THREAD_COUNT
    logMutex[threadIndex - 1].unlock();
#endif
}


int main() {
    std::ifstream file(ELF_FILE, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return 1;
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(fileSize);
    std::vector<std::thread> threads;

    if (file.read(buffer.data(), fileSize)) {
        char* pos = buffer.data();

        parseHeader(pos);


        auto entry = HEADER_VALUE(Elf, e_entry);
        pos = &pos[(entry >> 12) | (entry & 0x8)];

        entryAddr = pos;

        uint32_t* tmpPos = (uint32_t*)pos;

#ifdef THREAD_COUNT
        int arrSize = (fileSize / sizeof(uint32_t)) / (THREAD_COUNT);

        for (int i = 1; i <= THREAD_COUNT; ++i) {
            logMutex[i - 1].lock();
            uint32_t* start = tmpPos;
            uint32_t* end = (i == THREAD_COUNT) ? (uint32_t*) &pos[fileSize - 8] : &tmpPos[arrSize];
            threads.emplace_back([start, end, i]() {
                parseProg(start, end, i);
            });

            tmpPos = end;
        }

        for (int i = 0; i < threads.size(); ++i) {
            logMutex[i].unlock();

            if (threads[i].joinable()) {
                threads[i].join();
            }
        }
#else
        parseProg(tmpPos, &tmpPos[fileSize]);
#endif //THREAD_COUNT
    }
    else {
        std::cerr << "Failed to read the file." << std::endl;
    }

    ofile.close();

    std::cin;
}