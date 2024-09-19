// PS2ELFParser.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define ELF_FILE "C:\\Users\\redst\\Documents\\GitHub\\Transformers2004\\Mods\\SLUS_206.68"

struct instruction {
    const char* name;
    uint32_t mask;
};

#define REGIMM 1000000

//instruction instructions[] = {
//    { "ADD",     0b00000100000000000000000000000000 },
//    //ADDI
//    { "ADDIU",   0b00000000000000000000000000100100 },
//    { "ADDU",    0b10000100000000000000000000000000 },
//    { "AND",     0b00100100000000000000000000000000 },
//    { "ANDI",    0b00000000000000000000000000001100 },
//    { "BEQ",     0b00000000000000000000000000001000 },
//    { "BEQL",    0b00000000000000000000000000001010 },
//    { "BGEZ",    0b00000000000000001000000000100000 },
//    { "BGEZAL",  0b00000000000000001000100000100000 },
//    { "BGEZALL", 0b00000000000000001100100000100000 },
//    { "BGEZL",   0b00000000000000001100000000100000 },
//    { "BGTZ",    0b00000000000000000000000000111000 },
//    { "BGTZL",   0b00000000000000000000000000111010 },
//    { "BLEZ",    0b00000000000000000000000000011000 },
//    { "BLEZL",   0b00000000000000000000000000011010 },
//
//    { "BLTZ",    0b00000000000000000000000000100000 },
//    { "BLTZAL",  0b00000000000000000000100000100000 },
//    { "BLTZALL", 0b00000000000000000100100000100000 },
//    { "BLTZL",   0b00000000000000000100000000100000 },
//    { "BNE",     0b00000000000000000000000000101000 },
//    { "BNEL",    0b00000000000000000000000000101010 },
//    { "BREAK",   0b10110000000000000000000000000000 },
//    { "DADD",    0b00110100000000000000000000000000 },
//    { "DADDI",   0b00000000000000000000000000011000 },
//    { "DADDIU",  0b00000000000000000000000000100110 },
//    { "DADDU",   0b10110100000000000000000000000000 },
//    { "DIV",     0b01011000000000000000000000000000 },
//    { "DIVU",    0b11011000000000000000000000000000 },
//    { "DSLL",    0b00111000000000000000000000000000 },
//    { "DSLL32",  0b00111100000000000000000000000000 },
//    { "DSLLV",   0b00101000000000000000000000000000 },
//    { "DSRA",    0b11011100000000000000000000000000 },
//    { "DSRA32",  0b11111100000000000000000000000000 },
//    { "DSRAV",   0b11101000000000000000000000000000 },
//    { "DSRL",    0b01011100000000000000000000000000 },
//    { "DSRL32",  0b00111000000000000000000000000000 },
//    { "DSLL",    0b01101000000000000000000000000000 },
//    { "DSUB",    0b01110100000000000000000000000000 },
//    { "DSUBU",   0b11110100000000000000000000000000 },
//    { "J",       0b00000000000000000000000000010000 },
//    { "JAL",     0b00000000000000000000000000110000 },
//    //{ "JALR",    0b00100100000000000000000000000000 }, // Duplicate ?? This format will not work...
//    { "JR",      0b00001000000000000000000000000000 },
//    { "LB",      0b00000000000000000000000000000001 },
//    { "LBU",     0b00000000000000000000000000001001 },
//    { "LD",      0b00000000000000000000000000111011 },
//    { "LDL",     0b00000000000000000000000000010110 },
//    { "DSLL",    0b00100100000000000000000000000000 }
//
//};
//
//instruction NOP{ "NOP", 0 };

enum FORMAT : uint8_t{ 
    ELF_32BIT = 1, 
    ELF_64BIT = 2 
};

enum ENDIANNESS : uint8_t {
    LITTLE = 1, 
    BIG = 2 
};

enum TARGET_OS : uint8_t {
    System_V = 0x00,
    HP_UX = 0x01,
    NetBSD = 0x02,
    Linux = 0x03,
    GNU_Hurd = 0x04,
    Solaris = 0x06,
    AIX_Monterey = 0x07,
    IRIX = 0x08,
    FreeBSD = 0x09,
    Tru64 = 0x0A,
    Novell_Modesto = 0x0B,
    OpenBSD = 0x0C,
    OpenVMS = 0x0D,
    NonStop_Kernel = 0x0E,
    AROS = 0x0F,
    FenixOS = 0x10,
    Nuxi_CloudABI = 0x11,
    Stratus_Technologies_OpenVOS = 0x12
};

enum ELF_OBJECT_TYPE : uint16_t {
    ET_NONE = 0x00,
    ET_REL = 0x01,
    ET_EXEC = 0x02,
    ET_DYN = 0x03,
    ET_CORE = 0x04,
    ET_LOOS = 0xFE00,
    ET_HIOS = 0xFEFF,
    ET_LOPROC = 0xFF00,
    ET_HIPROC = 0xFFFF
};

enum INSTRUCTION_ARCH : uint16_t {
    Generic = 0x00,
    ATT_WE_32100 = 0x01,
    SPARC = 0x02,
    x86 = 0x03,
    Motorola_68000_M68k = 0x04,
    Motorola_88000_M88k = 0x05,
    Intel_MCU = 0x06,
    Intel_80860 = 0x07,
    MIPS = 0x08,
    IBM_System_370 = 0x09,
    MIPS_RS3000_Little_endian = 0x0A,
    /*-0x0E	Reserved for future use = 0x0B,*/
    HewlettPackard_PA_RISC = 0x0F,
    Intel_80960 = 0x13,
    PowerPC = 0x14,
    PowerPC_64bit = 0x15,
    S390_including_S390x = 0x16,
    IBM_SPU_SPC = 0x17,
    //-0x23	Reserved for future use = 0x18,
    NEC_V800 = 0x24,
    Fujitsu_FR20 = 0x25,
    TRW_RH_32 = 0x26,
    Motorola_RCE = 0x27,
    Arm = 0x28,
    Digital_Alpha = 0x29,
    SuperH = 0x2A,
    SPARC_Version_9 = 0x2B,
    Siemens_TriCore = 0x2C,
    Argonaut_RISC = 0x2D,
    Hitachi_H8_300 = 0x2E,
    Hitachi_H8_300H = 0x2F,
    Hitachi_H8S = 0x30,
    Hitachi_H8_500 = 0x31,
    IA64 = 0x32,
    Stanford_MIPSX = 0x33,
    Motorola_ColdFire = 0x34,
    Motorola_M68HC12 = 0x35,
    Fujitsu_MMA= 0x36,
    Siemens_PCP = 0x37,
    Sony_nCPU= 0x38,
    Denso_NDR1 = 0x39,
    Motorola_Star = 0x3A,
    Toyota_ME16 = 0x3B,
    ST100 = 0x3C,
    TinyJ = 0x3D,
    AMD = 0x3E,
    Sony_DSP = 0x3F,
    Digital_Equipment_Corp_10 = 0x40,
    Digital_Equipment_Corp_11 = 0x41,
    Siemens_FX66 = 0x42,
    ST9_16bit= 0x43,
    ST7_8bit = 0x44,
    MC68HC16 = 0x45,
    MC68HC11 = 0x46,
    MC68HC08 = 0x47,
    MC68HC05 = 0x48,
    SVx = 0x49,
    ST19_8bit = 0x4A,
    Digital_VAX = 0x4B,
    Axis_Communications_32bit = 0x4C,
    Infineon_Technologies_32bit = 0x4D,
    Element14_64_bit = 0x4E,
    LSI_16bit = 0x4F,
    TMS320C6000 = 0x8C,
    MCST_Elbrus_e2k = 0xAF,
    Arm64bits = 0xB7,
    ZilogZ80 = 0xDC,
    RISCV = 0xF3,
    Berkeley_Packet_Filter = 0xF7,
    WDC65C816 = 0x101,
    LoongArch = 0x102
};

typedef struct {
    unsigned char EI_MAG[4];
    FORMAT        EI_CLASS;
    ENDIANNESS    EI_DATA;
    uint8_t       EI_VERSION;
    TARGET_OS     EI_OSABI;
    uint8_t       EI_ABRIVERSION;
    uint8_t       EI_PADDING[8];
    ELF_OBJECT_TYPE   e_type;
    INSTRUCTION_ARCH  e_machine;
    uint32_t          e_version;
} ELF_HEADER;

typedef struct {
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} ELF_HEADER_32;

typedef struct {
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} ELF_HEADER_64;

typedef struct {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
} PROGRAM_HEADER_32;

typedef struct {
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
} PROGRAM_HEADER_64;

typedef struct {
    uint32_t sh_name;
    uint32_t sh_type;
    uint32_t sh_flags;
    uint32_t sh_addr;
    uint32_t sh_offset;
    uint32_t sh_size;
    uint32_t sh_links;
    uint32_t sh_info;
    uint32_t sh_addralign;
    uint32_t sh_entsize;
} SECTION_HEADER_32;

typedef struct {
    uint32_t sh_name;
    uint32_t sh_type;
    uint64_t sh_flags;
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;
    uint32_t sh_links;
    uint32_t sh_info;
    uint64_t sh_addralign;
    uint64_t sh_entsize;
} SECTION_HEADER_64;

int main() {

    std::ifstream file(ELF_FILE, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return 1;
    }

    // Get the size of the file
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Create a buffer to hold the data
    std::vector<char> buffer(fileSize);

    // Read the file contents into the buffer
    if (file.read(buffer.data(), fileSize)) {
        std::cout << "File read successfully." << std::endl;

        //parse(buffer);
    }
    else {
        std::cerr << "Failed to read the file." << std::endl;
    }

    std::cin;
}


//void parse(std::vector<char> buffer) {
//    for (char b : buffer) {
//        //if ((b & ADDIU_MASK) == ADDIU) {
//
//        //}
//    }
//}