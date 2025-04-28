
#ifndef TYPES_H
#define TYPES_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

using SignedInt = std::int64_t;
using UnsignedInt = std::uint64_t;
using RealType = long double;

enum class TokenSymbol : UnsignedInt
{
    BCDSym,
    BreakSym,
    CaseSym,
    CatchSym,
    ClassSym,
    ConstSym,
    ContinueSym,
    DefaultSym,
    DeleteSym,
    DoSym,
    ElseSym,
    EnumSym,
    ExternSym,
    ForSym,
    FriendSym,
    GotoSym,
    IfSym,
    InlineSym,
    NewSym,
    OperatorSym,
    PrivateSym,
    ProtectedSym,
    PublicSym,
    RegisterSym,
    ReturnSym,
    SignedSym,
    SizeOfSym,
    StaticSym,
    StructSym,
    SwitchSym,
    ThisSym,
    ThrowSym,
    TrySym,
    TypeDefSym,
    UnionSym,
    UnsignedSym,
    VirtualSym,
    VoidSym,
    VolatileSym,
    WhileSym,
    WithSym,
    IdentifierSym,
    NumberSym,
    NoTokenSym,
    PlusSym,
    MinusSym,
    MulSym,
    DivSymbol,
    AssignPlusSym,
    AssignMinusSym,
    AssignMulSym,
    AssignDivSym,
    LowerThan,
    GreaterThan,
    LeftParSym,
    RightParSym,
    EqualSym,
    NegativeSym,
    LowerOrEqualSym,
    GreaterOrEqualSym,
    CommaSym,
    SemicolonSym,
    LeftCurlySym,
    RightCurlySym,
    NotSym,
    OrSym,
    BitOrSym,
    AssignSym,
    AndSym,
    BitAndSym,
    BitXorSym,
    ModSym,
    AssignBitAndSym,
    AssignBitOrSym,
    AssignBitXorSym,
    AssignModuloSym,
    TildeSym,
    LeftSquareSym,
    RightSquareSym,
    SQuote,
    DQuote,
    IncSym,
    DecSym,
    ShlSym,
    ShrSym,
    QuestionSym,
    ColonSym,
    PointSym,
    PowerSym,
    AssignShlSym,
    AssignShrSym,
    ArrowSym,
    StrPtrSym,

    SignedCharSym,
    UnsignedCharSym,

    SignedShortIntSym,
    UnsignedShortIntSym,

    SignedIntSym,
    UnsignedIntSym,

    SignedLongIntSym,
    UnsignedLongIntSym,

    FloatSym,
    DoubleSym,
    LongDoubleSym,
    ExtendedSym
};

const std::unordered_map<std::string, TokenSymbol> KeyWords =
{
    { "bcd", TokenSymbol::BCDSym },
    { "break", TokenSymbol::BreakSym },
    { "case", TokenSymbol::CaseSym },
    { "catch", TokenSymbol::CatchSym },
    { "class", TokenSymbol::ClassSym },
    { "const", TokenSymbol::ConstSym },
    { "continue", TokenSymbol::ContinueSym },
    { "default", TokenSymbol::DefaultSym },
    { "delete", TokenSymbol::DeleteSym },
    { "do", TokenSymbol::DoSym },
    { "else", TokenSymbol::ElseSym },
    { "enum", TokenSymbol::EnumSym },
    { "extern", TokenSymbol::ExternSym },
    { "for", TokenSymbol::ForSym },
    { "friend", TokenSymbol::FriendSym },
    { "goto", TokenSymbol::GotoSym },
    { "if", TokenSymbol::IfSym },
    { "inline", TokenSymbol::InlineSym },
    { "new", TokenSymbol::NewSym },
    { "operator", TokenSymbol::OperatorSym },
    { "private", TokenSymbol::PrivateSym },
    { "protected", TokenSymbol::ProtectedSym },
    { "public", TokenSymbol::PublicSym },
    { "register", TokenSymbol::RegisterSym},
    { "return", TokenSymbol::ReturnSym },
    { "signed", TokenSymbol::SignedSym },
    { "sizeof", TokenSymbol::SizeOfSym },
    { "static", TokenSymbol::StaticSym },
    { "struct", TokenSymbol::StructSym },
    { "switch", TokenSymbol::SwitchSym },
    { "this", TokenSymbol::ThisSym },
    { "throw", TokenSymbol::ThrowSym },
    { "try", TokenSymbol::TrySym },
    { "typedef", TokenSymbol::TypeDefSym },
    { "union", TokenSymbol::UnionSym },
    { "unsigned", TokenSymbol::UnsignedSym },
    { "virtual", TokenSymbol::VirtualSym },
    { "void", TokenSymbol::VoidSym },
    { "volatile", TokenSymbol::VolatileSym },
    { "while", TokenSymbol::WhileSym },

    { "char", TokenSymbol::SignedCharSym },
    { "signed char", TokenSymbol::SignedCharSym },
    { "unsigned char", TokenSymbol::UnsignedCharSym },

    { "short", TokenSymbol::SignedShortIntSym },
    { "short int", TokenSymbol::SignedShortIntSym },
    { "signed short int", TokenSymbol::SignedShortIntSym },
    { "short signed int", TokenSymbol::SignedShortIntSym },

    { "unsigned short int", TokenSymbol::UnsignedShortIntSym },
    { "short unsigned int", TokenSymbol::UnsignedShortIntSym },

    { "int", TokenSymbol::SignedIntSym },
    { "signed int", TokenSymbol::SignedIntSym },

    { "unsigned", TokenSymbol::UnsignedIntSym },
    { "unsigned int", TokenSymbol::UnsignedIntSym },

    { "long", TokenSymbol::SignedLongIntSym },
    { "long int", TokenSymbol::SignedLongIntSym },
    { "signed long int",TokenSymbol::SignedLongIntSym },
    { "long signed int", TokenSymbol::SignedLongIntSym },

    { "unsigned long int", TokenSymbol::UnsignedLongIntSym },
    { "long unsigned int", TokenSymbol::UnsignedLongIntSym },

    { "float", TokenSymbol::FloatSym },
    { "double", TokenSymbol::DoubleSym },
    { "long double", TokenSymbol::LongDoubleSym },

    { "extended", TokenSymbol::ExtendedSym },
};

struct CallDefinition
{
    std::string StringCode;
    UnsignedInt NumberOfOverload;
};

struct GlobalDefinition
{
    RealType InitialValue;
    std::string Name;
    UnsignedInt DataSize;
    UnsignedInt TypeNumericCode;
};

struct AddressDefinition
{
    UnsignedInt NumericCode;
    UnsignedInt Address;
};

struct LexicalAnalysisTokenSymbol
{
    RealType Type;
    TokenSymbol Symbol;
    UnsignedInt LineInFile;
    UnsignedInt CurrentChar;
};

enum class VirtualCommandName : UnsignedInt
{
    GET,
    LDP,
    LDC,
    LDV,
    SVV,
    LDPTROFV,
    SVVFPTR,
    LDVFPTR,
    LOAD,
    LOADPUSH,
    OPR,
    CALL,
    JMP,
    JCON,
    JGOTO,
    JCONT,
    JBREAK,
    VIRTCALL,
    VIRTRET,
    FILLOPR,
    PUSHEDX,
    POPEDX,
    POPEDXPUSH,
    POPEDX0,
    STACK0,
    NUL,
    NOP,
    NTH,
    END,
    RET,
    PRINT,
};

enum class VirtualCommandDataType : SignedInt
{
    SIGNED_CHAR_TYPE = 301,
    UNSIGNED_CHAR_TYPE = 302,

    SIGNED_SHORT_TYPE = 303,
    UNSIGNED_SHORT_TYPE = 304,

    SIGNED_INT_TYPE = 305,
    UNSIGNED_INT_TYPE = 306,

    SIGNED_LONG_INT_TYPE = 307,
    UNSIGNED_LONG_INT_TYPE = 308,

    FLOAT_TYPE = 311,
    DOUBLE_TYPE = 312,
    LONG_DOUBLE_TYPE = 313,
    EXTENDED_TYPE = 314,

    BCD_TYPE = 319,
    VOID_TYPE = 320
};

enum class VirtualCommandOperationType : SignedInt
{
    M1 = -1,
    NOP = 0,

    FREE = 0,
    RET = -1,
    END_INLINE_INSIDE_FUNCTION  = -2,

    NEG = 1,
    ADD = 2,
    SUB = 3,
    SUB_ASS = 403,
    MUL = 4,
    DIV = 5,
    DIV_ASS = 405,
    MOD = 6,
    MOD_ASS = 406,
    NOT = 7,
    EQU = 8,
    NEQ = 9,
    AND = 113,
    OR = 114,

    INC = 17,
    DEC = 18,
    PLUS = 19,
    MINUS = 20,
    SHR = 21,
    SHR_ASS = 421,
    SHL = 22,
    SHL_ASS = 422,
    POW  = 23,
    LN = 24,
    EXP = 25,

    JB = 10,
    JAE = 11,
    JA = 12,
    JBE = 13,
    LT = 10,
    GT = 12,
    LE = 13,
    GE = 11,

    BITAND = 215,
    BITOR = 216,
    BITXOR = 217
};

struct VirtualCodeCommand
{
    VirtualCommandName CommandName;
    VirtualCommandOperationType Operation;
    VirtualCommandDataType Type;
    RealType Value;
    SignedInt Kind;
    UnsignedInt Level;
    UnsignedInt Index;
    UnsignedInt Inside;
    UnsignedInt LabelAddress;
    UnsignedInt TargetAddress;
};

struct LabelDefinition
{
    UnsignedInt LabelAddress;
};

struct ClassDefinition;
struct FunctionDefinition;
struct ArrayDefinition;

struct TypeDefinition
{
    UnsignedInt TypeNumericCode;
    /* if( !type_num ) nie obliczniowo rzeczywisty lub calkowity */
    /* type_num moze byc rowny np: INT_TYPE lub DOUBLE_TYPE */
    /* type_num moze byc rowny CLASS_TYPE i oznacza to typ_stucturalny ? */
    /* type_num moze byc rowny FUN_TYPE i oznacza to typ_funkcyjny ? */
    /* type_num moze byc rowny TAB_TYPE i oznacza to typ_tablicowy ? */
    /* type_num moze byc rowny ENUM_TYPE i oznacza to typ_wyliczneniowy ? */
    UnsignedInt Pointer;
    /* wskaznikowy czy nie , jesli tak to pointer = ile gwiazdek */
    UnsignedInt Length;
    /* ile bajtow */
    std::shared_ptr<ClassDefinition> PointerToClass;
    std::shared_ptr<FunctionDefinition> PointerToFunction;
    std::shared_ptr<ArrayDefinition> PointerToArray;
    /* if( !typnum ) to wskazuje na claso_strucure | funkje | tablice opisujaca go */
};

/* w przypadku gdy mam float* a;float* b;typedef float* pfloat; */
/* to rozbudowuje drzewo typow automatycznie o "float*" , lecz */
/* kontroluje rozbudowe = porownuje klucze gwiazdka = +10^6 */
/* i jesli sie powtarzaja to drzewo nie rosnie */
/* np : float* ma kodsym = floatsym + 10^6 */
/* np : float** ma kodsym = floatsym + 2*10^6 */

struct VariableDefinition
{
    std::shared_ptr<TypeDefinition> Type{};
    UnsignedInt Length{}; /* ile bajtow */
    UnsignedInt Kind{};
    UnsignedInt TypeAddress{};
    /* kind=1 , adres - jesli to parametr funkcji to jego numer */
    /* kind=2 , adres - jesli to zmienna lokalna to offset wzgledem 'ebp' */
    /* kind=3 , adres - jesli to globalna to adres bezwzgledny w pamieci */
    /* kind=4 , adres - jesli to zmienna obiektu to offset wzgledem poczatku klasy */
    UnsignedInt SafetyLevel{};
    /* poziom bezpieczenstwa w klasie 1-private,2-protected,3-public */

    std::vector<std::shared_ptr<VariableDefinition>> VariableOverloaded{};
    UnsignedInt Depth{}; /* poziom zagniezdzenia jesli to zmienna lokalna */

    /* ulatwienie */
    UnsignedInt IndexToDescribeInGlobalDataArray{}; /* index do tablicy opis_global */

    VariableDefinition() = default;
    VariableDefinition(const UnsignedInt AddressPar, const UnsignedInt KindPar, const std::shared_ptr<TypeDefinition>& FoundTypePar)
    {
        TypeAddress = AddressPar;
        Depth = 1; //zagniezdzenie lokalnych
        Kind = KindPar;
        Type = FoundTypePar;
        if (Type != nullptr)
            Length = Type->Length; //CORRECTA
    }
};

/* jesli mam tablice stala to do drzewa tablic */
/* jesli mam wskaznik to daj to do drzewa zmiennych */
/* w c++ rownowaznosc miedzy tablica i wsakaznikiem */
/* to znaczy ze jesli cos nie znajde w drzewie tablic mimo '[ ]' */
/* a znajde w drzewie zmiennych jako wskaznik , to wygeneruje wlasciwy kod */
struct ArrayDefinition
{
    std::shared_ptr<TypeDefinition> Type{};
    UnsignedInt Length{}; /* dlugosc absolutna */
    UnsignedInt NumberOfDimemsions{}; /* ile wymiarow , maxymalnie 10 */
    std::array<UnsignedInt, 10> SizeForEveryDimension{}; /* tablica rozmiaru kazdego wymiaru */
    UnsignedInt ArrayAddress{}; /* adres - interpretowany jak przy zmiennych */
    UnsignedInt Kind{}; /* kind - interpretowany jak przy zmiennych */
    UnsignedInt SafetyLevel{};
    /* poziom bezpieczenstwa w klasie 1-private,2-protected,3-public */

    std::vector<std::shared_ptr<ArrayDefinition>> ArraysOverloaded{};
    UnsignedInt DepthForLocalVariable{}; /* poziom zagniezdzenia jesli to zmienna lokalna */

    /* ulatwienie */
    UnsignedInt IndexToDescribeInGlobalDataArray{}; /* index do tablicy opis_global */

    ArrayDefinition() = default;
    ArrayDefinition(const UnsignedInt AddressPar, const std::shared_ptr<TypeDefinition>& TypePar) : ArrayAddress(AddressPar), Type(TypePar)
    {
    }
};

struct FunctionDefinition
{
    UnsignedInt OverloadNumber;
    /* jesli jestem przeciazona nie pierwsza to ktora w tablicy */
    //UnsignedInt licznik_przeciazen;
    std::vector<std::shared_ptr<FunctionDefinition>> FunctionsOverloaded;
    bool IsInline;
    /* czy zawsze inline */
    UnsignedInt Declare;
    /* =-1 deklaracja( UnsignedInt,UnsignedInt ) , =0 deklaracja( UnsignedInt a,UnsignedInt b ) , =1 cialo */
    std::shared_ptr<TypeDefinition> TypeOfResult;
    std::vector<UnsignedInt> ParametersTypes;

    std::unordered_map<UnsignedInt, std::shared_ptr<LabelDefinition>> LabelsContainer;
    std::unordered_map<UnsignedInt, std::shared_ptr<VariableDefinition>> VariablesContainer;
    std::unordered_map<UnsignedInt, std::shared_ptr<ArrayDefinition>> ArraysContainer;

    /* przeszukac 3 lub 4 drzewa : local , global i ewen. od klasy */
    /* Treestr.ptr_struct wskazuje na var_def , w nim jest wsakzany typ */
    UnsignedInt Kind;
    /* rodzaj funkcji w klasie np: -1 to operator , */
    /* 0-normal,1-virtual,2-static,3-constructor,4-destructor */
    UnsignedInt VirtualAddress; /* adres funkcji w kodzie wirtualnym */
    UnsignedInt VirtualLength; /* dlugosc funkcji w kodzie wirtualnym */
    UnsignedInt SafetyLevel;
    /* poziom bezpieczenstwa w klasie 1-private,2-protected,3-public */
    void* RealAddressInMemory; /* adres rzeczywisty w pamieci - usunac ? */
    UnsignedInt Length; /* dlugosc funkcji w kodzie maszynowym - usunac ? */

    std::string FunctionNameString; /* opis funkcji w drzewie stringow */
};

/* struktura rozni sie od klasy domyslnoscia skladowych */
/* classa domyslnie prywatne , a struktura publiczne */
struct ClassDefinition
{
    UnsignedInt NumberOfClassFields; /* ile danych */
    UnsignedInt ClassAddress;
    /* adres w pamieci - MOZE nie potrzbny bo to tylko TYP */
    /* natomiast zmienna tego typu juz posiada adres */
    UnsignedInt Length; /* ile bajtow */

    std::unordered_map<UnsignedInt, std::shared_ptr<FunctionDefinition>> FunctionsContainer;
    std::unordered_map<UnsignedInt, std::shared_ptr<VariableDefinition>> VariablesContainer;
    std::unordered_map<UnsignedInt, std::shared_ptr<ArrayDefinition>> ArraysContainer;
    std::unordered_map<UnsignedInt, std::shared_ptr<TypeDefinition>> TypesContainer; /* na podklasy */

    //class_definition* inheritance[ ];/* tablica klas dziedziczonych */
    std::vector<UnsignedInt> DataPointers; /* tablica wskaznikow danych */
    std::vector<UnsignedInt> DataTypes;
    /* tablica typow danych - by moc wyliczyc przesuniecie gdy klasa.pole */
    std::vector<UnsignedInt> DataSafety;
    /* tablica poziomu ochrony danych - moze usunac */

    std::string ClassNameString; /* opis klasy w drzewie stringow */
};

struct UnionDefinition
{
    ClassDefinition* PointerToUnion;
    /* inaczej interpretowana przez kompiltor */
};

struct FreeRegistersDefinition
{
    char UsedEAX;
    char UsedEBX;
    char UsedECX;
    char UsedEdDX;
    char UsedEDI;
    char UsedESI;
    char UsedEBP;
    char UsedESP;
    char UsedAX;
    char UsedBX;
    char UsedCX;
    char UsedDX;
    char UsedSI;
    char UsedDD;
    char UsedAL;
    char UsedBL;
    char UsedCL;
    char UsedDL;
    char UsedAH;
    char UsedBH;
    char UsedCH;
    char UsedDH;
};

enum class Object : UnsignedInt
{
    Constants,
    Variable,
    Function,
    Array,
    Structure,
    Label,
    Class
};

#endif