enum FORMAT : uint8_t {
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
    Fujitsu_MMA = 0x36,
    Siemens_PCP = 0x37,
    Sony_nCPU = 0x38,
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
    ST9_16bit = 0x43,
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
    uint8_t       EI_PADDING[7];
    ELF_OBJECT_TYPE   e_type;
    INSTRUCTION_ARCH  e_machine;
    uint32_t          e_version;
} ELF_IDENTITY_HEADER;

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

bool bit32;

#define HEADER_VALUE(x, attribute) (bit32 ? (x.Header32.attribute) : (x.Header64.attribute))
#define HEADER_SET(x, val, type32, type64) if(bit32){ x.Header32 = *(type32*)val; } else { x.Header64 = *(type64*)val; }

typedef struct {
    ELF_IDENTITY_HEADER Identity;

    union {
        ELF_HEADER_32 Header32;
        ELF_HEADER_64 Header64;
    };
} ELF_HEADER;

typedef struct {
    union {
        PROGRAM_HEADER_32 Header32;
        PROGRAM_HEADER_64 Header64;
    };
} PROGRAM_HEADER;

typedef struct {
    union {
        SECTION_HEADER_32 Header32;
        SECTION_HEADER_64 Header64;
    };
} SECTION_HEADER;

ELF_HEADER Elf;
PROGRAM_HEADER Program;
SECTION_HEADER Section;

void parseHeader(char* pos) {
    ELF_IDENTITY_HEADER* header = (ELF_IDENTITY_HEADER*)pos;
    Elf.Identity = *header;

    switch (header->EI_CLASS) {
    case ELF_32BIT:
        bit32 = true;

        Elf.Header32 = *(ELF_HEADER_32*)(&pos[sizeof(ELF_IDENTITY_HEADER)]);
        Program.Header32 = *(PROGRAM_HEADER_32*)(&pos[Elf.Header32.e_phoff]);
        Section.Header32 = *(SECTION_HEADER_32*)(&pos[Elf.Header32.e_shoff]);

        break;
    case ELF_64BIT:
        bit32 = false;

        Elf.Header64 = *(ELF_HEADER_64*)(&pos[sizeof(ELF_IDENTITY_HEADER)]);
        Program.Header64 = *(PROGRAM_HEADER_64*)(&pos[Elf.Header32.e_phoff]);
        Section.Header64 = *(SECTION_HEADER_64*)(&pos[Elf.Header32.e_shoff]);

        break;
    }
}