enum BlockType {
    opCode,
    stype,
    code,
    special,
    immediate,
    offset,
    instIndex,
    base,
    hint,
    mmi,
    reg,
    rs,
    rt,
    rd,
    fs,
    fd,
    ft,
    sa
};

class Block {
public:
    enum BlockType type;
    uint8_t value;
    uint8_t size;

    Block(BlockType t, uint8_t s) : type(t), value(0xFF), size(s) {}
    Block(uint8_t v, uint8_t s) : type(BlockType::opCode), value(v), size(s) {}

    uint32_t getValue(uint32_t* curInstruction, uint8_t bitPos) {
        auto mask = 0xFFFFFFFF >> (32 - size);

        return (*curInstruction >> bitPos) & mask;
    }

    uint32_t toSigned(uint32_t value) {
        if (size == 0)
            return 0;

        auto signedBit = 1 << (size - 1);
        auto maxValue = 1 << size;

        if (value & signedBit) {
            return maxValue - value;
        }

        return 0;
    }
};

struct blockResult {
    bool pass;
    std::string title;
};

blockResult parseBlocks(uint32_t* curInstruction, std::vector<Block>blocks) {
    blockResult result = { true, ""};
    uint8_t bitPos = 32;

    for (Block block : blocks) {
        std::stringstream ss;

        bitPos -= block.size;

        uint32_t instructionValue = block.getValue(curInstruction, bitPos);
        uint32_t signedResult = block.toSigned(instructionValue);

        switch (block.type) {
            case BlockType::opCode:
                if (block.value != instructionValue) {
                    return { false };
                }
                break;
            case BlockType::fs:
                result.title += " f" + std::to_string(instructionValue);
                break;
            case BlockType::immediate:
            case BlockType::offset:
                if (signedResult)
                    ss << " -0x" << std::hex << signedResult;
                else
                    ss << " 0x" << std::hex << instructionValue;

                result.title += ss.str();
                break;
            case BlockType::base:
                if (instructionValue == 0)
                    result.title += " zero";
                else
                    result.title += " base[" + std::to_string(instructionValue) + "]";
                break;
                break;
            case BlockType::rs:
            case BlockType::rt:
            case BlockType::rd:
                if (instructionValue == 0)
                    result.title += " zero";
                else
                    result.title += " GPR[" + std::to_string(instructionValue) + "]";
                break;
            case BlockType::fd:
            case BlockType::ft:
                if (instructionValue == 0)
                    result.title += " zero";
                else
                    result.title += " FPR[" + std::to_string(instructionValue) + "]";
                break;
            case BlockType::sa:
                if (instructionValue == 0) {
                    result.title += " zero";
                }
                else {
                    ss << std::hex << instructionValue;
                    result.title += " 0x" + ss.str();
                }
                break;
            case BlockType::hint:
                result.title += " " + std::to_string(instructionValue);
                break;
            case BlockType::special:
            case BlockType::stype:
            case BlockType::code:
            case BlockType::instIndex:
            case BlockType::mmi:
            case BlockType::reg:
            default:
                break;
        }


    }

    return result;
}

Block immediateBlock(BlockType::immediate, 16); //16 bits
Block offsetBlock(BlockType::offset, 16);//16 bits
Block rsBlock(BlockType::rs, 5);//5 bits
Block rtBlock(BlockType::rt, 5);//5 bits
Block rdBlock(BlockType::rd, 5);//5 bits
Block saBlock(BlockType::sa, 5);//5 bits
Block fsBlock(BlockType::fs, 5);//5 bits
Block fdBlock(BlockType::fd, 5);//5 bits
Block ftBlock(BlockType::ft, 5);//5 bits
Block stypeBlock(BlockType::stype, 5);//5 bits
Block baseBlock(BlockType::base, 5);//5 bits
Block hintBlock(BlockType::hint, 5);//5 bits
Block regBlock(BlockType::reg, 5);//5 bits
Block zeroBlock(0b00000, 5);//5 bits
Block bc0Block(0b01000, 5);//5 bits
Block c0Block(0b10000, 5);//5 bits
Block mf0Block(0b00000, 5);//5 bits
Block mt0Block(0b00100, 5);//5 bits
Block sBlock(0b10000, 5);//5 bits
Block bc1Block(0b01000, 5);//5 bits
Block wBlock(0b10100, 5);//5 bits

Block fcBlock(0b11, 2); //2 bits

Block specialBlock(0b000000, 6);//6 bits
Block RegimmBlock(0b000001, 6);//6 bits
Block cop0Block(0b010000, 6);//6 bits
Block cop1Block(0b010001, 6);//6 bits
Block mmiBlock(0b011100, 6);//6 bits
Block mmi0Block(0b001000, 6);//6 bits
Block mmi1Block(0b101000, 6);//6 bits
Block mmi2Block(0b001001, 6);//6 bits
Block mmi3Block(0b101001, 6);//6 bits
Block pmfhlBlock(0b110000, 6);//6 bits
Block pmthlBlock(0b110001, 6);//6 bits
Block cacheBlock(0b101111, 6);//6 bits

std::map<const char*, std::vector<Block>> opCodes = {
    //CPU                                                                                                                                                   
        { "nop",          { Block{0, 32}}}                                                                                                                  ,//0
        { "cfc2",         { Block{0b010010, 6}}}                                                                                                            ,//0
        { "sqc2",         { Block{0b111110, 6}}}                                                                                                            ,
        { "lqc2",         { Block{0b110110, 6}}}                                                                                                            ,
        { "J",            { Block{0b000010, 6}  , Block{BlockType::offset, 26}}}                                                                            ,
        { "JAL",          { Block{0b000011, 6}  , Block{BlockType::instIndex, 26}}}                                                                         ,
        { "BGTZ",         { Block{0b000111, 6}  , rsBlock          , zeroBlock        , offsetBlock}}                                                       ,
        { "BGTZL",        { Block{0b010111, 6}  , rsBlock          , zeroBlock        , offsetBlock}}                                                       ,
        { "BLEZ",         { Block{0b000110, 6}  , rsBlock          , zeroBlock        , offsetBlock}}                                                       ,
        { "BLEZL",        { Block{0b010110, 6}  , rsBlock          , zeroBlock        , offsetBlock}}                                                       ,
        { "BNE",          { Block{0b000101, 6}  , rsBlock          , rtBlock          , offsetBlock}}                                                       ,
        { "BNEL",         { Block{0b010101, 6}  , rsBlock          , rtBlock          , offsetBlock}}                                                       ,
        { "BEQ",          { Block{0b000100, 6}  , rsBlock          , rtBlock          , offsetBlock}}                                                       ,
        { "BEQL",         { Block{0b010100, 6}  , rsBlock          , rtBlock          , offsetBlock}}                                                       ,
        { "LB",           { Block{0b100000, 6}  , baseBlock        , rtBlock          , offsetBlock}}                                                       ,
        { "LBU",          { Block{0b100100, 6}  , baseBlock        , rtBlock          , offsetBlock}}                                                       ,
        { "LD",           { Block{0b110111, 6}  , baseBlock        , rtBlock          , offsetBlock}}                                                       ,
        { "LDL",          { Block{0b011010, 6}  , baseBlock        , rtBlock          , offsetBlock}}                                                       ,
        { "LDR",          { Block{0b011011, 6}  , baseBlock        , rtBlock          , offsetBlock}}                                                       ,
        { "LH",           { Block{0b100001, 6}  , baseBlock        , rtBlock          , offsetBlock}}                                                       ,
        { "LHU",          { Block{0b100101, 6}  , baseBlock        , rtBlock          , offsetBlock}}                                                       ,
        { "LW",           { Block{0b100011, 6}  , baseBlock        , rtBlock          , offsetBlock}}                                                       ,
        { "LWL",          { Block{0b100010, 6}  , baseBlock        , rtBlock          , offsetBlock}}                                                       ,
        { "LWR",          { Block{0b100110, 6}  , baseBlock        , rtBlock          , offsetBlock}}                                                       ,
        { "LWU",          { Block{0b100111, 6}  , baseBlock        , rtBlock          , offsetBlock}}                                                       ,
        { "SB",           { Block{0b101000, 6}  , baseBlock        , rtBlock          , offsetBlock}}                                                       ,
        { "SD",           { Block{0b111111, 6}  , baseBlock        , rtBlock          , offsetBlock}}                                                       ,
        { "SDL",          { Block{0b101100, 6}  , baseBlock        , rtBlock          , offsetBlock}}                                                       ,
        { "SDR",          { Block{0b101101, 6}  , baseBlock        , rtBlock          , offsetBlock}}                                                       ,
        { "SH",           { Block{0b101001, 6}  , baseBlock        , rtBlock          , offsetBlock}}                                                       ,
        { "SW",           { Block{0b101011, 6}  , baseBlock        , rtBlock          , offsetBlock}}                                                       ,
        { "SWL",          { Block{0b101010, 6}  , baseBlock        , rtBlock          , offsetBlock}}                                                       ,
        { "SWR",          { Block{0b101110, 6}  , baseBlock        , rtBlock          , offsetBlock}}                                                       ,
        { "LB",           { Block{0b100101, 6}  , baseBlock        , rtBlock          , offsetBlock}}                                                       ,
        { "LWC1",         { Block{0b110001, 6}  , baseBlock        , ftBlock          , offsetBlock}}                                                       ,
        { "SWC1",         { Block{0b111001, 6}  , baseBlock        , ftBlock          , offsetBlock}}                                                       ,
        { "LQ",           { Block{0b011110, 6}  , baseBlock        , rtBlock          , offsetBlock}}                                                       ,
        { "SQ",           { Block{0b011111, 6}  , baseBlock        , rtBlock          , offsetBlock}}                                                       ,
        { "PREF",         { Block{0b110011, 6}  , baseBlock        , hintBlock        , offsetBlock}}                                                       ,
        { "LUI",          { Block{0b001111, 6}  , zeroBlock        , rtBlock          , immediateBlock}}                                                    ,
        { "DADDI",        { Block{0b011000, 6}  , rsBlock          , rtBlock          , immediateBlock}}                                                    ,
        { "DADDIU",       { Block{0b011001, 6}  , rsBlock          , rtBlock          , immediateBlock}}                                                    ,
        { "ORI",          { Block{0b001101, 6}  , rsBlock          , rtBlock          , immediateBlock}}                                                    ,
        { "SLTI",         { Block{0b001010, 6}  , rsBlock          , rtBlock          , immediateBlock}}                                                    ,
        { "SLTIU",        { Block{0b001011, 6}  , rsBlock          , rtBlock          , immediateBlock}}                                                    ,
        { "XORI",         { Block{0b001110, 6}  , rsBlock          , rtBlock          , immediateBlock}}                                                    ,
        { "ADDIU",        { Block{0b001001, 6}  , rsBlock          , rtBlock          , immediateBlock}}                                                    ,
        { "ANDI",         { Block{0b001100, 6}  , rsBlock          , rtBlock          , immediateBlock}}                                                    ,
        { "ADDI",         { Block{0b001000, 6}  , rsBlock          , rtBlock          , immediateBlock}}                                                    ,
        { "MTHI",         { specialBlock        , rsBlock          , Block{0, 15}                                                 , Block{0b010001, 6}}}    ,
        { "MTLO",         { specialBlock        , rsBlock          , Block{0, 15}                                                 , Block{0b010011, 6}}}    ,
        { "JR",           { specialBlock        , rsBlock          , Block{0, 15}                                                 , Block{0b001000, 6}}}    ,
        { "BREAK",        { specialBlock        , Block{BlockType::code, 20}                                                      , Block{0b001101, 6}}}    ,
        { "SYSCALL",      { specialBlock        , Block{BlockType::code, 20}                                                      , Block{0b001100, 6}}}    ,
        { "SYNC",         { specialBlock        , Block{0, 15}                                            , stypeBlock            , Block{0b001111, 6}}}    ,
        { "MFHI",         { specialBlock        , Block{0, 10}                        , rdBlock           , zeroBlock             , Block{0b010000, 6}}}    ,
        { "MFLO",         { specialBlock        , Block{0, 10}                        , rdBlock           , zeroBlock             , Block{0b010010, 6}}}    ,
        { "MFSA",         { specialBlock        , Block{0, 10}                        , rdBlock           , zeroBlock             , Block{0b101000, 6}}}    ,
        { "JALR",         { specialBlock        , rsBlock          , zeroBlock        , rdBlock           , zeroBlock             , Block{0b001001, 6}}}    ,
        { "ADD",          { specialBlock        , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b100000, 6}}}    ,
        { "ADDU",         { specialBlock        , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b100001, 6}}}    ,
        { "AND",          { specialBlock        , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b100100, 6}}}    ,
        { "DSRAV",        { specialBlock        , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b010111, 6}}}    ,
        { "DSRLV",        { specialBlock        , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b010110, 6}}}    ,
        { "DSUB",         { specialBlock        , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b101110, 6}}}    ,
        { "DSUBU",        { specialBlock        , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b101111, 6}}}    ,
        { "MOVN",         { specialBlock        , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b001011, 6}}}    ,
        { "MOVZ",         { specialBlock        , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b001010, 6}}}    ,
        { "NOR",          { specialBlock        , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b100111, 6}}}    ,
        { "OR",           { specialBlock        , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b100101, 6}}}    ,
        { "SLLV",         { specialBlock        , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b000100, 6}}}    ,
        { "SLT",          { specialBlock        , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b101010, 6}}}    ,
        { "SLTU",         { specialBlock        , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b101011, 6}}}    ,
        { "SRAV",         { specialBlock        , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b000111, 6}}}    ,
        { "SRLV",         { specialBlock        , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b000110, 6}}}    ,
        { "SUB",          { specialBlock        , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b100010, 6}}}    ,
        { "SUBU",         { specialBlock        , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b100011, 6}}}    ,
        { "DADD",         { specialBlock        , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b101100, 6}}}    ,
        { "DADDU",        { specialBlock        , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b101101, 6}}}    ,
        { "XOR",          { specialBlock        , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b100110, 6}}}    ,
        { "MULT",         { specialBlock        , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b011000, 6}}}    ,
        { "MULTU",        { specialBlock        , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b011001, 6}}}    ,
        { "DSLLV",        { specialBlock        , rsBlock          , rtBlock          , rdBlock           , saBlock               , Block{0b010100, 6}}}    ,
        { "DIV",          { specialBlock        , rsBlock          , rtBlock          , Block{0,               10}                , Block{0b011010, 6}}}    ,
        { "DIVU",         { specialBlock        , rsBlock          , rtBlock          , Block{0,               10}                , Block{0b011011, 6}}}    ,
        { "MULT",         { specialBlock        , rsBlock          , rtBlock          , Block{0,               10}                , Block{0b011000, 6}}}    ,
        { "MULTU",        { specialBlock        , rsBlock          , rtBlock          , Block{0,               10}                , Block{0b011001, 6}}}    ,
        { "TGE",          { specialBlock        , rsBlock          , rtBlock          , Block{BlockType::code, 10}                , Block{0b110000, 6}}}    ,
        { "TGEU",         { specialBlock        , rsBlock          , rtBlock          , Block{BlockType::code, 10}                , Block{0b110001, 6}}}    ,
        { "TLT",          { specialBlock        , rsBlock          , rtBlock          , Block{BlockType::code, 10}                , Block{0b110010, 6}}}    ,
        { "TLTU",         { specialBlock        , rsBlock          , rtBlock          , Block{BlockType::code, 10}                , Block{0b110011, 6}}}    ,
        { "TNE",          { specialBlock        , rsBlock          , rtBlock          , Block{BlockType::code, 10}                , Block{0b110110, 6}}}    ,
        { "TEQ",          { specialBlock        , rsBlock          , rtBlock          , Block{BlockType::code, 10}                , Block{0b110100, 6}}}    ,
        { "MTSA",         { specialBlock        , rsBlock          , Block{0, 15}                                                 , Block{0b101001, 6}}}    ,
        { "DSLL",         { specialBlock        , zeroBlock        , rtBlock          , rdBlock           , saBlock               , Block{0b111000, 6}}}    ,
        { "DSLL32",       { specialBlock        , zeroBlock        , rtBlock          , rdBlock           , saBlock               , Block{0b111100, 6}}}    ,
        { "DSRA",         { specialBlock        , zeroBlock        , rtBlock          , rdBlock           , saBlock               , Block{0b111011, 6}}}    ,
        { "DSRA32",       { specialBlock        , zeroBlock        , rtBlock          , rdBlock           , saBlock               , Block{0b111111, 6}}}    , //Unknown
        { "DSRL",         { specialBlock        , zeroBlock        , rtBlock          , rdBlock           , saBlock               , Block{0b111010, 6}}}    ,
        { "DSRL32",       { specialBlock        , zeroBlock        , rtBlock          , rdBlock           , saBlock               , Block{0b111110, 6}}}    ,
        { "SLL",          { specialBlock        , zeroBlock        , rtBlock          , rdBlock           , saBlock               , Block{0b000000, 6}}}    ,
        { "SRA",          { specialBlock        , zeroBlock        , rsBlock          , rdBlock           , saBlock               , Block{0b000011, 6}}}    ,
        { "SRL",          { specialBlock        , zeroBlock        , rtBlock          , rdBlock           , saBlock               , Block{0b000010, 6}}}    ,
        { "DIV1",         { mmiBlock            , rsBlock          , rtBlock          , Block{0, 10}                              , Block{0b011010, 6}}}    ,
        { "DIVU1",        { mmiBlock            , rsBlock          , rtBlock          , Block{0, 10}                              , Block{0b011011, 6}}}    ,
        { "MTLO1",        { mmiBlock            , rsBlock          , Block{0, 15}                                                 , Block{0b010011, 6}}}    ,
        { "MTHI1",        { mmiBlock            , rsBlock          , Block{0, 15}                                                 , Block{0b010001, 6}}}    ,
        { "MFLO1",        { mmiBlock            , Block{0, 10}                        , rdBlock           , zeroBlock             , Block{0b010010, 6}}}    ,
        { "MFHI1",        { mmiBlock            , Block{0, 10}                        , rdBlock           , zeroBlock             , Block{0b010000, 6}}}    ,
        { "PLZCW",        { mmiBlock            , rsBlock          , zeroBlock        , rdBlock           , zeroBlock             , Block{0b000100, 5}}}    ,
        { "MADD",         { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b000000, 6}}}    ,
        { "MADD1",        { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b100000, 6}}}    ,
        { "MADDU",        { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b000001, 6}}}    ,
        { "MADDU1",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b100001, 6}}}    ,
        { "MULT1",        { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b011000, 6}}}    ,
        { "MULTU1",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , zeroBlock             , Block{0b011001, 6}}}    ,
        { "PSLLH",        { mmiBlock            , zeroBlock        , rtBlock          , rdBlock           , saBlock               , Block{0b110100, 6}}}    ,
        { "PSLLW",        { mmiBlock            , zeroBlock        , rtBlock          , rdBlock           , saBlock               , Block{0b111100, 6}}}    ,
        { "PSRAH",        { mmiBlock            , zeroBlock        , rtBlock          , rdBlock           , saBlock               , Block{0b110111, 6}}}    ,
        { "PSRAW",        { mmiBlock            , zeroBlock        , rtBlock          , rdBlock           , saBlock               , Block{0b111111, 6}}}    ,
        { "PSRLH",        { mmiBlock            , zeroBlock        , rtBlock          , rdBlock           , saBlock               , Block{0b110110, 6}}}    ,
        { "PSRLW",        { mmiBlock            , zeroBlock        , rtBlock          , rdBlock           , saBlock               , Block{0b111110, 6}}}    ,
        { "PABSH",        { mmiBlock            , zeroBlock        , rtBlock          , rdBlock           , Block{0b00101, 5}     , mmi1Block}}             ,
        { "PABSW",        { mmiBlock            , zeroBlock        , rtBlock          , rdBlock           , Block{0b00001, 5}     , mmi1Block}}             ,
        { "PEXCH",        { mmiBlock            , zeroBlock        , rtBlock          , rdBlock           , Block{0b11010, 5}     , mmi3Block}}             ,
        { "PEXCW",        { mmiBlock            , zeroBlock        , rtBlock          , rdBlock           , Block{0b11110, 5}     , mmi3Block}}             ,
        { "PEXEH",        { mmiBlock            , zeroBlock        , rtBlock          , rdBlock           , Block{0b11010, 5}     , mmi2Block}}             ,
        { "PEXEW",        { mmiBlock            , zeroBlock        , rtBlock          , rdBlock           , Block{0b11110, 5}     , mmi2Block}}             ,
        { "PEXT5",        { mmiBlock            , zeroBlock        , rtBlock          , rdBlock           , Block{0b11110, 5}     , mmi0Block}}             ,
        { "PCPYH",        { mmiBlock            , zeroBlock        , rtBlock          , rdBlock           , Block{0b11011, 5}     , mmi3Block}}             ,
        { "PPAC5",        { mmiBlock            , zeroBlock        , rtBlock          , rdBlock           , Block{0b11111, 5}     , mmi0Block}}             ,
        { "PREVH",        { mmiBlock            , zeroBlock        , rtBlock          , rdBlock           , Block{0b11011, 5}     , mmi2Block}}             ,
        { "PROT3W",       { mmiBlock            , zeroBlock        , rtBlock          , rdBlock           , Block{0b11111, 5}     , mmi2Block}}             ,
        { "PMTHI",        { mmiBlock            , rsBlock          , Block{0, 10}                         , Block{0b01000, 5}     , mmi3Block}}             ,
        { "PMTHL.LW",     { mmiBlock            , rsBlock          , Block{0, 10}                         , Block{0b00000, 5}     , pmthlBlock}}            ,
        { "PMTLO",        { mmiBlock            , rsBlock          , Block{0, 10}                         , Block{0b01001, 5}     , mmi3Block}}             ,
        { "PMFHI",        { mmiBlock            , Block{0, 10}                        , rdBlock           , Block{0b01000, 5}     , mmi2Block}}             ,
        { "PMFHL.LH",     { mmiBlock            , Block{0, 10}                        , rdBlock           , Block{0b00011, 5}     , pmfhlBlock}}            ,
        { "PMFHL.LW",     { mmiBlock            , Block{0, 10}                        , rdBlock           , Block{0b00000, 5}     , pmfhlBlock}}            ,
        { "PMFHL.SH",     { mmiBlock            , Block{0, 10}                        , rdBlock           , Block{0b00100, 5}     , pmfhlBlock}}            ,
        { "PMFHL.SLW",    { mmiBlock            , Block{0, 10}                        , rdBlock           , Block{0b00010, 5}     , pmfhlBlock}}            ,
        { "PMFHL.UW",     { mmiBlock            , Block{0, 10}                        , rdBlock           , Block{0b00001, 5}     , pmfhlBlock}}            ,
        { "PMFLO",        { mmiBlock            , Block{0, 10}                        , rdBlock           , Block{0b01001, 5}     , mmi2Block}}             ,
        { "PDIVBW",       { mmiBlock            , rsBlock          , rtBlock          , zeroBlock         , Block{0b11101, 5}     , mmi2Block}}             ,
        { "PDIVUW",       { mmiBlock            , rsBlock          , rtBlock          , zeroBlock         , Block{0b01101, 5}     , mmi3Block}}             ,
        { "PDIVW",        { mmiBlock            , rsBlock          , rtBlock          , zeroBlock         , Block{0b01101, 5}     , mmi2Block}}             ,
        { "PADDB",        { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b01000, 5}     , mmi0Block}}             ,
        { "PADDH",        { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b00100, 5}     , mmi0Block}}             ,
        { "PADDSB",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b11000, 5}     , mmi0Block}}             ,
        { "PADDSH",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b10100, 5}     , mmi0Block}}             ,
        { "PADDSW",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b10000, 5}     , mmi0Block}}             ,
        { "PADDUB",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b11000, 5}     , mmi1Block}}             ,
        { "PADDUH",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b10100, 5}     , mmi1Block}}             ,
        { "PADDUW",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b10000, 5}     , mmi1Block}}             ,
        { "PADDW",        { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b00000, 5}     , mmi0Block}}             ,
        { "PADSBH",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b00100, 5}     , mmi1Block}}             ,
        { "PAND",         { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b10010, 5}     , mmi2Block}}             ,
        { "PCEQB",        { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b01010, 5}     , mmi1Block}}             ,
        { "PCEQH",        { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b00110, 5}     , mmi1Block}}             ,
        { "PCEQW",        { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b00010, 5}     , mmi1Block}}             ,
        { "PCGTB",        { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b01010, 5}     , mmi0Block}}             ,
        { "PCGTH",        { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b00110, 5}     , mmi0Block}}             ,
        { "PCGTW",        { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b00010, 5}     , mmi0Block}}             ,
        { "PCPYLD",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b01110, 5}     , mmi2Block}}             ,
        { "PCPYUD",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b01110, 5}     , mmi3Block}}             ,
        { "PEXTLB",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b11010, 5}     , mmi0Block}}             ,
        { "PEXTLH",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b10110, 5}     , mmi0Block}}             ,
        { "PEXTLW",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b10010, 5}     , mmi0Block}}             ,
        { "PEXTUB",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b11010, 5}     , mmi1Block}}             ,
        { "PEXTUH",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b10110, 5}     , mmi1Block}}             ,
        { "PEXTUW",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b10010, 5}     , mmi1Block}}             ,
        { "PHMADH",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b10001, 5}     , mmi2Block}}             ,
        { "PHMSBH",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b10101, 5}     , mmi2Block}}             ,
        { "PINTEH",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b01010, 5}     , mmi3Block}}             ,
        { "PINTH",        { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b01010, 5}     , mmi2Block}}             ,
        { "PMADDH",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b10000, 5}     , mmi2Block}}             ,
        { "PMADDUW",      { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b00000, 5}     , mmi3Block}}             ,
        { "PMADDW",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b00000, 5}     , mmi2Block}}             ,
        { "PMAXH",        { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b00111, 5}     , mmi0Block}}             ,
        { "PMAXW",        { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b00011, 5}     , mmi0Block}}             ,
        { "PMINH",        { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b00111, 5}     , mmi1Block}}             ,
        { "PMINW",        { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b00011, 5}     , mmi1Block}}             ,
        { "PMSUBH",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b10100, 5}     , mmi2Block}}             ,
        { "PMSUBW",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b00100, 5}     , mmi2Block}}             ,
        { "PMULTH",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b11100, 5}     , mmi2Block}}             ,
        { "PMULTUW",      { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b01100, 5}     , mmi3Block}}             ,
        { "PMULTW",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b01100, 5}     , mmi2Block}}             ,
        { "PNOR",         { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b10011, 5}     , mmi3Block}}             ,
        { "POR",          { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b10010, 5}     , mmi3Block}}             ,
        { "PPACB",        { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b11011, 5}     , mmi0Block}}             ,
        { "PPACH",        { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b10111, 5}     , mmi0Block}}             ,
        { "PPACW",        { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b10011, 5}     , mmi0Block}}             ,
        { "PSLLVW",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b00010, 5}     , mmi2Block}}             ,
        { "PSRAVW",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b00011, 5}     , mmi3Block}}             ,
        { "PSRLVW",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b00011, 5}     , mmi2Block}}             ,
        { "PSUBB",        { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b01001, 5}     , mmi0Block}}             ,
        { "PSUBH",        { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b00101, 5}     , mmi0Block}}             ,
        { "PSUBSB",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b11001, 5}     , mmi0Block}}             ,
        { "PSUBSH",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b10101, 5}     , mmi0Block}}             ,
        { "PSUBSW",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b10001, 5}     , mmi0Block}}             ,
        { "PSUBUB",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b11001, 5}     , mmi1Block}}             ,
        { "PSUBUH",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b10101, 5}     , mmi1Block}}             ,
        { "PSUBUW",       { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b10001, 5}     , mmi1Block}}             ,
        { "PSUBW",        { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b00001, 5}     , mmi0Block}}             ,
        { "PXOR",         { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b10011, 5}     , mmi2Block}}             ,
        { "QFSRV",        { mmiBlock            , rsBlock          , rtBlock          , rdBlock           , Block{0b11011, 5}     , mmi1Block}}             ,
        { "CACHE BFH",    { cacheBlock          , baseBlock        , Block{0b01100, 5}, offsetBlock}}                                                       ,
        { "CACHE BHINBT", { cacheBlock          , baseBlock        , Block{0b01010, 5}, offsetBlock}}                                                       ,
        { "CACHE BXLBT",  { cacheBlock          , baseBlock        , Block{0b01100, 5}, offsetBlock}}                                                       ,
        { "CACHE BXSBT",  { cacheBlock          , baseBlock        , Block{0b00110, 5}, offsetBlock}}                                                       ,
        { "CACHE DHIN",   { cacheBlock          , baseBlock        , Block{0b11010, 5}, offsetBlock}}                                                       ,
        { "CACHE DHWBIN", { cacheBlock          , baseBlock        , Block{0b11000, 5}, offsetBlock}}                                                       ,
        { "CACHE DHWOIN", { cacheBlock          , baseBlock        , Block{0b11100, 5}, offsetBlock}}                                                       ,
        { "CACHE DXIN",   { cacheBlock          , baseBlock        , Block{0b10110, 5}, offsetBlock}}                                                       ,
        { "CACHE DXLDT",  { cacheBlock          , baseBlock        , Block{0b10001, 5}, offsetBlock}}                                                       ,
        { "CACHE DXLTG",  { cacheBlock          , baseBlock        , Block{0b10000, 5}, offsetBlock}}                                                       ,
        { "CACHE DXSDT",  { cacheBlock          , baseBlock        , Block{0b10011, 5}, offsetBlock}}                                                       ,
        { "CACHE DXSTG",  { cacheBlock          , baseBlock        , Block{0b10010, 5}, offsetBlock}}                                                       ,
        { "CACHE DXWBIN", { cacheBlock          , baseBlock        , Block{0b10100, 5}, offsetBlock}}                                                       ,
        { "CACHE IFL",    { cacheBlock          , baseBlock        , Block{0b01110, 5}, offsetBlock}}                                                       ,
        { "CACHE IHIN",   { cacheBlock          , baseBlock        , Block{0b01011, 5}, offsetBlock}}                                                       ,
        { "CACHE IXIN",   { cacheBlock          , baseBlock        , Block{0b00111, 5}, offsetBlock}}                                                       ,
        { "CACHE IXLDT",  { cacheBlock          , baseBlock        , Block{0b00001, 5}, offsetBlock}}                                                       ,
        { "CACHE IXLTG",  { cacheBlock          , baseBlock        , Block{0b00000, 5}, offsetBlock}}                                                       ,
        { "CACHE IXSDT",  { cacheBlock          , baseBlock        , Block{0b00101, 5}, offsetBlock}}                                                       ,
        { "CACHE IXSTG",  { cacheBlock          , baseBlock        , Block{0b00100, 5}, offsetBlock}}                                                       ,
        { "BGEZ",         { RegimmBlock         , rsBlock          , Block(0b00001, 5), offsetBlock}}                                                       ,
        { "BGEZAL",       { RegimmBlock         , rsBlock          , Block{0b10001, 5}, offsetBlock}}                                                       ,
        { "BGEZALL",      { RegimmBlock         , rsBlock          , Block{0b10011, 5}, offsetBlock}}                                                       ,
        { "BGEZL",        { RegimmBlock         , rsBlock          , Block{0b00011, 5}, offsetBlock}}                                                       ,
        { "BLTZ",         { RegimmBlock         , rsBlock          , Block{0b00000, 5}, offsetBlock}}                                                       ,
        { "BLTZAL",       { RegimmBlock         , rsBlock          , Block{0b10000, 5}, offsetBlock}}                                                       ,
        { "BLTZALL",      { RegimmBlock         , rsBlock          , Block{0b10010, 5}, offsetBlock}}                                                       ,
        { "BLTZL",        { RegimmBlock         , rsBlock          , Block{0b00010, 5}, offsetBlock}}                                                       ,
        { "TEQI",         { RegimmBlock         , rsBlock          , Block{0b01100, 5}, immediateBlock}}                                                    ,
        { "TGEI",         { RegimmBlock         , rsBlock          , Block{0b01000, 5}, immediateBlock}}                                                    ,
        { "TGEIU",        { RegimmBlock         , rsBlock          , Block{0b01001, 5}, immediateBlock}}                                                    ,
        { "TLTI",         { RegimmBlock         , rsBlock          , Block{0b01010, 5}, immediateBlock}}                                                    ,
        { "TLTIU",        { RegimmBlock         , rsBlock          , Block{0b01011, 5}, immediateBlock}}                                                    ,
        { "TNEI",         { RegimmBlock         , rsBlock          , Block{0b01110, 5}, immediateBlock}}                                                    ,
        { "MTSAB",        { RegimmBlock         , rsBlock          , Block{0b11000, 5}, immediateBlock}}                                                    ,
        { "MTSAH",        { RegimmBlock         , rsBlock          , Block{0b11001, 5}, immediateBlock}}                                                    ,
        { "BC0F",         { cop0Block           , bc0Block         , Block{0b00000, 5}, offsetBlock}}                                                       ,
        { "BC0FL",        { cop0Block           , bc0Block         , Block{0b00010, 5}, offsetBlock}}                                                       ,
        { "BC0T",         { cop0Block           , bc0Block         , Block{0b00001, 5}, offsetBlock}}                                                       ,
        { "BC0TL",        { cop0Block           , bc0Block         , Block{0b00011, 5}, offsetBlock}}                                                       ,
        { "DI",           { cop0Block           , c0Block          , Block{0, 15}                                                 , Block{0b111001, 6}}}    ,
        { "EI",           { cop0Block           , c0Block          , Block{0, 15}                                                 , Block{0b111000, 6}}}    ,
        { "ERET",         { cop0Block           , c0Block          , Block{0, 15}                                                 , Block{0b011000, 6}}}    ,
        { "TLBP",         { cop0Block           , c0Block          , Block{0, 15}                                                 , Block{0b001000, 6}}}    ,
        { "TLBR",         { cop0Block           , c0Block          , Block{0, 15}                                                 , Block{0b000001, 6}}}    ,
        { "TLBWI",        { cop0Block           , c0Block          , Block{0, 15}                                                 , Block{0b000010, 6}}}    ,
        { "TLBWR",        { cop0Block           , c0Block          , Block{0, 15}                                                 , Block{0b000110, 6}}}    ,
        { "MFPC",         { cop0Block           , mf0Block         , rtBlock          , Block{0b11001, 5} , zeroBlock             , regBlock, Block{1, 1}}} ,
        { "MFPS",         { cop0Block           , mf0Block         , rtBlock          , Block{0b11001, 5} , zeroBlock             , regBlock, Block{0, 1}}} ,
        { "MTPC",         { cop0Block           , mt0Block         , rtBlock          , Block{0b11001, 5} , zeroBlock             , regBlock, Block{1, 1}}} ,
        { "MTPS",         { cop0Block           , mt0Block         , rtBlock          , Block{0b11001, 5} , zeroBlock             , regBlock, Block{0, 1}}} ,
        { "MFBPC",        { cop0Block           , mf0Block         , rtBlock          , Block{0b11000, 5} , Block{0b00000000000, 11}}}                      ,
        { "MFDAB",        { cop0Block           , mf0Block         , rtBlock          , Block{0b11000, 5} , Block{0b00000000100, 11}}}                      ,
        { "MFDABM",       { cop0Block           , mf0Block         , rtBlock          , Block{0b11000, 5} , Block{0b00000000101, 11}}}                      ,
        { "MFDVB",        { cop0Block           , mf0Block         , rtBlock          , Block{0b11000, 5} , Block{0b00000000110, 11}}}                      ,
        { "MFDVBM",       { cop0Block           , mf0Block         , rtBlock          , Block{0b11000, 5} , Block{0b00000000111, 11}}}                      ,
        { "MFIAB",        { cop0Block           , mf0Block         , rtBlock          , Block{0b11000, 5} , Block{0b00000000010, 11}}}                      ,
        { "MFIABM",       { cop0Block           , mf0Block         , rtBlock          , Block{0b11000, 5} , Block{0b00000000011, 11}}}                      ,
        { "MTBPC",        { cop0Block           , mt0Block         , rtBlock          , Block{0b11000, 5} , Block{0b00000000000, 11}}}                      ,
        { "MTDAB",        { cop0Block           , mt0Block         , rtBlock          , Block{0b11000, 5} , Block{0b00000000100, 11}}}                      ,
        { "MTDABM",       { cop0Block           , mt0Block         , rtBlock          , Block{0b11000, 5} , Block{0b00000000101, 11}}}                      ,
        { "MTDVB",        { cop0Block           , mt0Block         , rtBlock          , Block{0b11000, 5} , Block{0b00000000110, 11}}}                      ,
        { "MTDVBM",       { cop0Block           , mt0Block         , rtBlock          , Block{0b11000, 5} , Block{0b00000000111, 11}}}                      ,
        { "MTIAB",        { cop0Block           , mt0Block         , rtBlock          , Block{0b11000, 5} , Block{0b00000000010, 11}}}                      ,
        { "MTIABM",       { cop0Block           , mt0Block         , rtBlock          , Block{0b11000, 5} , Block{0b00000000011, 11}}}                      ,
        { "MFC0",         { cop0Block           , mf0Block         , rtBlock          , rdBlock           , Block{0b00000000000, 11}}}                      ,
        { "MTC0",         { cop0Block           , mt0Block         , rtBlock          , rdBlock           , Block{0b00000000000, 11}}}                      ,
        { "BC1F",         { cop1Block           , bc1Block         , Block{0b00000, 5}, offsetBlock}}                                                       ,
        { "BC1FL",        { cop1Block           , bc1Block         , Block{0b00010, 5}, offsetBlock}}                                                       ,
        { "BC1T",         { cop1Block           , bc1Block         , Block{0b00001, 5}, offsetBlock}}                                                       ,
        { "BC1TL",        { cop1Block           , bc1Block         , Block{0b00011, 5}, offsetBlock}}                                                       ,
        { "CFC1",         { cop1Block           , Block{0b00010, 5}, rtBlock          , fsBlock           , Block{0b00000000000, 11}}}                      ,
        { "CTC1",         { cop1Block           , Block{0b00110, 5}, rtBlock          , fsBlock           , Block{0b00000000000, 11}}}                      ,
        { "MFC1",         { cop1Block           , Block{0b00000, 5}, rtBlock          , fsBlock           , Block{0b00000000000, 11}}}                      ,
        { "MTC1",         { cop1Block           , Block{0b00100, 5}, rtBlock          , fsBlock           , Block{0b00000000000, 11}}}                      ,
        { "ABS.S",        { cop1Block           , sBlock           , zeroBlock        , fsBlock           , fdBlock               , Block{0b000101, 6}}}    ,
        { "CVT.S.W",      { cop1Block           , wBlock           , zeroBlock        , fsBlock           , fdBlock               , Block{0b100000, 6}}}    ,
        { "CVT.W.S",      { cop1Block           , sBlock           , zeroBlock        , fsBlock           , fdBlock               , Block{0b100100, 6}}}    ,
        { "MOV.S",        { cop1Block           , sBlock           , zeroBlock        , fsBlock           , fdBlock               , Block{0b000110, 6}}}    ,
        { "NEG.S",        { cop1Block           , sBlock           , zeroBlock        , fsBlock           , fdBlock               , Block{0b000111, 6}}}    ,
        { "ADDA.S",       { cop1Block           , sBlock           , ftBlock          , fsBlock           , zeroBlock             , Block{0b011000, 6}}}    ,
        { "MADDA.S",      { cop1Block           , sBlock           , ftBlock          , fsBlock           , zeroBlock             , Block{0b011110, 6}}}    ,
        { "MSUBA.S",      { cop1Block           , sBlock           , ftBlock          , fsBlock           , zeroBlock             , Block{0b011111, 6}}}    ,
        { "MULA.S",       { cop1Block           , sBlock           , ftBlock          , fsBlock           , zeroBlock             , Block{0b011010, 6}}}    ,
        { "SUBA.S",       { cop1Block           , sBlock           , ftBlock          , fsBlock           , zeroBlock             , Block{0b011001, 6}}}    ,
        { "ADD.S",        { cop1Block           , sBlock           , ftBlock          , fsBlock           , fdBlock               , Block{0b000000, 6}}}    ,
        { "DIV.S",        { cop1Block           , sBlock           , ftBlock          , fsBlock           , fdBlock               , Block{0b000011, 6}}}    ,
        { "MADD.S",       { cop1Block           , sBlock           , ftBlock          , fsBlock           , fdBlock               , Block{0b011100, 6}}}    ,
        { "MAX.S",        { cop1Block           , sBlock           , ftBlock          , fsBlock           , fdBlock               , Block{0b101000, 6}}}    ,
        { "MIN.S",        { cop1Block           , sBlock           , ftBlock          , fsBlock           , fdBlock               , Block{0b101001, 6}}}    ,
        { "MSUB.S",       { cop1Block           , sBlock           , ftBlock          , fsBlock           , fdBlock               , Block{0b011101, 6}}}    ,
        { "MUL.S",        { cop1Block           , sBlock           , ftBlock          , fsBlock           , fdBlock               , Block{0b000010, 6}}}    ,
        { "RSQRT.S",      { cop1Block           , sBlock           , ftBlock          , fsBlock           , fdBlock               , Block{0b010110, 6}}}    ,
        { "SQRT.S",       { cop1Block           , sBlock           , ftBlock          , zeroBlock         , fdBlock               , Block{0b000100, 6}}}    ,
        { "SUB.S",        { cop1Block           , sBlock           , ftBlock          , fsBlock           , fdBlock               , Block{0b000001, 6}}}    ,
        { "C.EQ.S",       { cop1Block           , sBlock           , ftBlock          , fsBlock           , zeroBlock             , fcBlock, Block{0, 1}, Block{0b01, 2}, Block{0, 1}} },
        { "C.F.S",        { cop1Block           , sBlock           , ftBlock          , fsBlock           , zeroBlock             , fcBlock, Block{0, 1}, Block{0b00, 2}, Block{0, 1}} },
        { "C.LE.S",       { cop1Block           , sBlock           , ftBlock          , fsBlock           , zeroBlock             , fcBlock, Block{0, 1}, Block{0b11, 2}, Block{0, 1}} },
        { "C.LT.S",       { cop1Block           , sBlock           , ftBlock          , fsBlock           , zeroBlock             , fcBlock, Block{0, 1}, Block{0b10, 2}, Block{0, 1}} },
};
