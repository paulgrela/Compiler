
#include <cstdio>

#include "Constants.h"

#include "Types.h"
#include "Constants.h"
#include "ParserToVirtualCodeGenerator.h"

using namespace std;

#define printff if(VerbosePrintingOfInformation) printf

void ParserToVirtualCodeGenerator::ChangeCodeForOptimizingJumps()
{
    for (UnsignedInt GeneratedVirtualCodeCommandIndex = 0; GeneratedVirtualCodeCommandIndex < NumberOfGeneratedVirtualCodeCommands; GeneratedVirtualCodeCommandIndex++)
        if (GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].CommandName == VirtualCommandName::JCON)
            while (GeneratedVirtualCode[GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].TargetAddress].CommandName == VirtualCommandName::JCON)
            {
                if (GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].Type == GeneratedVirtualCode[GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].TargetAddress].Type)
                    GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].TargetAddress = GeneratedVirtualCode[GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].TargetAddress].TargetAddress;
                else
                    GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].TargetAddress++;
            }
}

shared_ptr<TypeDefinition> ParserToVirtualCodeGenerator::FindType(const UnsignedInt TypeCode) const
{
    shared_ptr<TypeDefinition> FoundLocalTypeObject = nullptr;

    if (InsideClassesNestedLevel > 0)
    {
        if (auto TypeIterator = ProgramClassPointer->TypesContainer.find(TypeCode); TypeIterator != ProgramClassPointer->TypesContainer.end())
            FoundLocalTypeObject = TypeIterator->second;
        else
        {
            if (TypeIterator = ProgramPointer->TypesContainer.find(TypeCode); TypeIterator != ProgramPointer->TypesContainer.end())
                FoundLocalTypeObject = TypeIterator->second;
        }
    }
    if (InsideClassesNestedLevel == 0)
        if (auto TypeIterator = ProgramPointer->TypesContainer.find(TypeCode); TypeIterator != ProgramPointer->TypesContainer.end())
            FoundLocalTypeObject = TypeIterator->second;

    return FoundLocalTypeObject;
}

shared_ptr<FunctionDefinition> ParserToVirtualCodeGenerator::FindFunction(const UnsignedInt TypeCode, UnsignedInt& Level) const
{
    shared_ptr<FunctionDefinition> fptr = nullptr;
    if (InsideClassesNestedLevel > 0)
    {
        Level = CLASS_LEVEL;
        if (auto FunctionsIterator = ProgramClassPointer->FunctionsContainer.find(TypeCode); FunctionsIterator != ProgramClassPointer->FunctionsContainer.end())
            fptr = FunctionsIterator->second;
    }
    if (fptr == nullptr)
    {
        Level = GLOBAL_LEVEL;
        if (auto FunctionsIterator = ProgramPointer->FunctionsContainer.find(TypeCode); FunctionsIterator != ProgramPointer->FunctionsContainer.end())
            fptr = FunctionsIterator->second;
    }
    return fptr;
}

shared_ptr<ArrayDefinition> ParserToVirtualCodeGenerator::FindArray(const UnsignedInt ArrayTypeCode, UnsignedInt& Level) const
{
    Level = LOCAL_LEVEL;
    if (auto ArraysIterator = ProgramFunctionPointer->ArraysContainer.find(ArrayTypeCode); ArraysIterator != ProgramFunctionPointer->ArraysContainer.end())
        return (ArraysIterator->second);
    else
    {
        if (InsideClassesNestedLevel > 0)
            if (ArraysIterator = ProgramClassPointer->ArraysContainer.find(ArrayTypeCode); ArraysIterator != ProgramClassPointer->ArraysContainer.end())
            {
                //tez odziedziczone wyszukac
                //uwzglednic poziom ochrony
                Level = CLASS_LEVEL;
                return (ArraysIterator->second);
            }
        if (ArraysIterator = ProgramPointer->ArraysContainer.find(ArrayTypeCode); ArraysIterator != ProgramPointer->ArraysContainer.end())
        {
            Level = GLOBAL_LEVEL;
            return (ArraysIterator->second);
        }
    }
    return nullptr;
}

std::shared_ptr<VariableDefinition> ParserToVirtualCodeGenerator::FindVariable(const UnsignedInt VariableTypeCode, UnsignedInt& Level) const
{
    Level = LOCAL_LEVEL;
    if (auto VariablesIterator = ProgramFunctionPointer->VariablesContainer.find(VariableTypeCode); VariablesIterator != ProgramFunctionPointer->VariablesContainer.end())
        return (VariablesIterator->second);
    else
    {
        if (InsideClassesNestedLevel > 0)
            if (VariablesIterator = ProgramClassPointer->VariablesContainer.find(VariableTypeCode); VariablesIterator != ProgramClassPointer->VariablesContainer.end())
            {
                //tez odziedziczone wyszukac
                //uwzglednic poziom ochrony
                Level = CLASS_LEVEL;
                return (VariablesIterator->second);
            }
        if (VariablesIterator = ProgramPointer->VariablesContainer.find(VariableTypeCode); VariablesIterator != ProgramPointer->VariablesContainer.end())
        {
            Level = GLOBAL_LEVEL;
            return (VariablesIterator->second);
        }
    }
    return nullptr;
}

shared_ptr<TypeDefinition> ParserToVirtualCodeGenerator::CreateDescriptionOfType(const UnsignedInt TypeNumericCode, const UnsignedInt TypeLength, const UnsignedInt PointerPar, const shared_ptr<ClassDefinition>& ClassPointer, const shared_ptr<FunctionDefinition>& FunctionPointer, const shared_ptr<ArrayDefinition>& ArrayPointer)
{
    auto typer = make_shared<TypeDefinition>();
    typer->TypeNumericCode = TypeNumericCode;
    typer->Pointer = PointerPar;
    typer->Length = TypeLength;
    typer->PointerToClass = ClassPointer;
    typer->PointerToFunction = FunctionPointer;
    typer->PointerToArray = ArrayPointer;
    return typer;
}

void ParserToVirtualCodeGenerator::InitializeType(TokenSymbol TypeTokenSymbol, const VirtualCommandDataType TypeNumericCode, const UnsignedInt TypeLength, const UnsignedInt Pointer, const shared_ptr<ClassDefinition>& ClassPointer, const shared_ptr<FunctionDefinition>& FunctionPointer, const shared_ptr<ArrayDefinition>& ArrayPointer) const
{
    ProgramClassPointer->TypesContainer[static_cast<UnsignedInt>(TypeTokenSymbol)] = CreateDescriptionOfType(static_cast<UnsignedInt>(TypeNumericCode), TypeLength, Pointer, ClassPointer, FunctionPointer, ArrayPointer);
}

void ParserToVirtualCodeGenerator::InitDataForParserToVirtualCodeGenerator()
{
    VerbosePrintingOfInformation = true;
    PointerToGlobalStack = 0;
    PointerToFunctionStack = 0; //dx to jest naprawde wskaznik szczytu stosu
    NumberOfGeneratedVirtualCodeCommands = 0;
    TokenSymbolType = TokenSymbol::NoTokenSym;
    LineInFile = 1;
    GeneratedLexicalAnalysisTokenPosition = 0;
    StartAddressOfProgram = -1;
    InsideLoopsNestedLevel = 0;
    InsideFunctionsNextedLevel = 0;
    InsideClassesNestedLevel = 0;

    ProgramPointer = ProgramClassPointer = make_shared<ClassDefinition>();

    InitializeType(TokenSymbol::SignedCharSym, VirtualCommandDataType::SIGNED_CHAR_TYPE, 1, 0, nullptr, nullptr, nullptr);
    InitializeType(TokenSymbol::UnsignedCharSym, VirtualCommandDataType::UNSIGNED_CHAR_TYPE, 1, 0, nullptr, nullptr, nullptr);

    InitializeType(TokenSymbol::SignedShortIntSym, VirtualCommandDataType::SIGNED_SHORT_TYPE, 2, 0, nullptr, nullptr, nullptr);
    InitializeType(TokenSymbol::UnsignedShortIntSym, VirtualCommandDataType::UNSIGNED_SHORT_TYPE, 2, 0, nullptr, nullptr, nullptr);

    InitializeType(TokenSymbol::SignedIntSym, VirtualCommandDataType::SIGNED_INT_TYPE, 4, 0, nullptr, nullptr, nullptr);
    InitializeType(TokenSymbol::UnsignedIntSym, VirtualCommandDataType::UNSIGNED_INT_TYPE, 4, 0, nullptr, nullptr, nullptr);

    InitializeType(TokenSymbol::SignedLongIntSym, VirtualCommandDataType::SIGNED_LONG_INT_TYPE, 8, 0, nullptr, nullptr, nullptr);
    InitializeType(TokenSymbol::UnsignedLongIntSym, VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE, 8, 0, nullptr, nullptr, nullptr);

    InitializeType(TokenSymbol::FloatSym, VirtualCommandDataType::FLOAT_TYPE, 4, 0, nullptr, nullptr, nullptr);
    InitializeType(TokenSymbol::DoubleSym, VirtualCommandDataType::DOUBLE_TYPE, 8, 0, nullptr, nullptr, nullptr);
    InitializeType(TokenSymbol::LongDoubleSym, VirtualCommandDataType::LONG_DOUBLE_TYPE, 10, 0, nullptr, nullptr, nullptr);
    InitializeType(TokenSymbol::ExtendedSym, VirtualCommandDataType::EXTENDED_TYPE, 10, 0, nullptr, nullptr, nullptr);

    InitializeType(TokenSymbol::BCDSym, VirtualCommandDataType::BCD_TYPE, 4, 0, nullptr, nullptr, nullptr);
    InitializeType(TokenSymbol::VoidSym, VirtualCommandDataType::VOID_TYPE, 4, 0, nullptr, nullptr, nullptr);
}

void ParserToVirtualCodeGenerator::PrintError(const string_view ErrorString)
{
    if (VerbosePrintingOfInformation)
    {
        if (const auto SymbolInt = NumericCodeToStringSymbolsMap.find(static_cast<UnsignedInt>(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type)); SymbolInt != NumericCodeToStringSymbolsMap.end())
        {
            if (InsideClassesNestedLevel == 0)
                printf("error in line %lu ( sym = %s ) : %s\n", GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].LineInFile, SymbolInt->second.c_str(), SymbolInt->second.c_str());
            if (InsideClassesNestedLevel > 0)
                printf("error in line %lu ( sym = %s )( in class %s ) : %s\n", GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].LineInFile, SymbolInt->second.c_str(), ClassName.c_str(), ErrorString.data());
        }
    }
    GetNextTokenSymbol();
}

void ParserToVirtualCodeGenerator::PrintErrorToFile(const UnsignedInt PositionInFile, const string_view ErrorString)
{
    const UnsignedInt RememberGeneratedLexicalAnalysisTokenPosition = GeneratedLexicalAnalysisTokenPosition;
    GeneratedLexicalAnalysisTokenPosition = PositionInFile;
    PrintError(ErrorString);
    GeneratedLexicalAnalysisTokenPosition = RememberGeneratedLexicalAnalysisTokenPosition;
}

void ParserToVirtualCodeGenerator::GenerateVirtualCodeCommand(const VirtualCommandName CommandNameParam, const SignedInt TypeParam, const RealType ValueParam, const UnsignedInt TargetAddressParam, const VirtualCommandOperationType OperationParam)
{
    if (NumberOfGeneratedVirtualCodeCommands > MaximalNumberOfGeneratedVirtualCodeCommands)
        PrintError("Source program is too long.");

    GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands].CommandName = CommandNameParam;
    GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands].Type = static_cast<VirtualCommandDataType>(TypeParam);
    GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands].Operation = OperationParam;
    GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands].Value = ValueParam;
    GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands].TargetAddress = TargetAddressParam;
    GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands].LabelAddress = NumberOfGeneratedVirtualCodeCommands;
    GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands].Size = 0;
    NumberOfGeneratedVirtualCodeCommands++;
}

UnsignedInt ParserToVirtualCodeGenerator::GetSizeForTokenType(TokenSymbol TokenSymbolToGetSize) const
{
    const auto TypeToGetSize = FindType(static_cast<UnsignedInt>(TokenSymbolToGetSize));
    if (TypeToGetSize)
        return TypeToGetSize->Length;
    else
        return 4;
}

shared_ptr<VariableDefinition> ParserToVirtualCodeGenerator::CreateVariable(UnsignedInt Address, UnsignedInt Kind) const
{
    auto NewVariable = make_shared<VariableDefinition>();
    NewVariable->TypeAddress = Address;
    NewVariable->Depth = 1; //zagniezdzenie lokalnych
    NewVariable->Kind = Kind;
    NewVariable->Type = FindType(static_cast<UnsignedInt>(TokenSymbolType));
    if (NewVariable->Type != nullptr)
        NewVariable->Length = NewVariable->Type->Length; //CORRECTA
    return NewVariable;
}

std::shared_ptr<ArrayDefinition> ParserToVirtualCodeGenerator::CreateArray(UnsignedInt Address, UnsignedInt TypeCode) const
{
    auto NewArray = make_shared<ArrayDefinition>();
    NewArray->Type = FindType(TypeCode);
    NewArray->ArrayAddress = Address;
    return NewArray;
}

void ParserToVirtualCodeGenerator::Expression()
{
    ExpressionAssignment();
    while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::CommaSym)
    {
        GetNextTokenSymbol();
        ExpressionAssignment();
        //gen( stack_1 , 0 , 0 );
    }
}

#define ConditionForAssignOperation( p ) \
        ( GeneratedLexicalAnalysisTokens[p].Symbol==TokenSymbol::AssignSym || GeneratedLexicalAnalysisTokens[p].Symbol==TokenSymbol::AssignModuloSym || \
          GeneratedLexicalAnalysisTokens[p].Symbol==TokenSymbol::AssignMulSym || GeneratedLexicalAnalysisTokens[p].Symbol==TokenSymbol::AssignDivSym || \
          GeneratedLexicalAnalysisTokens[p].Symbol==TokenSymbol::AssignPlusSym || GeneratedLexicalAnalysisTokens[p].Symbol==TokenSymbol::AssignMinusSym || \
          GeneratedLexicalAnalysisTokens[p].Symbol==TokenSymbol::AssignBitAndSym || GeneratedLexicalAnalysisTokens[p].Symbol==TokenSymbol::AssignBitOrSym || \
          GeneratedLexicalAnalysisTokens[p].Symbol==TokenSymbol::AssignBitXorSym || GeneratedLexicalAnalysisTokens[p].Symbol==TokenSymbol::AssignShlSym || \
          GeneratedLexicalAnalysisTokens[p].Symbol==TokenSymbol::AssignShrSym)

void ParserToVirtualCodeGenerator::GenerateAssignOperation(const VirtualCommandOperationType OperationType)
{
    const UnsignedInt GeneratedVirtualCodeCommandIndex2 = NumberOfGeneratedVirtualCodeCommands - 1;
    GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, OperationType);
    if (GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex2].CommandName == VirtualCommandName::LDV)
    {
        GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands].Kind = GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex2].Kind;
        GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands].Level = GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex2].Level;
        GenerateVirtualCodeCommand(VirtualCommandName::SVV, static_cast<SignedInt>(NewLastTokenSymbolType = static_cast<TokenSymbol>(GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex2].Type)), 0, GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex2].TargetAddress);
        //GenerateVirtualCodeCommand(VirtualCommandName::SVV, NewLastTokenSymbolType = static_cast<TokenSymbol>(GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex2].Type), 0, GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex2].TargetAddress);
    }
    else
    if (GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex2].CommandName == VirtualCommandName::LDVFPTR)
    {
        //GenerateVirtualCodeCommand(VirtualCommandName::SVVFPTR, static_cast<SignedInt>(VirtualCommandDataType::SIGNED_INT_TYPE), 0, 0);
        GenerateVirtualCodeCommand(VirtualCommandName::SVVFPTR, 0, 0, 0);
        GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands].Level = GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex2].Level;
        GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands].Type = GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex2].Type;
        //GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands].Level = GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex2].Level;
    }
}

void ParserToVirtualCodeGenerator::GenerateAssignmentWithAdditionalActionOperationCode(const TokenSymbol Token, const UnsignedInt GeneratedVirtualCodeCommandIndex1)
{
    switch (Token)
    {
        case TokenSymbol::AssignPlusSym : GenerateAssignOperation(VirtualCommandOperationType::ADD); break;
        case TokenSymbol::AssignMinusSym : GenerateAssignOperation(VirtualCommandOperationType::SUB_ASS); break;
        case TokenSymbol::AssignMulSym : GenerateAssignOperation(VirtualCommandOperationType::MUL); break;
        case TokenSymbol::AssignDivSym : GenerateAssignOperation(VirtualCommandOperationType::DIV_ASS); break;
        case TokenSymbol::AssignModuloSym : GenerateAssignOperation(VirtualCommandOperationType::MOD_ASS); break;
        case TokenSymbol::AssignShrSym : GenerateAssignOperation(VirtualCommandOperationType::SHR_ASS); break;
        case TokenSymbol::AssignShlSym : GenerateAssignOperation(VirtualCommandOperationType::SHL_ASS); break;
        case TokenSymbol::AssignBitXorSym : GenerateAssignOperation(VirtualCommandOperationType::BITXOR); break;
        case TokenSymbol::AssignBitAndSym : GenerateAssignOperation(VirtualCommandOperationType::BITAND); break;
        case TokenSymbol::AssignBitOrSym : GenerateAssignOperation(VirtualCommandOperationType::BITOR); break;
        default : break;
    }
}

void ParserToVirtualCodeGenerator::GenerateAssignmentOperationCode()
{
    if (GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].CommandName == VirtualCommandName::LDV)
        GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].CommandName = VirtualCommandName::SVV;
    else
    if (GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].CommandName == VirtualCommandName::LDC)
    {
        PrintError("Can't assign value to numeric value");
        GeneratedLexicalAnalysisTokenPosition--;
    }
    else
    if (GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].CommandName == VirtualCommandName::LDVFPTR)
    {
        GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].CommandName = VirtualCommandName::NUL;
        //GenerateVirtualCodeCommand(VirtualCommandName::SVVFPTR, static_cast<SignedInt>(VirtualCommandDataType::SIGNED_INT_TYPE), 0, 0);
        GenerateVirtualCodeCommand(VirtualCommandName::SVVFPTR, 0, 0, 0);
        GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Level = GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 2].Level;
        GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Type = GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 2].Type;
    }
}

void ParserToVirtualCodeGenerator::ExpressionAssignment()
{
    array<UnsignedInt, MaximalNumberOfPossiblesAssignmentsInExpression> Positions{};
    array<TokenSymbol, MaximalNumberOfPossiblesAssignmentsInExpression> OperationsAssignment{};

    const UnsignedInt GeneratedVirtualCodeCommandIndex = NumberOfGeneratedVirtualCodeCommands;
    UnsignedInt Position = 0;
    Positions[Position] = GeneratedLexicalAnalysisTokenPosition;
    Position++;

    VerbosePrintingOfInformation = false;
    ExpressionConditional();
    while (ConditionForAssignOperation(GeneratedLexicalAnalysisTokenPosition))
    {
        OperationsAssignment[Position - 1] = GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol;
        GetNextTokenSymbol();
        Positions[Position] = GeneratedLexicalAnalysisTokenPosition;
        Position++;
        ExpressionConditional();
    }

    const UnsignedInt LastPosition = GeneratedLexicalAnalysisTokenPosition;

    NumberOfGeneratedVirtualCodeCommands = GeneratedVirtualCodeCommandIndex;
    Position--;
    GeneratedLexicalAnalysisTokenPosition = Positions[Position];
    VerbosePrintingOfInformation = true;
    ExpressionConditional();
    while (Position > 0)
    {
        Position--;
        GeneratedLexicalAnalysisTokenPosition = Positions[Position];
        ExpressionOneArgument();

        if (OperationsAssignment[Position] == TokenSymbol::AssignSym)
            GenerateAssignmentOperationCode();
        else
            GenerateAssignmentWithAdditionalActionOperationCode(OperationsAssignment[Position], GeneratedVirtualCodeCommandIndex);

        if (!ConditionForAssignOperation(GeneratedLexicalAnalysisTokenPosition))
        {
            PrintError("L-value required ");
            GeneratedLexicalAnalysisTokenPosition--;
        }
    }
    GeneratedLexicalAnalysisTokenPosition = LastPosition;
}

void ParserToVirtualCodeGenerator::ExpressionConditional()
{
    ExpressionLogicOR();
}

void ParserToVirtualCodeGenerator::ExpressionLogicOR()
{
    vector<UnsignedInt> RememberedPositions;
    ExpressionLogicAND();
    while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::OrSym)
    {
        GetNextTokenSymbol();
        RememberedPositions.emplace_back(NumberOfGeneratedVirtualCodeCommands);
        GenerateVirtualCodeCommand(VirtualCommandName::JCON, 1, 0, 0);
        ExpressionLogicAND();
    }

    for (const auto RememberedPosition : RememberedPositions)
        GeneratedVirtualCode[RememberedPosition].TargetAddress = NumberOfGeneratedVirtualCodeCommands;
}

void ParserToVirtualCodeGenerator::ExpressionLogicAND()
{
    vector<UnsignedInt> RememberPositions;
    ExpressionOR();
    while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::AndSym)
    {
        GetNextTokenSymbol();
        RememberPositions.emplace_back(NumberOfGeneratedVirtualCodeCommands);
        GenerateVirtualCodeCommand(VirtualCommandName::JCON, 0, 0, 0);
        ExpressionOR();
    }

    for (const auto RememberedPosition : RememberPositions)
        GeneratedVirtualCode[RememberedPosition].TargetAddress = NumberOfGeneratedVirtualCodeCommands;
}


void ParserToVirtualCodeGenerator::ExpressionOR()
{
    ExpressionXOR();
    while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::BitOrSym)
    {
        GetNextTokenSymbol();
        ExpressionXOR();
        GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::BITOR);
    }
}

void ParserToVirtualCodeGenerator::ExpressionXOR()
{
    ExpressionAND();
    while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::BitXorSym)
    {
        GetNextTokenSymbol();
        ExpressionAND();
        GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::BITXOR);
    }
}

void ParserToVirtualCodeGenerator::ExpressionAND()
{
    ExpressionComparison();
    while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::BitAndSym)
    {
        GetNextTokenSymbol();
        ExpressionComparison();
        GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::BITAND);
    }
}

void ParserToVirtualCodeGenerator::ExpressionComparison()
{
    const UnsignedInt GeneratedVirtualCodeCommandIndex = NumberOfGeneratedVirtualCodeCommands;
    ExpressionRelation();
    while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::EqualSym || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::NegativeSym)
    {
        //czy nie za duzo push_edx 2 x ?
        bool MakePush = false;
        const UnsignedInt GeneratedVirtualCodeCommandIndex2 = NumberOfGeneratedVirtualCodeCommands;
        if (GeneratedVirtualCodeCommandIndex2 - GeneratedVirtualCodeCommandIndex > 1)
        {
            GenerateVirtualCodeCommand(VirtualCommandName::PUSHEDX, 0, 0, 0);
            MakePush = true;
        }
        TokenSymbol OperationComparison = GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol;
        GetNextTokenSymbol();

        if (!MakePush)
            GenerateVirtualCodeCommand(VirtualCommandName::PUSHEDX, 0, 0, 0);

        const UnsignedInt GeneratedVirtualCodeCommandIndex3 = NumberOfGeneratedVirtualCodeCommands;
        ExpressionRelation();
        const UnsignedInt GeneratedVirtualCodeCommandIndex4 = NumberOfGeneratedVirtualCodeCommands;

        if (!MakePush)
            if (GeneratedVirtualCodeCommandIndex4 - GeneratedVirtualCodeCommandIndex3 == 1)
                GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex3 - 1].CommandName = VirtualCommandName::NTH;

        if (!MakePush)
            if (GeneratedVirtualCodeCommandIndex4 - GeneratedVirtualCodeCommandIndex3 > 1)
                GenerateVirtualCodeCommand(VirtualCommandName::POPEDX, 0, 0, 0);

        if (GeneratedVirtualCodeCommandIndex2 - GeneratedVirtualCodeCommandIndex > 1)
            GenerateVirtualCodeCommand(VirtualCommandName::POPEDX, 0, 0, 0);

        if (OperationComparison == TokenSymbol::EqualSym)
            GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::EQU);
        else
        if (OperationComparison == TokenSymbol::NegativeSym)
            GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::NEQ); //tu mozna uproscic
    }
}

void ParserToVirtualCodeGenerator::ExpressionRelation()
{
    const UnsignedInt GeneratedVirtualCodeCommandIndex = NumberOfGeneratedVirtualCodeCommands;
    ExpressionShift();
    while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::LowerThan || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::GreaterThan || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::LowerOrEqualSym || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::GreaterOrEqualSym)
    {
        UnsignedInt MakePush = false;
        const UnsignedInt GeneratedVirtualCodeCommandIndex2 = NumberOfGeneratedVirtualCodeCommands;
        if (GeneratedVirtualCodeCommandIndex2 - GeneratedVirtualCodeCommandIndex > 1)
        {
            GenerateVirtualCodeCommand(VirtualCommandName::PUSHEDX, 0, 0, 0);
            MakePush = true;
        }
        TokenSymbol OperationRelation = GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol;
        GetNextTokenSymbol();

        if (!MakePush)
            GenerateVirtualCodeCommand(VirtualCommandName::PUSHEDX, 0, 0, 0);

        UnsignedInt GeneratedVirtualCodeCommandIndex3 = NumberOfGeneratedVirtualCodeCommands;
        ExpressionShift();
        UnsignedInt GeneratedVirtualCodeCommandIndex4 = NumberOfGeneratedVirtualCodeCommands;

        if (!MakePush)
            if (GeneratedVirtualCodeCommandIndex4 - GeneratedVirtualCodeCommandIndex3 == 1)
                GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex3 - 1].CommandName = VirtualCommandName::NTH;

        if (!MakePush)
            if (GeneratedVirtualCodeCommandIndex4 - GeneratedVirtualCodeCommandIndex3 > 1)
                GenerateVirtualCodeCommand(VirtualCommandName::POPEDX, 0, 0, 0);

        if (GeneratedVirtualCodeCommandIndex2 - GeneratedVirtualCodeCommandIndex > 1)
            GenerateVirtualCodeCommand(VirtualCommandName::POPEDX, 0, 0, 0);

        if (OperationRelation == TokenSymbol::LowerThan)
            GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::LT);
        else
        if (OperationRelation == TokenSymbol::GreaterThan)
            GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::GT);
        else
        if (OperationRelation == TokenSymbol::LowerOrEqualSym)
            GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::LE);
        else
        if (OperationRelation == TokenSymbol::GreaterOrEqualSym)
            GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::GE);
    }
}

void ParserToVirtualCodeGenerator::ExpressionShift()
{
    ExpressionAdditive();
    while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::ShlSym || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::ShrSym)
    {
        const TokenSymbol OperationShift = GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol;
        GetNextTokenSymbol();
        ExpressionAdditive();
        if (OperationShift == TokenSymbol::ShlSym)
            GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::SHL);
        else
        if (OperationShift == TokenSymbol::ShrSym)
            GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::SHR);
    }
}

void ParserToVirtualCodeGenerator::ExpressionAdditive()
{
    const UnsignedInt GeneratedVirtualCodeCommandIndex = NumberOfGeneratedVirtualCodeCommands;
    ExpressionMultiplicative();
    while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::PlusSym || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::MinusSym)
    {
        const TokenSymbol OperationAdditive = GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol;
        GetNextTokenSymbol();
        ExpressionMultiplicative();

        // tu moze byc blad-na razie nie ma
        if (NumberOfGeneratedVirtualCodeCommands >= 3 && GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].CommandName == VirtualCommandName::LDC && GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 3].CommandName == VirtualCommandName::LDC && GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 2].CommandName == VirtualCommandName::OPR && (GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 2].Operation == VirtualCommandOperationType::SUB || GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 2].Operation == VirtualCommandOperationType::ADD))
        {
            //printf( "TU MOZE BYC BLAD %d\n",lr );
            const auto DataType = GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Type;
            const RealType p1 = GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Value;
            const RealType p2 = GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 3].Value;
            if (GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 2].Operation == VirtualCommandOperationType::ADD)
            {
                NumberOfGeneratedVirtualCodeCommands -= 3;
                //GenerateVirtualCodeCommand(VirtualCommandName::LDC, static_cast<SignedInt>(VirtualCommandDataType::SIGNED_INT_TYPE), p1 + p2, 0);
                GenerateVirtualCodeCommand(VirtualCommandName::LDC, static_cast<SignedInt>(DataType), p1 + p2, 0);
            }
            else
            if (GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 2].Operation == VirtualCommandOperationType::SUB)
            {
                NumberOfGeneratedVirtualCodeCommands -= 3;
                //GenerateVirtualCodeCommand(VirtualCommandName::LDC, static_cast<SignedInt>(VirtualCommandDataType::SIGNED_INT_TYPE), p1 - p2, 0);
                GenerateVirtualCodeCommand(VirtualCommandName::LDC,  static_cast<SignedInt>(DataType), p1 - p2, 0);
            }
            if (OperationAdditive == TokenSymbol::PlusSym)
                GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::ADD);
            else
            if (OperationAdditive == TokenSymbol::MinusSym)
                GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::SUB);
        }
        else
        // end potencjalny blad
        if (NumberOfGeneratedVirtualCodeCommands - GeneratedVirtualCodeCommandIndex == 2 && GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].CommandName == VirtualCommandName::LDC && GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 2].CommandName == VirtualCommandName::LDC)
        {
            const auto DataType = GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Type;
            const RealType p1 = GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Value;
            const RealType p2 = GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 2].Value;
            NumberOfGeneratedVirtualCodeCommands = GeneratedVirtualCodeCommandIndex;
            if (OperationAdditive == TokenSymbol::PlusSym)
                //GenerateVirtualCodeCommand(VirtualCommandName::LDC, static_cast<SignedInt>(VirtualCommandDataType::SIGNED_INT_TYPE), p2 + p1, 0);
                GenerateVirtualCodeCommand(VirtualCommandName::LDC, static_cast<SignedInt>(DataType), p2 + p1, 0);
            else
            if (OperationAdditive == TokenSymbol::MinusSym)
                //GenerateVirtualCodeCommand(VirtualCommandName::LDC, static_cast<SignedInt>(VirtualCommandDataType::SIGNED_INT_TYPE), p2 - p1, 0);
                GenerateVirtualCodeCommand(VirtualCommandName::LDC, static_cast<SignedInt>(DataType), p2 - p1, 0);
        }
        else
        {
            if (OperationAdditive == TokenSymbol::PlusSym)
                GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::ADD);
            else
            if (OperationAdditive == TokenSymbol::MinusSym)
                GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::SUB);
        }
    }
}

void ParserToVirtualCodeGenerator::ExpressionMultiplicative()
{
    const UnsignedInt GeneratedVirtualCodeCommandIndex = NumberOfGeneratedVirtualCodeCommands;
    //wyrazenie_rzutowania();
    ExpressionPower();
    while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::MulSym || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::DivSymbol || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::ModSym)
    {
        const TokenSymbol OperationMultiplicative = GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol;
        GetNextTokenSymbol();
        //wyrazenie_rzutowania();
        ExpressionPower();
        if (NumberOfGeneratedVirtualCodeCommands - GeneratedVirtualCodeCommandIndex == 2 && GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].CommandName == VirtualCommandName::LDC && GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 2].CommandName == VirtualCommandName::LDC)
        {
            const auto DataType = GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Type;
            const RealType p1 = GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Value;
            const RealType p2 = GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 2].Value;
            NumberOfGeneratedVirtualCodeCommands = GeneratedVirtualCodeCommandIndex;
            if (OperationMultiplicative == TokenSymbol::MulSym)
                //GenerateVirtualCodeCommand(VirtualCommandName::LDC, static_cast<SignedInt>(VirtualCommandDataType::SIGNED_INT_TYPE), p2 * p1, 0);
                GenerateVirtualCodeCommand(VirtualCommandName::LDC, static_cast<SignedInt>(DataType), p2 * p1, 0);
            else
            if (OperationMultiplicative == TokenSymbol::DivSymbol)
                //GenerateVirtualCodeCommand(VirtualCommandName::LDC, static_cast<SignedInt>(VirtualCommandDataType::SIGNED_INT_TYPE), p2 / p1, 0);
                GenerateVirtualCodeCommand(VirtualCommandName::LDC, static_cast<SignedInt>(DataType), p2 / p1, 0);
            else
            if (OperationMultiplicative == TokenSymbol::ModSym)
                //GenerateVirtualCodeCommand(VirtualCommandName::LDC, static_cast<SignedInt>(VirtualCommandDataType::SIGNED_INT_TYPE), (UnsignedInt)p2 % (UnsignedInt)p1, 0);
                GenerateVirtualCodeCommand(VirtualCommandName::LDC, static_cast<SignedInt>(DataType), (UnsignedInt)p2 % (UnsignedInt)p1, 0);
        }
        else
        {
            if (OperationMultiplicative == TokenSymbol::MulSym)
                GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::MUL);
            else
            if (OperationMultiplicative == TokenSymbol::DivSymbol)
                GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::DIV);
            else
            if (OperationMultiplicative == TokenSymbol::ModSym)
                GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::MOD);
        }
    }
}

void ParserToVirtualCodeGenerator::ExpressionPower()
{
    ExpressionCast();
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::PowerSym)
    {
        GetNextTokenSymbol();
        GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::LN);
        ExpressionPower();
        GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::MUL);
        GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::EXP);
    }
}

void ParserToVirtualCodeGenerator::ExpressionCast()
{
    ExpressionOneArgument();
}

shared_ptr<TypeDefinition> ParserToVirtualCodeGenerator::CheckType(const UnsignedInt GeneratedLexicalAnalysisTokenIndex) const
{
    auto TypeToCheck = FindType(static_cast<UnsignedInt>(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenIndex + 1].Symbol));

    if (!TypeToCheck && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenIndex + 1].Symbol == TokenSymbol::IdentifierSym)
        TypeToCheck = FindType(static_cast<UnsignedInt>(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenIndex + 1].Type));

    return TypeToCheck;
}

void ParserToVirtualCodeGenerator::PointerExpressionSum(const UnsignedInt& OperationCode, const shared_ptr<VariableDefinition>& VariablePointer, UnsignedInt& InsidePointerExpression)
{
    if (OperationCode > 0)
    {
        const UnsignedInt GeneratedVirtualCodeCommandIndex = NumberOfGeneratedVirtualCodeCommands;
        Expression();
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::RightParSym)
            GetNextTokenSymbol();
        else
            PrintError("1 right paren in pointer expression is not closed");
        UnsignedInt SizeToMultiply = 0;
        if (InsidePointerExpression >= 2)
            SizeToMultiply = 4;
        if (InsidePointerExpression == 1)
        {
            UnsignedInt VariableNumericCode = VariablePointer->Type->TypeNumericCode - VariablePointer->Type->Pointer * ConstantForPointerToMultiply;
            const auto TypePtr = FindType(VariableNumericCode);
            SizeToMultiply = TypePtr->Length;
        }
        if (NumberOfGeneratedVirtualCodeCommands - GeneratedVirtualCodeCommandIndex == 1 && GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].CommandName == VirtualCommandName::LDC)
            GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Value *= SizeToMultiply;
        else
        {
            //GenerateVirtualCodeCommand(VirtualCommandName::LDC, static_cast<SignedInt>(VirtualCommandDataType::SIGNED_INT_TYPE), SizeToMultiply, 0);
            GenerateVirtualCodeCommand(VirtualCommandName::LDC, VariablePointer->Type->TypeNumericCode, SizeToMultiply, 0);

            GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::MUL);
        }
        GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::ADD);
    }
    GenerateVirtualCodeCommand(VirtualCommandName::LDVFPTR, VariablePointer->Type->TypeNumericCode, 0, 0);
    //GenerateVirtualCodeCommand(VirtualCommandName::LDVFPTR, static_cast<SignedInt>(NewLastTokenSymbolType = static_cast<TokenSymbol>(VariablePointer->Type->TypeNumericCode)), 0, 0);

    //GenerateVirtualCodeCommand(VirtualCommandName::LDVFPTR, static_cast<SignedInt>(VirtualCommandDataType::SIGNED_INT_TYPE), 0, 0);
    //GenerateVirtualCodeCommand(VirtualCommandName::LDVFPTR, static_cast<SignedInt>(VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE), 0, 0);
    //GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Level = Level;

    InsidePointerExpression--;
}

void ParserToVirtualCodeGenerator::ExpressionOneArgumentIncOrDec()
{
    const TokenSymbol OperationIncOrDecSymbol = GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol;
    const UnsignedInt GeneratedVirtualCodeCommandIndex = NumberOfGeneratedVirtualCodeCommands;
    GetNextTokenSymbol();
    ExpressionOneArgument();
    if (OperationIncOrDecSymbol == TokenSymbol::IncSym)
        GenerateVirtualCodeCommand(VirtualCommandName::OPR, GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].TargetAddress, 0, 0, VirtualCommandOperationType::INC);
    else
    if (OperationIncOrDecSymbol == TokenSymbol::DecSym)
        GenerateVirtualCodeCommand(VirtualCommandName::OPR, GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].TargetAddress, 0, 0, VirtualCommandOperationType::DEC);

    GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Kind = -100;
    GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Level = GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].Level;
    GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Index = GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].Index;
}

void ParserToVirtualCodeGenerator::ExpressionOneArgumentBitAnd()
{
    GetNextTokenSymbol();
    Expression();
    //GenerateVirtualCodeCommand(VirtualCommandName::LDPTROFV, 0, 0, GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].TargetAddress);
    GenerateVirtualCodeCommand(VirtualCommandName::LDPTROFV, static_cast<SignedInt>(GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Type), 0, GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].TargetAddress);
    GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Level = LOCAL_LEVEL;
    if (GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 2].CommandName != VirtualCommandName::LDV)
        PrintError("variable name expected after &");
    GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 2].CommandName = VirtualCommandName::NTH;
}

void ParserToVirtualCodeGenerator::ExpressionOneArgumentStrPtr()
{
    GetNextTokenSymbol();
    //GenerateVirtualCodeCommand(VirtualCommandName::LDV, static_cast<SignedInt>(NewLastTokenSymbolType = static_cast<TokenSymbol>(static_cast<UnsignedInt>(TokenSymbol::UnsignedCharSym) + ConstantForSymbolToAdd)), 0, GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition - 1].Type);
    GenerateVirtualCodeCommand(VirtualCommandName::LDV, static_cast<SignedInt>(NewLastTokenSymbolType = static_cast<TokenSymbol>(static_cast<UnsignedInt>(TokenSymbol::UnsignedCharSym) + ConstantForSymbolToAdd)), 0, GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition - 1].Type);
    GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Kind = -ConstantForSymbolToAdd;
}

void ParserToVirtualCodeGenerator::ExpressionOneArgumentIdentifier()
{
    GetNextTokenSymbol();
    UnsignedInt Level;
    if (const auto VariablePointer = FindVariable((UnsignedInt)GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type, Level))
    {
        GetNextTokenSymbol();
        //GenerateVirtualCodeCommand(VirtualCommandName::LDV, static_cast<SignedInt>(NewLastTokenSymbolType = static_cast<TokenSymbol>(VariablePointer->Type->TypeNumericCode)), 0, VariablePointer->TypeAddress - VariablePointer->Length);
        GenerateVirtualCodeCommand(VirtualCommandName::LDV, VariablePointer->Type->TypeNumericCode, 0, VariablePointer->TypeAddress - VariablePointer->Length);
        GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Kind = VariablePointer->Kind;
        GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Level = Level;
        GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Index = VariablePointer->IndexToDescribeInGlobalDataArray;
        //GenerateVirtualCodeCommand(VirtualCommandName::LDVFPTR, static_cast<SignedInt>(VirtualCommandDataType::SIGNED_INT_TYPE), 0, 0);
        //GenerateVirtualCodeCommand(VirtualCommandName::LDVFPTR, static_cast<SignedInt>(VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE), 0, 0);

        //GenerateVirtualCodeCommand(VirtualCommandName::LDVFPTR, static_cast<SignedInt>(NewLastTokenSymbolType = static_cast<TokenSymbol>(VariablePointer->Type->TypeNumericCode)), 0, 0);
        GenerateVirtualCodeCommand(VirtualCommandName::LDVFPTR, VariablePointer->Type->TypeNumericCode, 0, 0);
    }
}

void ParserToVirtualCodeGenerator::ExpressionOneArgumentMulLeftPar()
{
    std::shared_ptr<VariableDefinition> VariablePointer;
    UnsignedInt InsidePointerExpression = 0;
    while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::MulSym && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition + 1].Symbol == TokenSymbol::LeftParSym)
    {
        GetNextTokenSymbol();
        GetNextTokenSymbol();
        InsidePointerExpression++;
    }
    // NAJGLEBSZY POZIOM START
    //UnsignedInt Operation = -1;
    VirtualCommandOperationType Operation = VirtualCommandOperationType::M1;
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::IdentifierSym)
    {
        UnsignedInt Level;
        VariablePointer = FindVariable((UnsignedInt)GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type, Level);

        if (VariablePointer)
        {
            GetNextTokenSymbol();
            if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::PlusSym)
                Operation = VirtualCommandOperationType::ADD;
            else
            if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::MinusSym)
                Operation = VirtualCommandOperationType::SUB;
            else
            if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::RightParSym)
            {
                GetNextTokenSymbol();
                Operation = VirtualCommandOperationType::NOP;
            }
            else
                PrintError("1-after pointer must be '+/-' or ')' in this expression");
        }
        else
            PrintError("pointer is unknown");
        if (VariablePointer->Type->Pointer != InsidePointerExpression)
            PrintError("type of pointer is not correct in that expression");

        //if ta zmienna to tablica to tu gen( ladadr a nie gen( lad
        //zamiast suma_wyrazenia_skaznikowego to suma_wyarzenia_tablicowego

        //GenerateVirtualCodeCommand(VirtualCommandName::LDV, static_cast<SignedInt>(NewLastTokenSymbolType = static_cast<TokenSymbol>(VariablePointer->Type->TypeNumericCode)), 0, VariablePointer->TypeAddress - VariablePointer->Length);
        GenerateVirtualCodeCommand(VirtualCommandName::LDV, VariablePointer->Type->TypeNumericCode, 0, VariablePointer->TypeAddress - VariablePointer->Length);
        GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Kind = VariablePointer->Kind;
        GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Level = Level;
        GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Index = VariablePointer->IndexToDescribeInGlobalDataArray;
        PointerExpressionSum(static_cast<SignedInt>(Operation), VariablePointer, InsidePointerExpression);
    }
    //END NAJGLEBSZY POZIOM
    while (InsidePointerExpression >= 1)
    {
        Operation = VirtualCommandOperationType::NEG;
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::PlusSym)
            Operation = VirtualCommandOperationType::ADD;
        else
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::MinusSym)
            Operation = VirtualCommandOperationType::SUB;
        else
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::RightParSym)
        {
            GetNextTokenSymbol();
            Operation = VirtualCommandOperationType::NOP;
        }
        else
            PrintError("2-after pointer must be '+/-' or ')' in this expression");

        PointerExpressionSum(static_cast<SignedInt>(Operation), VariablePointer, InsidePointerExpression);
    }
}

void ParserToVirtualCodeGenerator::ExpressionOneArgumentMul()
{
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition + 1].Symbol == TokenSymbol::IdentifierSym)
        ExpressionOneArgumentIdentifier();
    else
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition + 1].Symbol == TokenSymbol::LeftParSym) //*(*(p+wyrazenie*4)+wyrazenie*size)
        ExpressionOneArgumentMulLeftPar();
}

void ParserToVirtualCodeGenerator::ExpressionOneArgumentLeftPar1()
{
    TokenSymbol RememberedTokenSymbol = GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition + 1].Symbol;
    GetNextTokenSymbol();
    GetNextTokenSymbol();
    UnsignedInt Pointer = 0;
    while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::MulSym)
    {
        GetNextTokenSymbol();
        Pointer += ConstantForSymbolToAdd;
    }
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol != TokenSymbol::RightParSym)
        PrintError("error of parsing ) type conversion");
    else
        GetNextTokenSymbol();
    ExpressionConditional();
    NewLastTokenSymbolType = static_cast<TokenSymbol>(static_cast<UnsignedInt>(RememberedTokenSymbol) + static_cast<UnsignedInt>(Pointer));
}

void ParserToVirtualCodeGenerator::ExpressionOneArgumentLeftPar2()
{
    TokenSymbol RememberedTokenSymbol = GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol;
    GetNextTokenSymbol();
    GetNextTokenSymbol();
    Expression();
    NewLastTokenSymbolType = RememberedTokenSymbol;
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol != TokenSymbol::RightParSym)
        PrintError("error of parsing ) 2");
    GetNextTokenSymbol();
}

void ParserToVirtualCodeGenerator::ExpressionOneArgumentSizeOf()
{
    GetNextTokenSymbol();
    GetNextTokenSymbol();
    auto VariableType = FindType(static_cast<UnsignedInt>(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol));
    if (!VariableType)
        PrintError("type to get_sizeof in parents is not declared");

    //GenerateVirtualCodeCommand(VirtualCommandName::LDC, static_cast<SignedInt>(VirtualCommandDataType::SIGNED_INT_TYPE), VariableType->Length, 0);
    GenerateVirtualCodeCommand(VirtualCommandName::LDC, VariableType->TypeNumericCode, VariableType->Length, 0);
    NewLastTokenSymbolType = TokenSymbol::SignedIntSym;
    GetNextTokenSymbol();
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol != TokenSymbol::RightParSym)
        PrintError("missing )");
    GetNextTokenSymbol();
}

void ParserToVirtualCodeGenerator::ExpressionOneArgumentBitAndOrPlusOrMinusOrNotOrTilde()
{
    const TokenSymbol RememberedTokenSymbol = GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol;
    GetNextTokenSymbol();
    ExpressionCast();
    if (RememberedTokenSymbol == TokenSymbol::TildeSym)
    {
        if (GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].CommandName == VirtualCommandName::LDC)
            GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Value = ~(UnsignedInt)GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Value;
        else
            GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::NEG);
    }
    else
    if (RememberedTokenSymbol == TokenSymbol::MinusSym)
    {
        if (GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].CommandName == VirtualCommandName::LDC)
            GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Value = -GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Value;
        else
            GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::MINUS);
    }
    else
    if (RememberedTokenSymbol == TokenSymbol::NotSym)
    {
        if (GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].CommandName == VirtualCommandName::LDC)
            GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Value = !GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Value;
        else
            GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::NOT);
    }
}

void ParserToVirtualCodeGenerator::ExpressionOneArgument()
{
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::IncSym || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::DecSym)
        ExpressionOneArgumentIncOrDec();
    else
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::BitAndSym) //& adres zmiennej ,co gdy klasa,funkcja,tablica
        ExpressionOneArgumentBitAnd();
    else
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::StrPtrSym)
        ExpressionOneArgumentStrPtr();
    //3) wprowadzic funkcje co rownoczesnie przeszukuje drzewo tablic i zmiennych
    //   i zwraca wlasciwe wlacznie z przeciazonymi i GLOBAL data i zastapic
    //   ta funkcja wywolanie pozycja_in_tree( symb , var_root lub arr_root );
    else
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::MulSym)
        ExpressionOneArgumentMul();
    else
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::LeftParSym && CheckType(GeneratedLexicalAnalysisTokenPosition))
        ExpressionOneArgumentLeftPar1();
    else
    if (FindType(static_cast<UnsignedInt>(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol)) && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition + 1].Symbol == TokenSymbol::LeftParSym)
        ExpressionOneArgumentLeftPar2();
    else
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::NewSym)
        InstructionNew();
    else
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::SizeOfSym && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition + 1].Symbol == TokenSymbol::LeftParSym)
        ExpressionOneArgumentSizeOf();
    else
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::BitAndSym || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::PlusSym || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::MinusSym || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::TildeSym || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::NotSym)
        ExpressionOneArgumentBitAndOrPlusOrMinusOrNotOrTilde();
    else
        ExpressionSuffix();
}

void ParserToVirtualCodeGenerator::ExpressionSuffix()
{
    ExpressionSimple();
    shared_ptr<ArrayDefinition> ArrayPointer = nullptr;
    UnsignedInt Level;
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition - 1].Symbol == TokenSymbol::IdentifierSym)
        ArrayPointer = FindArray((UnsignedInt)GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition - 1].Type, Level);

    //if( !vtab && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition] == lsquare )
    //{
    //  przeszukiwac takze drzewo zmiennych by znalezc (UnsignedInt)GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition-1].Type
    //  i sprawdzic czy to wskaznik - if tak to liczba rozmiarow tablicy
    //  musi sie zgadzac z liczba gwiazdek
    //}
    UnsignedInt SizePosition = 0;
    if (ArrayPointer)
    {
        while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::LeftSquareSym)
        {
            GetNextTokenSymbol();
            const UnsignedInt GeneratedVirtualCodeCommandIndex1 = NumberOfGeneratedVirtualCodeCommands;
            Expression();
            const UnsignedInt GeneratedVirtualCodeCommandIndex2 = NumberOfGeneratedVirtualCodeCommands;
            UnsignedInt MultiplyDataSize = ArrayPointer->Type->Length;
            for (UnsignedInt SizeDimensionIndex = SizePosition + 1; SizeDimensionIndex < ArrayPointer->NumberOfDimemsions; SizeDimensionIndex++)
                MultiplyDataSize *= (ArrayPointer->SizeForEveryDimension[SizeDimensionIndex]); //+1
            if (GeneratedVirtualCodeCommandIndex2 - GeneratedVirtualCodeCommandIndex1 == 1 && GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].CommandName == VirtualCommandName::LDC)
            {
                NumberOfGeneratedVirtualCodeCommands = GeneratedVirtualCodeCommandIndex1;
                //GenerateVirtualCodeCommand(VirtualCommandName::LDC, static_cast<SignedInt>(VirtualCommandDataType::SIGNED_INT_TYPE), MultiplyDataSize * GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands].Value, 0);
                //GenerateVirtualCodeCommand(VirtualCommandName::LDC, static_cast<SignedInt>(GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Type), MultiplyDataSize * GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands].Value, 0);
                GenerateVirtualCodeCommand(VirtualCommandName::LDC, ArrayPointer->Type->TypeNumericCode, MultiplyDataSize * GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands].Value, 0);

                GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::ADD);
                if (GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 3].CommandName == VirtualCommandName::OPR && GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 3].Operation == VirtualCommandOperationType::ADD && GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 4].CommandName == VirtualCommandName::LDC)
                {
                    const RealType Operand1 = GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 4].Value;
                    const RealType Operand2 = GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 2].Value;
                    NumberOfGeneratedVirtualCodeCommands -= 4;
                    //GenerateVirtualCodeCommand(VirtualCommandName::LDC, static_cast<SignedInt>(VirtualCommandDataType::SIGNED_INT_TYPE), Operand1 + Operand2, 0);
                    GenerateVirtualCodeCommand(VirtualCommandName::LDC, ArrayPointer->Type->TypeNumericCode, Operand1 + Operand2, 0);
                    GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::ADD);
                }
            }
            else
            {
                //GenerateVirtualCodeCommand(VirtualCommandName::LDC, static_cast<SignedInt>(VirtualCommandDataType::SIGNED_INT_TYPE), MultiplyDataSize, 0);
                GenerateVirtualCodeCommand(VirtualCommandName::LDC, ArrayPointer->Type->TypeNumericCode, MultiplyDataSize, 0);
                NewLastTokenSymbolType = TokenSymbol::SignedIntSym;
                GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::MUL);
                GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::ADD);
            }
            SizePosition++;
            if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::RightSquareSym)
                GetNextTokenSymbol();
            else
                PrintError("unmatched [ ");
        }
        if (GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 3].CommandName == VirtualCommandName::LDPTROFV && GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].CommandName == VirtualCommandName::OPR && GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Operation == VirtualCommandOperationType::ADD && GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 2].CommandName == VirtualCommandName::LDC)
        {
            GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 3].TargetAddress += static_cast<UnsignedInt>(GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 2].Value);

            NumberOfGeneratedVirtualCodeCommands -= 2;
        }
        if (SizePosition != ArrayPointer->NumberOfDimemsions)
            PrintError("size of array doesn't match");
        else
        {
            //GenerateVirtualCodeCommand(VirtualCommandName::LDVFPTR, static_cast<SignedInt>(VirtualCommandDataType::SIGNED_INT_TYPE), 0, 0);
            //GenerateVirtualCodeCommand(VirtualCommandName::LDVFPTR, static_cast<SignedInt>(VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE), 0, 0);
            //GenerateVirtualCodeCommand(VirtualCommandName::LDVFPTR, static_cast<SignedInt>(NewLastTokenSymbolType = static_cast<TokenSymbol>(ArrayPointer->Type->TypeNumericCode)), 0, 0);
            GenerateVirtualCodeCommand(VirtualCommandName::LDVFPTR, ArrayPointer->Type->TypeNumericCode, 0, 0);
            GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Level = Level;
        }
    }
}

TokenSymbol ParserToVirtualCodeGenerator::GetTokenSymbolForType(VirtualCommandDataType TypeNumericCode)
{
    switch (TypeNumericCode)
    {
        case VirtualCommandDataType::SIGNED_CHAR_TYPE : return TokenSymbol::SignedCharSym;
        case VirtualCommandDataType::UNSIGNED_CHAR_TYPE : return TokenSymbol::UnsignedCharSym;

        case VirtualCommandDataType::SIGNED_SHORT_TYPE : return TokenSymbol::SignedShortIntSym;
        case VirtualCommandDataType::UNSIGNED_SHORT_TYPE : return TokenSymbol::UnsignedShortIntSym;

        case VirtualCommandDataType::SIGNED_INT_TYPE : return TokenSymbol::SignedIntSym;
        case VirtualCommandDataType::UNSIGNED_INT_TYPE : return TokenSymbol::UnsignedIntSym;

        case VirtualCommandDataType::SIGNED_LONG_INT_TYPE : return TokenSymbol::SignedLongIntSym;
        case VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE : return TokenSymbol::UnsignedLongIntSym;

        case VirtualCommandDataType::FLOAT_TYPE : return TokenSymbol::FloatSym;
        case VirtualCommandDataType::DOUBLE_TYPE : return TokenSymbol::DoubleSym;
        case VirtualCommandDataType::EXTENDED_TYPE : return TokenSymbol::ExtendedSym;

        case VirtualCommandDataType::BCD_TYPE : return TokenSymbol::BCDSym;

        default : return static_cast<TokenSymbol>(TypeNumericCode);
    }
}

void ParserToVirtualCodeGenerator::ExpressionSimpleIdentifierLeftSquare()
{
    UnsignedInt Level;
    const auto ArrayObject = FindArray((UnsignedInt)GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type, Level);
    if (ArrayObject == nullptr)
        PrintError("identifier not declared");
    else
    {
        GetNextTokenSymbol();
        //GenerateVirtualCodeCommand(VirtualCommandName::LDPTROFV, ArrayObject->NumberOfDimemsions, 0, ArrayObject->ArrayAddress);
        //GenerateVirtualCodeCommand(VirtualCommandName::LDPTROFV, static_cast<SignedInt>(VirtualCommandDataType::SIGNED_INT_TYPE), 0, ArrayObject->ArrayAddress);
        GenerateVirtualCodeCommand(VirtualCommandName::LDPTROFV, ArrayObject->Type->TypeNumericCode, 0, ArrayObject->ArrayAddress);
        GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Level = Level;
        GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Index = ArrayObject->IndexToDescribeInGlobalDataArray;
        //if( level == GLOBAL_TYPE ) kod[ lr-1 ].Kind = //indeks w opis_global
    }
}

void ParserToVirtualCodeGenerator::ExpressionSimpleIdentifierLeftPar(UnsignedInt& FunctionCallParameterIndex)
{
    UnsignedInt RememberGeneratedVirtualCodeCommandIndex = NumberOfGeneratedVirtualCodeCommands;
    InsideFunctionsNextedLevel++;
    ExpressionAssignment();
    FunctionCallParameters.emplace_back();
    FunctionCallParameters[FunctionCallParameterIndex] = static_cast<UnsignedInt>(GetTokenSymbolForType(static_cast<VirtualCommandDataType>(NewLastTokenSymbolType)));
    GenerateVirtualCodeCommand(VirtualCommandName::LOAD, 0, 0, 0);
    GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Inside = InsideFunctionsNextedLevel;
    GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Index = FunctionCallParameterIndex;
                                                GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Level = LOCAL_LEVEL;
    while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::CommaSym)
    {
        FunctionCallParameterIndex++;
        GetNextTokenSymbol();
        UnsignedInt GeneratedVirtualCodeCommandIndex1 = NumberOfGeneratedVirtualCodeCommands;
        ExpressionAssignment();
        FunctionCallParameters.emplace_back();
        FunctionCallParameters[FunctionCallParameterIndex] = static_cast<UnsignedInt>(GetTokenSymbolForType(static_cast<VirtualCommandDataType>(NewLastTokenSymbolType)));
        if (NumberOfGeneratedVirtualCodeCommands - GeneratedVirtualCodeCommandIndex1 == 1 && GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex1].CommandName == VirtualCommandName::LDC)
        {
            const UnsignedInt Value = GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex1].Value;
            auto Type = GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex1].Type;
            NumberOfGeneratedVirtualCodeCommands = GeneratedVirtualCodeCommandIndex1;
            GenerateVirtualCodeCommand(VirtualCommandName::LOAD, static_cast<SignedInt>(Type), Value, 0);
                                                GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Level = LOCAL_LEVEL;
            GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Index = FunctionCallParameterIndex;
            GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Kind = CONSTANT_TYPE;
            GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Inside = InsideFunctionsNextedLevel;
        }
        else
        if (NumberOfGeneratedVirtualCodeCommands - GeneratedVirtualCodeCommandIndex1 == 1 && GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex1].CommandName == VirtualCommandName::LDV)
        {
            const UnsignedInt TargetAddress = GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex1].TargetAddress;
            auto Type = GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex1].Type;
            const UnsignedInt What = GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex1].Kind;
            NumberOfGeneratedVirtualCodeCommands = GeneratedVirtualCodeCommandIndex1;
            GenerateVirtualCodeCommand(VirtualCommandName::LOAD, static_cast<SignedInt>(Type), 0, TargetAddress);
            GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Index = FunctionCallParameterIndex;
                                                GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Level = LOCAL_LEVEL;
            GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Kind = static_cast<SignedInt>(Type); // typ zmiennej
            //GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Level = What; //parametr czy lokalna
            GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Inside = InsideFunctionsNextedLevel;
        }
        else
        {
            auto Type = GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex1].Type;
            GenerateVirtualCodeCommand(VirtualCommandName::LOAD, static_cast<SignedInt>(Type), 0, 0);
            GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Index = FunctionCallParameterIndex;
                                                        GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Level = LOCAL_LEVEL;
            GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Kind = 0;
            GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Inside = InsideFunctionsNextedLevel;
        }
    }
    if (FunctionCallParameterIndex >= NumberOfProperFastRegisters)
    {
        for (UnsignedInt GeneratedVirtualCodeCommandIndex = RememberGeneratedVirtualCodeCommandIndex; GeneratedVirtualCodeCommandIndex < NumberOfGeneratedVirtualCodeCommands; GeneratedVirtualCodeCommandIndex++)
            if (GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].CommandName == VirtualCommandName::LOAD && GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].Inside == InsideFunctionsNextedLevel)
                GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].CommandName = VirtualCommandName::LOADPUSH;
    }
    InsideFunctionsNextedLevel--;
    FunctionCallParameterIndex++;
}

void ParserToVirtualCodeGenerator::ExpressionSimpleFunctionCall(shared_ptr<FunctionDefinition>& FunctionPointer, const UnsignedInt NumberOfFunctionParameters, UnsignedInt& CallAddress, UnsignedInt& MaxOk, UnsignedInt& PosMaxOk)
{
    UnsignedInt Level;

    printff("call ");

    if (Level == CLASS_LEVEL)
        printf("%s::", ClassName.c_str());

    printff("%s = ", FunctionPointer->FunctionNameString.c_str());

    for (UnsignedInt k = 0; k < NumberOfFunctionParameters; k++)
        printff("%lu_", FunctionCallParameters[k]);

    printff(" \n");

    std::vector<std::shared_ptr<FunctionDefinition>> ListOfFunctionsPointers;

    const UnsignedInt NumberOfFunctionsOverloaded = FunctionPointer->FunctionsOverloaded.size();
    ListOfFunctionsPointers.emplace_back(FunctionPointer);
    for (UnsignedInt FunctionOverloadedIndex = 1; FunctionOverloadedIndex <= NumberOfFunctionsOverloaded; FunctionOverloadedIndex++)
        ListOfFunctionsPointers.emplace_back(FunctionPointer->FunctionsOverloaded[FunctionOverloadedIndex - 1]);

    MaxOk = 0;
    PosMaxOk = 0;
    bool FoundTheSameNumberOfParameters = false;
    for (UnsignedInt FunctionsPointersIndex = 0; FunctionsPointersIndex < ListOfFunctionsPointers.size(); FunctionsPointersIndex++)
        if (ListOfFunctionsPointers[FunctionsPointersIndex]->ParametersTypes.size() == NumberOfFunctionParameters)
        {
            FoundTheSameNumberOfParameters = true;
            UnsignedInt NumberOfTheSameFunctionParametersTypes = 0;
            for (UnsignedInt FunctionParameterIndex = 1; FunctionParameterIndex < NumberOfFunctionParameters; FunctionParameterIndex++)
                if (ListOfFunctionsPointers[FunctionsPointersIndex]->ParametersTypes[FunctionParameterIndex] == FunctionCallParameters[FunctionParameterIndex])
                    NumberOfTheSameFunctionParametersTypes++;
            if (NumberOfTheSameFunctionParametersTypes == NumberOfFunctionParameters)
            {
                PosMaxOk = FunctionsPointersIndex;
                break;
            }
            if (NumberOfTheSameFunctionParametersTypes > MaxOk)
            {
                MaxOk = NumberOfTheSameFunctionParametersTypes;
                PosMaxOk = FunctionsPointersIndex;
            }
        }
    if (!FoundTheSameNumberOfParameters)
        PrintError("number of function arguments is incorrect in calling");

    FunctionPointer = ListOfFunctionsPointers[PosMaxOk];
    CallAddress = FunctionPointer->VirtualAddress;
}

void ParserToVirtualCodeGenerator::ExpressionSimpleIdentifierRightPar(const shared_ptr<FunctionDefinition>& FunctionPointer, const UnsignedInt& CallAddress, const UnsignedInt NumericFunctionCode, const UnsignedInt PositionMaximalOK)
{
    GetNextTokenSymbol();
    if (FunctionPointer->Declare == 1)
    {
        GenerateVirtualCodeCommand(VirtualCommandName::CALL, NumericFunctionCode, 0, CallAddress);
        GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Kind = FunctionPointer->IsInline;
        GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Level = PositionMaximalOK;
    }
    if (FunctionPointer->Declare == 0)
    {
        GenerateVirtualCodeCommand(VirtualCommandName::CALL, NumericFunctionCode, 0, UINT64_MAX);//UINT64_MAX zamiast -9999
        GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Kind = FunctionPointer->IsInline;
        GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Level = PositionMaximalOK;

        FunctionsAddresses.emplace_back((UnsignedInt)FunctionPointer.get(), NumberOfGeneratedVirtualCodeCommands - 1);
    }
}

void ParserToVirtualCodeGenerator::ExpressionSimpleIdentifierLeftParFunctionCallRightPar()
{
    UnsignedInt RememberGeneratedLexicalAnalysisTokenPosition = GeneratedLexicalAnalysisTokenPosition;
    FunctionCallParameters.clear();
    FunctionCallParameters.emplace_back(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type);
    UnsignedInt NumericFunctionCode = FunctionCallParameters[0];
    GetNextTokenSymbol();
    GetNextTokenSymbol();
    UnsignedInt Counter = 1;
    UnsignedInt MaxOk, posMaxOk;

    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol != TokenSymbol::RightParSym)
        ExpressionSimpleIdentifierLeftPar(Counter);

    UnsignedInt Level;
    UnsignedInt CallAddress;
    auto FunctionPointer = FindFunction(NumericFunctionCode, Level);
    if (FunctionPointer == nullptr)
        PrintErrorToFile(RememberGeneratedLexicalAnalysisTokenPosition, "that function is not declared or predeclared");
    else
        ExpressionSimpleFunctionCall(FunctionPointer, Counter, CallAddress, MaxOk, posMaxOk);

    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::RightParSym)
        ExpressionSimpleIdentifierRightPar(FunctionPointer, CallAddress, NumericFunctionCode, posMaxOk);
    else
        PrintError("unbalanced (");
}

void ParserToVirtualCodeGenerator::ExpressionSimpleIdentifier()
{
    UnsignedInt Level;
    const auto VariablePointer = FindVariable((UnsignedInt)GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type, Level);

    if (VariablePointer != nullptr)
    {
        /* VARIABLES OF CLASS CALLING */
        if (VariablePointer->Type->TypeNumericCode == CLASS_LEVEL) //zmienna typu klasowego //7.04.2001
        {
            if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition + 1].Symbol == TokenSymbol::PointSym)
            {
                if (VariablePointer->Type->Pointer != 0)
                    PrintError("pointer type not allowed");
                GetNextTokenSymbol();
                //czy nowa zmienna istnieje w danej klasie	.
                //pobierz z drzewa typu klasowego czy istnieje dla tej konkretnej klasy
                //nie uzywac to find_var bez przerobki
                //trzeba uzywac caly czas: (var_definition*)tree.pozycja_in_tree( typesymvar,prev->var_root );
                //musi wyszukac z danej podklasy wsrod ponizszych
                //drzew:
                //ptr->type->pclass->types_root
                //AZ inny niz typ klasowo-strukturalny np:

                //ptr->type->pclass->var_root
                //ptr->type->pclass->arr_root
                //ptr->type->pclass->fun_root
                //gdy jeden z trzech powyzszych to przerywa wyszukiwanie , cofa pobysm()
                //a nastepnie
                //wyrazenie_proste();
                //lecz prcls jest nastawiony na ta zmienna
                //genracja
            }
            else
            if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition + 1].Symbol == TokenSymbol::MinusSym && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition + 2].Symbol == TokenSymbol::GreaterThan)
            {
                if (VariablePointer->Type->Pointer == 0)
                    PrintError("must be pointer type");
                GetNextTokenSymbol();
                GetNextTokenSymbol();
                //czy nowa zmienna istnieje w danej klasie	- z wyrazenie_proste().


                //wyrazenie_proste();
            }
        }
        /* VARIABLES OF CLASS CALLING END */
        else
        if (VariablePointer->Type->TypeNumericCode != CLASS_LEVEL)
        {
            GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands].Level = Level;
            GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands].Index = VariablePointer->IndexToDescribeInGlobalDataArray;
            GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands].Kind = VariablePointer->Kind;
            //GenerateVirtualCodeCommand(VirtualCommandName::LDV, static_cast<UnsignedInt>(NewLastTokenSymbolType = static_cast<TokenSymbol>(VariablePointer->Type->TypeNumericCode)), 0, VariablePointer->TypeAddress - VariablePointer->Length);
            GenerateVirtualCodeCommand(VirtualCommandName::LDV, VariablePointer->Type->TypeNumericCode, 0, VariablePointer->TypeAddress - VariablePointer->Length);
            GetNextTokenSymbol();
        }
    }
    else
        PrintError("identifier not declared");
}

void ParserToVirtualCodeGenerator::ExpressionSimpleNumber()
{
    GenerateVirtualCodeCommand(VirtualCommandName::LDC, static_cast<SignedInt>(VirtualCommandDataType::SIGNED_INT_TYPE), GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type, 0);
    NewLastTokenSymbolType = TokenSymbol::SignedIntSym;
    GetNextTokenSymbol();
}

void ParserToVirtualCodeGenerator::ExpressionSimpleLeftRightPar()
{
    GetNextTokenSymbol();
    Expression();
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::RightParSym)
        GetNextTokenSymbol();
    else
        PrintError("blad nawiasowy");
}

void ParserToVirtualCodeGenerator::ExpressionSimple()
{
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::IdentifierSym && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition + 1].Symbol == TokenSymbol::LeftSquareSym)
        ExpressionSimpleIdentifierLeftSquare();
    else
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::IdentifierSym && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition + 1].Symbol == TokenSymbol::LeftParSym)
        ExpressionSimpleIdentifierLeftParFunctionCallRightPar();
    else
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::IdentifierSym)
        ExpressionSimpleIdentifier();
    else
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::NumberSym)
        ExpressionSimpleNumber();
    else
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::LeftParSym)
        ExpressionSimpleLeftRightPar();
    else
        PrintError("small error");
}

void ParserToVirtualCodeGenerator::GenerateFillUnknownCallAddresses()
{
    for (const auto& FunctionAddress : FunctionsAddresses)
    {
        //const auto FunctionPointer = (FunctionDefinition*)FunctionAddress.NumericCode;
        const auto FunctionPointer = (FunctionDefinition*)FunctionAddress.NumericCode;
        if (FunctionPointer->Declare == 1)
        {
            //printff( "%d %d\n",adresy_fun[ k ].adr , fptr->virtual_adress );

            GeneratedVirtualCode[FunctionAddress.Address].TargetAddress = FunctionPointer->VirtualAddress;
        }
        if (FunctionPointer->Declare == 0)
        {
            UnsignedInt kod_fun = FunctionPointer->ParametersTypes[0];
            if (auto SymbolInt = NumericCodeToStringSymbolsMap.find(kod_fun); SymbolInt != NumericCodeToStringSymbolsMap.end())
            {
                printff("function %s nas no body in unit\n", SymbolInt->second.c_str());
                // ErrorString "function not declared"
                // //1)ponizsze 'if' musi byc bo wielokrotne rowna sie generuje to 2x
                if (ExternData.empty() == true || (ExternData.empty() == false && SymbolInt->second != ExternData.back().StringCode))
                    ExternData.emplace_back(SymbolInt->second, FunctionPointer->OverloadNumber);
            }
        }
    }
}

void ParserToVirtualCodeGenerator::GenerateFillUnknownJumpAddresses(const shared_ptr<FunctionDefinition>& LabelFunctionPointer, const string_view ErrorString)
{
    for (const auto& LabelsAddress : LabelsAddresses)
    {
        if (auto LabelPointer = LabelFunctionPointer->LabelsContainer.find(LabelsAddress.NumericCode); LabelPointer != LabelFunctionPointer->LabelsContainer.end())
            GeneratedVirtualCode[LabelsAddress.Address].TargetAddress = LabelPointer->second->LabelAddress;
        else
            PrintError(ErrorString);
    }
}

void ParserToVirtualCodeGenerator::InstructionExpression()
{
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::SemicolonSym)
        GetNextTokenSymbol();
    else
    {
        Expression();

        //INC,DEC tutaj co gdy r=(x+1)++; gdy r++; to dobrze
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::IncSym || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::DecSym)
        {
            const TokenSymbol OperationCode = GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol;
            GetNextTokenSymbol();
            if (OperationCode == TokenSymbol::IncSym)
                GenerateVirtualCodeCommand(VirtualCommandName::OPR, GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].TargetAddress, 0, 0, VirtualCommandOperationType::INC);
            if (OperationCode == TokenSymbol::DecSym)
                GenerateVirtualCodeCommand(VirtualCommandName::OPR, GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].TargetAddress, 0, 0, VirtualCommandOperationType::DEC);
            GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Kind = -100;
            GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Level = GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1 - 1].Level;
            GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Index = GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1 - 1].Index;
        }

        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::SemicolonSym)
            GetNextTokenSymbol();
        else
            PrintError("missing ;");
    }
    GenerateVirtualCodeCommand(VirtualCommandName::STACK0, 0, 0, 0);
}

void ParserToVirtualCodeGenerator::InstructionChoiceIf()
{
    GetNextTokenSymbol();
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::LeftParSym)
    {
        GetNextTokenSymbol();
        ExpressionAssignment();
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::RightParSym)
        {
            GetNextTokenSymbol();
            const UnsignedInt GeneratedVirtualCodeCommandIndex1 = NumberOfGeneratedVirtualCodeCommands;
            GenerateVirtualCodeCommand(VirtualCommandName::JCON, 0, 0, 0); //to juz jest dobrze
            Instruction();
            GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex1].TargetAddress = NumberOfGeneratedVirtualCodeCommands;
            if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::ElseSym)
            {
                GetNextTokenSymbol();
                GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex1].TargetAddress++;
                const UnsignedInt GeneratedVirtualCodeCommandIndex2 = NumberOfGeneratedVirtualCodeCommands;
                GenerateVirtualCodeCommand(VirtualCommandName::JMP, 0, 0, 0);
                Instruction();
                GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex2].TargetAddress = NumberOfGeneratedVirtualCodeCommands;
            }
        }
        else
            PrintError("missing right paren");
    }
    else
        PrintError("lpar expected");
}

void ParserToVirtualCodeGenerator::InstructionChoiceSwitch()
{
    InsideLoopsNestedLevel++;
    const UnsignedInt LocalGeneratedVirtualCodeCommandIndex = NumberOfGeneratedVirtualCodeCommands;
    GetNextTokenSymbol();
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::LeftParSym)
    {
        GetNextTokenSymbol();
        Expression();
        GenerateVirtualCodeCommand(VirtualCommandName::PUSHEDX, 0, 0, 0);

        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::RightParSym)
            GetNextTokenSymbol();
        else
            PrintError("missing right paren");
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::LeftCurlySym)
            GetNextTokenSymbol();
        else
            PrintError("missing left lcurly");
        while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::CaseSym)
        {
            GetNextTokenSymbol();
            Expression();
            GenerateVirtualCodeCommand(VirtualCommandName::POPEDXPUSH, 0, 0, 0);

            GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::EQU);
            const UnsignedInt GeneratedVirtualCodeCommandIndex1 = NumberOfGeneratedVirtualCodeCommands;
            GenerateVirtualCodeCommand(VirtualCommandName::JCON, 0, 0, 0);
            if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::ColonSym)
                GetNextTokenSymbol();
            else
                PrintError("colon expected");
            while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol != TokenSymbol::RightCurlySym && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol != TokenSymbol::CaseSym && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol != TokenSymbol::DefaultSym)
                Instruction();
            GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex1].TargetAddress = NumberOfGeneratedVirtualCodeCommands;
        }
        GenerateVirtualCodeCommand(VirtualCommandName::POPEDX0, 0, 0, 0); //pierwsze pop_edx

        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::DefaultSym)
        {
            GetNextTokenSymbol();
            if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::ColonSym)
                GetNextTokenSymbol();
            else
                PrintError("colon expected");
            Expression();
            while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol != TokenSymbol::RightCurlySym)
                Instruction();
            GenerateVirtualCodeCommand(VirtualCommandName::JMP, 0, 0, NumberOfGeneratedVirtualCodeCommands + 2); //przeskocz ostatnie pop_edx
        }
        for (UnsignedInt GeneratedVirtualCodeCommandIndex = LocalGeneratedVirtualCodeCommandIndex; GeneratedVirtualCodeCommandIndex < NumberOfGeneratedVirtualCodeCommands; GeneratedVirtualCodeCommandIndex++)
            if (GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].CommandName == VirtualCommandName::JBREAK && static_cast<UnsignedInt>(GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].Type) == InsideLoopsNestedLevel)
                GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].TargetAddress = NumberOfGeneratedVirtualCodeCommands;
        GenerateVirtualCodeCommand(VirtualCommandName::POPEDX0, 0, 0, 0); //ostatnie pop_edx

        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::RightCurlySym)
            GetNextTokenSymbol();
        else
            PrintError("missing right lcurly");
    }
    else
        PrintError("lpar expected");

    InsideLoopsNestedLevel--;
}

void ParserToVirtualCodeGenerator::ChoiceInstruction()
{
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::IfSym)
        InstructionChoiceIf();
    else
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::SwitchSym)
        InstructionChoiceSwitch();
    else
        PrintError("if or switch expected");
}

void ParserToVirtualCodeGenerator::FillBreakContinueLabelsAddresses(const UnsignedInt GeneratedVirtualCodeCommandIndex, const UnsignedInt GeneratedVirtualCodeCommandIndexAdditional)
{
    for (UnsignedInt VirtualCodeCommandIndex = GeneratedVirtualCodeCommandIndex; VirtualCodeCommandIndex < NumberOfGeneratedVirtualCodeCommands; VirtualCodeCommandIndex++)
    {
        if (GeneratedVirtualCode[VirtualCodeCommandIndex].CommandName == VirtualCommandName::JCONT && static_cast<UnsignedInt>(GeneratedVirtualCode[VirtualCodeCommandIndex].Type) == InsideLoopsNestedLevel)
            GeneratedVirtualCode[VirtualCodeCommandIndex].TargetAddress = GeneratedVirtualCodeCommandIndexAdditional;
        else
        if (GeneratedVirtualCode[VirtualCodeCommandIndex].CommandName == VirtualCommandName::JBREAK && static_cast<UnsignedInt>(GeneratedVirtualCode[VirtualCodeCommandIndex].Type) == InsideLoopsNestedLevel)
            GeneratedVirtualCode[VirtualCodeCommandIndex].TargetAddress = NumberOfGeneratedVirtualCodeCommands;
    }
}

void ParserToVirtualCodeGenerator::CheckBalanceOfParenthesis()
{
    UnsignedInt Unbalanced = 0;
    while (Unbalanced != 1)
    {
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol != TokenSymbol::RightParSym && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol != TokenSymbol::LeftParSym)
            GeneratedLexicalAnalysisTokenPosition++;
        else
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::LeftParSym)
        {
            Unbalanced--;
            GeneratedLexicalAnalysisTokenPosition++;
        }
        else
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::RightParSym)
        {
            Unbalanced++;
            GeneratedLexicalAnalysisTokenPosition++;
        }
    }
}

void ParserToVirtualCodeGenerator::InstructionLoopFor()
{
    InsideLoopsNestedLevel++;
    UnsignedInt LexicalAnalysisTokenSymbol1;
    GetNextTokenSymbol(); //zjedz forsym
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::LeftParSym)
        GetNextTokenSymbol();
    else
        PrintError("left paren expected");
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::SemicolonSym)
        GetNextTokenSymbol();
    else
    {
        if (FindType(static_cast<UnsignedInt>(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol)))
            Declaration(false);
        else
            Expression();
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::SemicolonSym)
        {
            GetNextTokenSymbol();
            LexicalAnalysisTokenSymbol1 = GeneratedLexicalAnalysisTokenPosition;
        }
        else
            PrintError(" ; expected in for");
    }
    const UnsignedInt GeneratedVirtualCodeCommandIndex1 = NumberOfGeneratedVirtualCodeCommands;
    GenerateVirtualCodeCommand(VirtualCommandName::JMP, 0, 0, 0);
    while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol != TokenSymbol::SemicolonSym)
        GeneratedLexicalAnalysisTokenPosition++;
    GetNextTokenSymbol(); //do zjedzenia 2 srednika
    const UnsignedInt LexicalAnalysisTokenSymbol2 = GeneratedLexicalAnalysisTokenPosition; //do zapamietania 1. pozycji po drugim sredniku
    CheckBalanceOfParenthesis();
    //UnsignedInt lex3 = GeneratedLexicalAnalysisTokenPosition; //do zapamietania 1. pozycji po prawym nawiasie zamykajacym
    // ale to chyba jest zbedne
    Instruction();
    const UnsignedInt LexicalAnalysisTokenSymbol4 = GeneratedLexicalAnalysisTokenPosition;
    GeneratedLexicalAnalysisTokenPosition = LexicalAnalysisTokenSymbol2;
    const UnsignedInt VirtualCodeCommandIndex = NumberOfGeneratedVirtualCodeCommands;
    Expression();
    GeneratedLexicalAnalysisTokenPosition = LexicalAnalysisTokenSymbol1;
    GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex1].TargetAddress = NumberOfGeneratedVirtualCodeCommands;
    ExpressionConditional();
    GenerateVirtualCodeCommand(VirtualCommandName::JCON, 1, 0, GeneratedVirtualCodeCommandIndex1 + 1);
    GeneratedLexicalAnalysisTokenPosition = LexicalAnalysisTokenSymbol4;
    FillBreakContinueLabelsAddresses(GeneratedVirtualCodeCommandIndex1, VirtualCodeCommandIndex);
    InsideLoopsNestedLevel--;
}

void ParserToVirtualCodeGenerator::InstructionLoopDo()
{
    InsideLoopsNestedLevel++;
    GetNextTokenSymbol();
    UnsignedInt GeneratedVirtualCodeCommandIndex1 = NumberOfGeneratedVirtualCodeCommands;
    Instruction();
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::WhileSym)
        GetNextTokenSymbol();
    else
        PrintError("while expected");
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::LeftParSym)
        GetNextTokenSymbol();
    else
        PrintError("lpar expected");
    UnsignedInt VirtualCodeCommandIndex = NumberOfGeneratedVirtualCodeCommands;
    ExpressionConditional();
    GenerateVirtualCodeCommand(VirtualCommandName::JCON, 0, 0, GeneratedVirtualCodeCommandIndex1);
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::RightParSym)
        GetNextTokenSymbol();
    else
        PrintError("rpar expected");
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::SemicolonSym)
        GetNextTokenSymbol();
    else
        PrintError("semicolon expected");
    FillBreakContinueLabelsAddresses(GeneratedVirtualCodeCommandIndex1, VirtualCodeCommandIndex);
    InsideLoopsNestedLevel--;
}

void ParserToVirtualCodeGenerator::InstructionLoopWhile()
{
    UnsignedInt GeneratedVirtualCodeCommandIndex1 = NumberOfGeneratedVirtualCodeCommands;
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::WhileSym)
    {
        InsideLoopsNestedLevel++;
        GetNextTokenSymbol();
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::LeftParSym)
        {
            GetNextTokenSymbol();
            GeneratedVirtualCodeCommandIndex1 = NumberOfGeneratedVirtualCodeCommands;
            GenerateVirtualCodeCommand(VirtualCommandName::JMP, 0, 0, 0);
            const UnsignedInt LexicalAnalysisTokenStartPosition = GeneratedLexicalAnalysisTokenPosition;
            CheckBalanceOfParenthesis();
            Instruction();
            const UnsignedInt RememberedGeneratedLexicalAnalysisTokenPosition = GeneratedLexicalAnalysisTokenPosition;
            GeneratedLexicalAnalysisTokenPosition = LexicalAnalysisTokenStartPosition;
            GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex1].TargetAddress = NumberOfGeneratedVirtualCodeCommands;
            const UnsignedInt VirtualCodeCommandIndex = NumberOfGeneratedVirtualCodeCommands;
            ExpressionConditional();
            GeneratedLexicalAnalysisTokenPosition = RememberedGeneratedLexicalAnalysisTokenPosition;
            GenerateVirtualCodeCommand(VirtualCommandName::JCON, 1, 0, GeneratedVirtualCodeCommandIndex1 + 1);
            FillBreakContinueLabelsAddresses(GeneratedVirtualCodeCommandIndex1, VirtualCodeCommandIndex);
        }
        else
            PrintError("missing left parenthesis");
        InsideLoopsNestedLevel--;
    }
}

void ParserToVirtualCodeGenerator::InstructionLoop()
{
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::WhileSym)
        InstructionLoopWhile();
    else
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::ForSym)
        InstructionLoopFor();
    else
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::DoSym)
        InstructionLoopDo();
    else
        PrintError("while or for or do expected");
}

void ParserToVirtualCodeGenerator::InstructionContinue()
{
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::ContinueSym)
    {
        GetNextTokenSymbol();
        GenerateVirtualCodeCommand(VirtualCommandName::JCONT, InsideLoopsNestedLevel, 0, 0);
    }
}

void ParserToVirtualCodeGenerator::InstructionBreak()
{
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::BreakSym)
    {
        GetNextTokenSymbol();
        GenerateVirtualCodeCommand(VirtualCommandName::JBREAK, InsideLoopsNestedLevel, 0, 0);
    }
}

void ParserToVirtualCodeGenerator::InstructionLabel()
{
    if (!ProgramFunctionPointer->LabelsContainer.contains((UnsignedInt)GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type))
    {
        ProgramFunctionPointer->LabelsContainer[GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type] = make_shared<LabelDefinition>(NumberOfGeneratedVirtualCodeCommands);
        GetNextTokenSymbol();
        GetNextTokenSymbol();
    }
}

void ParserToVirtualCodeGenerator::InstructionGoto()
{
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::GotoSym)
    {
        GetNextTokenSymbol();
        shared_ptr<LabelDefinition> FoundLabelPointer;
        if (auto LabelPointer = ProgramFunctionPointer->LabelsContainer.find(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type); LabelPointer != ProgramFunctionPointer->LabelsContainer.end())
            FoundLabelPointer = LabelPointer->second;
        GetNextTokenSymbol();
        if (FoundLabelPointer)
            GenerateVirtualCodeCommand(VirtualCommandName::JGOTO, 0, 0, FoundLabelPointer->LabelAddress);
        else
        {
            LabelsAddresses.clear();
            GenerateVirtualCodeCommand(VirtualCommandName::JGOTO, 0, 0, 0);
            LabelsAddresses.emplace_back((UnsignedInt)GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition - 1].Type, NumberOfGeneratedVirtualCodeCommands - 1);
        }
        //if(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol==semicolon) pobsym(); else error("; expected/goto");
        //gen( stack_0 , 0 , 0 );
    }
}

void ParserToVirtualCodeGenerator::InstructionDelete()
{
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::DeleteSym)
    {
        GetNextTokenSymbol();
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::LeftSquareSym && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition + 1].Symbol == TokenSymbol::RightSquareSym)
        {
            GetNextTokenSymbol();
            GetNextTokenSymbol();
        }
        Expression();
        GenerateVirtualCodeCommand(VirtualCommandName::LOAD, 0, 0, 0);
        GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Index = 1;
        GenerateVirtualCodeCommand(VirtualCommandName::CALL, -2000, 0, 0);
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::SemicolonSym)
            GetNextTokenSymbol();
        else
            PrintError("; expected/delete");
        //gen( stack_0 , 0 , 0 );
    }
}

void ParserToVirtualCodeGenerator::InstructionNew()
{
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::NewSym)
    {
        GetNextTokenSymbol();
        auto PointerType = CheckType(GeneratedLexicalAnalysisTokenPosition - 1);
        if (!PointerType)
            PrintError("type after new operator expected");
        UnsignedInt Pointer = 0;
        GetNextTokenSymbol();
        while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::MulSym)
        {
            GetNextTokenSymbol();
            Pointer++;
        }
        if (Pointer)
        {
            GenerateVirtualCodeCommand(VirtualCommandName::LDC, static_cast<SignedInt>(VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE), 4, 0);
            NewLastTokenSymbolType = TokenSymbol::SignedIntSym;
        }
        if (!Pointer)
        {
            GenerateVirtualCodeCommand(VirtualCommandName::LDC, static_cast<SignedInt>(VirtualCommandDataType::SIGNED_INT_TYPE), PointerType->Length, 0);
            NewLastTokenSymbolType = TokenSymbol::SignedIntSym;
        }
        while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::LeftSquareSym)
        {
            GetNextTokenSymbol();
            Expression();
            GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::MUL);
            if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::RightSquareSym)
                GetNextTokenSymbol();
            else
                PrintError("] expected in new[]");
        }
        GenerateVirtualCodeCommand(VirtualCommandName::LOAD, 0, 0, 0);
        GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Index = 1;
        GenerateVirtualCodeCommand(VirtualCommandName::CALL, -3000, 0, 0);
    }
}

void ParserToVirtualCodeGenerator::JumpInstruction()
{
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::ReturnSym)
    {
        GetNextTokenSymbol();
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::SemicolonSym)
            GetNextTokenSymbol();
        else
        {
            Expression();
            //GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::FREE);
            GenerateVirtualCodeCommand(VirtualCommandName::FREE, 0, 0, 0);
            if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::SemicolonSym)
                GetNextTokenSymbol();
            else
                PrintError("; expected/return");
            //gen( stack_0 , 0 , 0 );
        }
    }
}

void ParserToVirtualCodeGenerator::DeclarationInstruction()
{
    Declaration(true);
}

void ParserToVirtualCodeGenerator::ComplexInstruction()
{
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::LeftCurlySym)
    {
        GetNextTokenSymbol();
        while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol != TokenSymbol::RightCurlySym)
            Instruction();
        GetNextTokenSymbol();
    }
    else
        PrintError("lcurly expected");
}

void ParserToVirtualCodeGenerator::Instruction()
{
    //printf( "%d\n",GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Indexine );
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::IfSym || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::SwitchSym)
        ChoiceInstruction();
    else
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::WhileSym || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::ForSym || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::DoSym)
        InstructionLoop();
    else
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::LeftCurlySym)
        ComplexInstruction();
    else
    if (FindType(static_cast<UnsignedInt>(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol)))
        DeclarationInstruction();
    else
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::ReturnSym)
        JumpInstruction();
    else
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::ContinueSym)
        InstructionContinue();
    else
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::BreakSym)
        InstructionBreak();
    else
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::GotoSym)
        InstructionGoto();
    else
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::IdentifierSym && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition + 1].Symbol == TokenSymbol::ColonSym)
        InstructionLabel();
    else
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::DeleteSym)
        InstructionDelete();
    else
        InstructionExpression();
}

void ParserToVirtualCodeGenerator::ParseFunctionDefinition()
{
    DeclarationSpecifiers();
    Declarator();
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::LeftCurlySym)
        ComplexInstruction();
    else
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::SemicolonSym)
        GetNextTokenSymbol();
}

void ParserToVirtualCodeGenerator::UnionDefinition()
{
};

void ParserToVirtualCodeGenerator::EnumDefinition()
{
    GetNextTokenSymbol();

    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::IdentifierSym)
        GetNextTokenSymbol();

    //ident do tablicy typow
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::LeftCurlySym)
    {
        GetNextTokenSymbol();
    }
    else
        PrintError("{ expected");

    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::RightCurlySym)
        GetNextTokenSymbol();
    else
        PrintError("} expected");
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::SemicolonSym)
        GetNextTokenSymbol();
    else
        PrintError("; expected");
};

void ParserToVirtualCodeGenerator::ParseInheritanceListInClassDefinition()
{
    GetNextTokenSymbol();
    while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol != TokenSymbol::LeftCurlySym && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol != TokenSymbol::SemicolonSym)
    {
        UnsignedInt IsVirtualInheritance = false;
        UnsignedInt SafenessLevel = PRIVATE_TYPE;
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::VirtualSym)
        {
            GetNextTokenSymbol();
            IsVirtualInheritance = true;
        }

        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::PublicSym)
        {
            GetNextTokenSymbol();
            SafenessLevel = PUBLIC_TYPE;
        }
        else
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::ProtectedSym)
        {
            GetNextTokenSymbol();
            SafenessLevel = PROTECTED_TYPE;
        }
        else
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::PrivateSym)
        {
            GetNextTokenSymbol();
            SafenessLevel = PRIVATE_TYPE;
        }

        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::IdentifierSym)
        {
            GetNextTokenSymbol();
            //sprawdz ident jaka to klasa czy struktura w drzewie typow
            //i zapamietaj w tablicy inheritance
            //Przekopiowac drzewa i wszystkie dane czy tylko
            //dodac i odpowiednio wyszukiwac przeszukjac drzewa przodkow
        }
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::CommaSym)
            GetNextTokenSymbol();
    }
}

void ParserToVirtualCodeGenerator::ParseClassElementsDefinition()
{
    while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol != TokenSymbol::RightCurlySym && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol != TokenSymbol::SemicolonSym)
    {
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::PublicSym && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition + 1].Symbol == TokenSymbol::ColonSym)
        {
            GetNextTokenSymbol();
            GetNextTokenSymbol();
            SafenessLevelOfClass = PUBLIC_TYPE;
        }
        else
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::PrivateSym && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition + 1].Symbol == TokenSymbol::ColonSym)
        {
            GetNextTokenSymbol();
            GetNextTokenSymbol();
            SafenessLevelOfClass = PRIVATE_TYPE;
        }
        else
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::ProtectedSym && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition + 1].Symbol == TokenSymbol::ColonSym)
        {
            GetNextTokenSymbol();
            GetNextTokenSymbol();
            SafenessLevelOfClass = PROTECTED_TYPE;
        }

        ClassUnit();

        //wowczas wszystkie co bylo globalne zyskuje przedwsakznik :
        //np: class_ptr->var_root
        //a SAFENESS dla globalnych zawsze = PUBLIC_TYPE

        //if class_inside to co z przydzielaniem

        //poprzedzanie funkcji przez inna w klasie :
        //co gdy w ciele klasy definicje (jak w javie) wszystkich funkcji
        //czyli co gdy ta co zdefiniowana pozniej niewidoczna dla wczesniejszych

        //:konstruktory i destruktory i operatory
        //gdy nowa funkcja to gdy CLASS_INSIDE > 1 to porownaj z nazwa
        //i dopisz do drzewa funkcji i np: funptr->kind = konstrutkor

        //slowka jak virtual przed funkcja - gdy globalne to ERROR
        //lub
        //NOWA jednostka tlumaczenia bez typedef itp , ale ze slowkiem virtual
        //NOWA jednostka tlumaczenia konczy sie gdy napotka:
        //public,private,protected lub rcurly=}
    }
}

void ParserToVirtualCodeGenerator::ParseClassDefinition()
{
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::ClassSym)
        SafenessLevelOfClass = PRIVATE_TYPE;
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::StructSym)
        SafenessLevelOfClass = PUBLIC_TYPE;
    GetNextTokenSymbol();
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::IdentifierSym)
    {
        printf("CLASS ", GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol);
        PointerToClassStack = 4;
        InsideClassesNestedLevel++;
        VariableType = CLASS_LEVEL;
        UnsignedInt ClassTypeCode = (UnsignedInt)GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type;

        if (auto SymbolInt = NumericCodeToStringSymbolsMap.find(ClassTypeCode); SymbolInt != NumericCodeToStringSymbolsMap.end())
            ClassName = SymbolInt->second;
        else
        {
        }

        GetNextTokenSymbol();
        shared_ptr<TypeDefinition> ClassTypePointer = FindType(ClassTypeCode);
        if (ClassTypePointer == nullptr)
        {
            //class_definition* act_ptr_class = (class_definition*)create_class();
            shared_ptr<ClassDefinition> NewClassPointer = make_shared<ClassDefinition>();
            InitializeType(static_cast<TokenSymbol>(ClassTypeCode), static_cast<VirtualCommandDataType>(CLASS_LEVEL), PointerToClassStack, 0, NewClassPointer, nullptr, nullptr);
            //dxc - min dlugosc = 4 na poczatku dla "this" , 0 - liczba gwiazdek
            ProgramClassPointer = NewClassPointer;
            ProgramClassPointer->ClassNameString = ClassName;
        }
        else
        {
            //prcls = (class_definition*)typklasa->pclass;
            ProgramClassPointer = ClassTypePointer->PointerToClass;
            printf("wypelniam predeklaracje klasy %s\n", ClassName.c_str());
        }

        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::ColonSym)
            ParseInheritanceListInClassDefinition();

        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::SemicolonSym)
        {
            printf("tylko predeklaracja klasy %s = %lu\n", ClassName.c_str(), ClassTypeCode);
            GetNextTokenSymbol();
            goto koniec;
        }
        else
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::LeftCurlySym)
            GetNextTokenSymbol();
        else
            PrintError("{ expected");

        while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol != TokenSymbol::RightCurlySym && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol != TokenSymbol::SemicolonSym)
            ParseClassElementsDefinition();
    }
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::RightCurlySym)
        GetNextTokenSymbol();
    else
        PrintError("} expected");
    //deklaracja zmienncych typu klasowego
    //utworz klase,strukture z adesem gdy adres = -1 to tylko klasa else
    //to oznacza obiekt
    //lub
    //jak wczesniej zminenna typu klasowego a ona ma adres
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::SemicolonSym)
        GetNextTokenSymbol();
    else
        PrintError("; expected");
koniec:
    //wypelnic dlugosc typu i klasy i juz mozna deklarowac
    InsideClassesNestedLevel--;
    ProgramClassPointer = ProgramPointer;
    VariableType = GLOBAL_LEVEL;
}

void ParserToVirtualCodeGenerator::ClassUnit()
{
    while ((FindType(static_cast<UnsignedInt>(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol)) ||
          ((GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::StaticSym || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::InlineSym)
          && FindType(static_cast<UnsignedInt>(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition + 1].Symbol)))
          || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::ClassSym
          || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::StructSym
          || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::UnionSym)
          && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol != TokenSymbol::PublicSym
          && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol != TokenSymbol::ProtectedSym
          && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol != TokenSymbol::PrivateSym
          && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol != TokenSymbol::RightCurlySym)
    {
        UnsignedInt LocalSymbolIdentifier = true;
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::ClassSym)
        {
            LocalSymbolIdentifier = false;
            ParseClassDefinition();
        }
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::StructSym)
        {
            LocalSymbolIdentifier = false;
            ParseClassDefinition();
        }
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::UnionSym)
        {
            LocalSymbolIdentifier = false;
            UnionDefinition();
        }
        if (!LocalSymbolIdentifier)
            continue;

        UnsignedInt LocalIsInline = true;
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::StaticSym)
        {
            LocalIsInline = false;
            GetNextTokenSymbol();
            printff("static ");
        }
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::InlineSym)
        {
            LocalIsInline = true;
            GetNextTokenSymbol();
            printff("inline ");
        }
        UnsignedInt RememberedLexicalAnalysisTokenPosition = GeneratedLexicalAnalysisTokenPosition;
        TypeSpecifier();
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::IdentifierSym && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition + 1].Symbol == TokenSymbol::LeftParSym)
        {
            GeneratedLexicalAnalysisTokenPosition = RememberedLexicalAnalysisTokenPosition;
            FunctionDeclaration(LocalIsInline);
        }
        else
        {
            VariableType = CLASS_LEVEL;
            GeneratedLexicalAnalysisTokenPosition = RememberedLexicalAnalysisTokenPosition;
            Declaration(true);
        }
    }
    //put_unknown_adr( FUN_TYPE , prcls->fun_root , "function not declared" );
}

void ParserToVirtualCodeGenerator::FunctionDeclaration(const UnsignedInt IsInline)
{
    VariableType = LOCAL_LEVEL;

    LabelsAddresses.clear();

    const auto RememberedTokenSymbol = GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol;
    PointerToFunctionStack = 0;
    UnsignedInt LocalGeneratedVirtualCodeCommandsPositionIndex = NumberOfGeneratedVirtualCodeCommands;
    GenerateVirtualCodeCommand(VirtualCommandName::GET, 0, 0, 0);
    ProgramFunctionPointer = CreateFunction(NumberOfGeneratedVirtualCodeCommands, 1, IsInline);
    ParseFunctionDefinition();
    UnsignedInt GeneratedVirtualCodeCommandIndex2 = NumberOfGeneratedVirtualCodeCommands;
    GeneratedVirtualCode[LocalGeneratedVirtualCodeCommandsPositionIndex].TargetAddress = PointerToFunctionStack;
    GeneratedVirtualCode[LocalGeneratedVirtualCodeCommandsPositionIndex].Level = NumberOfFunctionsOverloaded;
    GeneratePushFunctionParameters(LocalGeneratedVirtualCodeCommandsPositionIndex);
    if (ProgramFunctionPointer)
        GeneratedVirtualCode[LocalGeneratedVirtualCodeCommandsPositionIndex].Type = static_cast<VirtualCommandDataType>(ProgramFunctionPointer->ParametersTypes[0]);
    //if (ProgramFunctionPointer && !(GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].CommandName == VirtualCommandName::OPR && GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Operation == VirtualCommandOperationType::FREE))
    if (ProgramFunctionPointer && !(GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].CommandName == VirtualCommandName::FREE))
    {
        //GenerateVirtualCodeCommand(VirtualCommandName::OPR, 0, 0, 0, VirtualCommandOperationType::FREE);
        GenerateVirtualCodeCommand(VirtualCommandName::FREE, 0, 0, 0);
        if (RememberedTokenSymbol != TokenSymbol::VoidSym)
            PrintError("return expression expected");
    }
    if (ProgramFunctionPointer)
    {
        //GenerateVirtualCodeCommand(VirtualCommandName::OPR, ProgramFunctionPointer->ParametersTypes[0], 0, 0, VirtualCommandOperationType::RET);
        GenerateVirtualCodeCommand(VirtualCommandName::RET, ProgramFunctionPointer->ParametersTypes[0], 0, 0);
        GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Level = NumberOfFunctionsOverloaded;
    }

    GenerateFillReturnOperationAddresses(LocalGeneratedVirtualCodeCommandsPositionIndex, GeneratedVirtualCodeCommandIndex2, PointerToFunctionStack);
    if (ProgramFunctionPointer != nullptr) //CORRECTA
        GenerateFillUnknownJumpAddresses(ProgramFunctionPointer, "label not declared");
}

void ParserToVirtualCodeGenerator::ParserToVirtualCodeGenerationUnit() //pierwsza w supercpp
{
    InitDataForParserToVirtualCodeGenerator();
    while (FindType(static_cast<UnsignedInt>(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol)) ||
          ((GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::StaticSym ||
            GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::InlineSym)
            && FindType(static_cast<UnsignedInt>(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition + 1].Symbol)))
            || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::EnumSym
            || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::ConstSym
            || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::ClassSym
            || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::StructSym
            || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::UnionSym
            || GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::TypeDefSym)
    {
        printf("POSSYM1 = %lu\n", GeneratedLexicalAnalysisTokenPosition);

        UnsignedInt IdentifierSymbol = true;
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::EnumSym)
        {
            IdentifierSymbol = false;
            EnumDefinition();
        }
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::ClassSym)
        {
            IdentifierSymbol = false;
            ParseClassDefinition();
        }
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::StructSym)
        {
            IdentifierSymbol = false;
            ParseClassDefinition();
        }
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::UnionSym)
        {
            IdentifierSymbol = false;
            UnionDefinition();
        }
        if (!IdentifierSymbol)
            continue;

        UnsignedInt IsInline = true;
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::StaticSym)
        {
            IsInline = false;
            GetNextTokenSymbol();
            printff("static ");
        }
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::InlineSym)
        {
            IsInline = true;
            GetNextTokenSymbol();
            printff("inline ");
        }
        const UnsignedInt RememberedLexicalAnalysisTokenPosition = GeneratedLexicalAnalysisTokenPosition;
        TypeSpecifier();
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::IdentifierSym && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition + 1].Symbol == TokenSymbol::LeftParSym)
        {
            GeneratedLexicalAnalysisTokenPosition = RememberedLexicalAnalysisTokenPosition;
            FunctionDeclaration(IsInline);
        }
        else
        {
            VariableType = GLOBAL_LEVEL;
            GeneratedLexicalAnalysisTokenPosition = RememberedLexicalAnalysisTokenPosition;
            //printf("POSSYM3 = %d %d %d %d %f %c\n", GeneratedLexicalAnalysisTokenPosition, pampos, GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].line, TABSTR1[GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol], GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type, GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].c);
            printf("POSSYM3 = %lu %lu %lu %lu %f %c\n", GeneratedLexicalAnalysisTokenPosition, RememberedLexicalAnalysisTokenPosition, GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].LineInFile, GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol, GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type, GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar);
            Declaration(true);
        }
        //printf("POSSYM2 = %d %d %d %d %f %c\n", GeneratedLexicalAnalysisTokenPosition, pampos, GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].line, TABSTR1[GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol], GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type, GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].c);
        printf("POSSYM2 = %lu %lu %lu %lu %f %c\n", GeneratedLexicalAnalysisTokenPosition, RememberedLexicalAnalysisTokenPosition, GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].LineInFile, GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol, GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type, GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar);
    }
    GenerateFillUnknownCallAddresses();
    ChangeCodeForOptimizingJumps(); //by zoptymalizowac || i &&
}

void ParserToVirtualCodeGenerator::GenerateFillReturnOperationAddresses(const UnsignedInt GeneratedVirtualCodeCommandStartIndex, const UnsignedInt GeneratedVirtualCodeCommandStopIndex, const UnsignedInt StackSize)
{
    for (UnsignedInt GeneratedVirtualCodeCommandIndex = GeneratedVirtualCodeCommandStartIndex; GeneratedVirtualCodeCommandIndex <= GeneratedVirtualCodeCommandStopIndex; GeneratedVirtualCodeCommandIndex++)
        //if (GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].CommandName == VirtualCommandName::OPR && GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].Type == static_cast<VirtualCommandDataType>(0) && GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].Operation == VirtualCommandOperationType::FREE)
            //GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].Type = static_cast<VirtualCommandDataType>(StackSize + SumOfParametersLength);
        if (GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].CommandName == VirtualCommandName::FREE && GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].Size == 0)
            GeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].Size = StackSize + SumOfParametersLength;
}

void ParserToVirtualCodeGenerator::GeneratePushFunctionParameters(const UnsignedInt GeneratedVirtualCodeCommandIndex)
{
    if (FunctionDeclarationParameters.size() > NumberOfProperFastRegisters)
        for (UnsignedInt LocalGeneratedVirtualCodeCommandIndex = GeneratedVirtualCodeCommandIndex; LocalGeneratedVirtualCodeCommandIndex < NumberOfGeneratedVirtualCodeCommands; LocalGeneratedVirtualCodeCommandIndex++)
            if ((GeneratedVirtualCode[LocalGeneratedVirtualCodeCommandIndex].CommandName == VirtualCommandName::LDV || GeneratedVirtualCode[LocalGeneratedVirtualCodeCommandIndex].CommandName == VirtualCommandName::SVV) && GeneratedVirtualCode[LocalGeneratedVirtualCodeCommandIndex].Kind == 1)
            {
                const auto DataType = FindType(static_cast<UnsignedInt>(GetTokenSymbolForType(GeneratedVirtualCode[LocalGeneratedVirtualCodeCommandIndex].Type)));
                GeneratedVirtualCode[LocalGeneratedVirtualCodeCommandIndex].TargetAddress = (SumOfParametersLength - (GeneratedVirtualCode[LocalGeneratedVirtualCodeCommandIndex].TargetAddress - 4)) + PointerToFunctionStack - DataType->Length;
            }
}

void ParserToVirtualCodeGenerator::Declaration(const UnsignedInt IsSemicolon)
{
    DeclarationSpecifiers();
    InitializedDeclaratorsList();
    if (IsSemicolon)
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::SemicolonSym)
            GetNextTokenSymbol();
        else
            PrintError("missing ;");
}

void ParserToVirtualCodeGenerator::DeclarationSpecifiers()
{
    TypeSpecifier();
}

void ParserToVirtualCodeGenerator::TypeSpecifier()
{
    //WYKRYC TU CZY TO TYP funkcyjny lub tablicowy
    //wtedy po bitand wykrywac cos innego
    if (FindType(static_cast<UnsignedInt>(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol)))
    {
    ala:
        TokenSymbolType = GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol;
        //if (typesym==0) { pobsym(); goto ala; }
        GetNextTokenSymbol();

        UnsignedInt Pointer = 0;
        while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::MulSym)
        {
            Pointer++;
            GetNextTokenSymbol();
        }
        TokenSymbolType = static_cast<TokenSymbol>(static_cast<UnsignedInt>(TokenSymbolType) + static_cast<UnsignedInt>(Pointer * ConstantForPointerToMultiply));
        if (Pointer)
            if (FindType(static_cast<UnsignedInt>(TokenSymbolType)) == nullptr)
                InitializeType(TokenSymbolType, static_cast<VirtualCommandDataType>(TokenSymbolType), 4, Pointer, nullptr, nullptr, nullptr);

        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::BitAndSym)
        {
            GetNextTokenSymbol();
            TokenSymbolType = static_cast<TokenSymbol>(static_cast<UnsignedInt>(TokenSymbolType) + ConstantBiggerForSymbolToAdd);

            //CZY GDZIES INDZIEJ NIE DODANY TYP SYMBOLU
            if (FindType(static_cast<UnsignedInt>(TokenSymbolType)) == nullptr) //CORRECTA
                InitializeType(TokenSymbolType, static_cast<VirtualCommandDataType>(TokenSymbolType), 4, 0, nullptr, nullptr, nullptr); //CORRECTA

            // gdy wywolanie funkcji z parametrem referncyjnym to
            // musze do funkcji przekazac adres zmiennej tzn:
            // mov eax,ebp
            // add eax,offset
            // mov param[n],eax
            // w czasie genracji kodu traktuje typesym+ConstantBiggerForSymbolToAdd jak wskaznik do typu
            // typesym i odwolanie jakby zamiast zmiennej ref bylo napisane *ref
        }
    }
    else
        PrintError("type identifier expected");
}

void ParserToVirtualCodeGenerator::InitializedDeclaratorsList()
{
    InitializedDeclarator();
    while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::CommaSym)
    {
        GetNextTokenSymbol();
        InitializedDeclarator();
    }
}

void ParserToVirtualCodeGenerator::InitializedDeclarator()
{
    Declarator();
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::AssignSym)
    {
        GetNextTokenSymbol();
        Initiator();
    }
}

void ParserToVirtualCodeGenerator::Declarator()
{
    DirectDeclarator();
}

void ParserToVirtualCodeGenerator::Initiator()
{
    UnsignedInt GeneratedVirtualCodeCommandIndex1 = NumberOfGeneratedVirtualCodeCommands;
    ExpressionAssignment();

    if (VariableType == GLOBAL_LEVEL)
        if (NumberOfGeneratedVirtualCodeCommands - GeneratedVirtualCodeCommandIndex1 == 1 && GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].CommandName == VirtualCommandName::LDC)
        {
            NumberOfGeneratedVirtualCodeCommands--;
            GlobalData.back().InitialValue = GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands].Value;
        }

    if (VariableType == LOCAL_LEVEL)
        if (NumberOfGeneratedVirtualCodeCommands - GeneratedVirtualCodeCommandIndex1 > 0)
        {
            auto LocalFoundType = FindType(LastTokenSymbolType);
            GenerateVirtualCodeCommand(VirtualCommandName::SVV, LocalFoundType->TypeNumericCode, 0, LastPointerToFunctionStack);
            GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Kind = 2;
            GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Level = LOCAL_LEVEL;
        }
}

std::shared_ptr<FunctionDefinition> ParserToVirtualCodeGenerator::CreateFunction(const UnsignedInt Address, const UnsignedInt Declare, const UnsignedInt IsInline)
{
    auto FunctionPointer = std::make_shared<FunctionDefinition>();
    FunctionPointer->Declare = Declare;
    FunctionPointer->VirtualAddress = Address - 1;
    FunctionPointer->IsInline = IsInline;
    return FunctionPointer;
}

UnsignedInt ParserToVirtualCodeGenerator::ArrayDeclaration(UnsignedInt& StackOffset, ArrayDefinition* ArrayDefinitionObject)
{
    GetNextTokenSymbol();
    UnsignedInt SizePosition = 0;
    UnsignedInt Size = 1;
    while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::LeftSquareSym)
    {
        printf("LSQUARE = %lu\n", GeneratedLexicalAnalysisTokenPosition);
        GetNextTokenSymbol();
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::NumberSym) //na razie rozroznia tylko stale tablice
        {
            ArrayDefinitionObject->SizeForEveryDimension[SizePosition] = (UnsignedInt)GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type;
            if (static_cast<UnsignedInt>(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type) <= 0)
                PrintError("bad size");
            else
                Size *= (static_cast<UnsignedInt>(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type));
            SizePosition++;
            GetNextTokenSymbol();
        }
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::RightSquareSym)
            GetNextTokenSymbol();
        else
            PrintError("unmatched [");
    }
    ArrayDefinitionObject->Kind = 2;
    ArrayDefinitionObject->NumberOfDimemsions = SizePosition;
    ArrayDefinitionObject->Length = Size * GetSizeForTokenType(TokenSymbolType);
    StackOffset += ArrayDefinitionObject->Length;
    return ArrayDefinitionObject->Length;
}

void ParserToVirtualCodeGenerator::SaveGlobalData(const UnsignedInt SymbolNumericCode, const TokenSymbol GlobalDataType, UnsignedInt DataSize)
{
    if (const auto SymbolInt = NumericCodeToStringSymbolsMap.find(SymbolNumericCode); SymbolInt != NumericCodeToStringSymbolsMap.end())
        GlobalData.emplace_back(0, SymbolInt->second, DataSize, static_cast<UnsignedInt>(GlobalDataType));
}

void ParserToVirtualCodeGenerator::ArrayDeclarationLocal(const UnsignedInt ArrayNumericCode)
{
    if (VariableType == LOCAL_LEVEL)
        if (!ProgramFunctionPointer->ArraysContainer.contains(ArrayNumericCode))
        {
            ProgramFunctionPointer->ArraysContainer[ArrayNumericCode] = make_shared<ArrayDefinition>(PointerToFunctionStack, FindType(static_cast<UnsignedInt>(TokenSymbolType)));
            ArrayDeclaration(PointerToFunctionStack, ProgramFunctionPointer->ArraysContainer[ArrayNumericCode].get());
        }
        else
            PrintError("local array already defined");
}

void ParserToVirtualCodeGenerator::ArrayDeclarationGlobal(const UnsignedInt ArrayNumericCode)
{
    if (VariableType == GLOBAL_LEVEL)
        if (!ProgramPointer->ArraysContainer.contains(ArrayNumericCode))
        {
            ProgramPointer->ArraysContainer[ArrayNumericCode] = make_shared<ArrayDefinition>(PointerToGlobalStack, FindType(static_cast<UnsignedInt>(TokenSymbolType)));
            UnsignedInt tab_length = ArrayDeclaration(PointerToGlobalStack, ProgramPointer->ArraysContainer[ArrayNumericCode].get());
            SaveGlobalData(ArrayNumericCode, TokenSymbol::UnsignedCharSym, tab_length);
            ProgramPointer->ArraysContainer[ArrayNumericCode]->IndexToDescribeInGlobalDataArray = GlobalData.size() - 1;
            //napisz nazwy w pliku asemblerowym,a po sredniku adres-liczbe
            //CO GDY obliczenia NA STALYCH wowczas mov ebx,offset tab1+1234 ; 2045
        }
        else
            PrintError("global array already defined");
}

void ParserToVirtualCodeGenerator::ArrayDeclarationClass(const UnsignedInt ArrayNumericCode)
{
    if (VariableType == CLASS_LEVEL)
        if (!ProgramClassPointer->ArraysContainer.contains(ArrayNumericCode))
        {
            ProgramClassPointer->ArraysContainer[ArrayNumericCode] = make_shared<ArrayDefinition>(PointerToClassStack, FindType(static_cast<UnsignedInt>(TokenSymbolType)));
            UnsignedInt tab_length = ArrayDeclaration(PointerToClassStack, ProgramClassPointer->ArraysContainer[ArrayNumericCode].get());
            ProgramClassPointer->ArraysContainer[ArrayNumericCode]->SafetyLevel = SafenessLevelOfClass;
            // //put_global( kod_tab , charsym , tab_length );
            // //vtab->IndexToDescribeInGlobalDataArray = ile_global;
            printf("ADDITION TAB in CLASS %s\n", ClassName.c_str());
        }
        else
            PrintError("array in that class or in inherited class is already defined");
}

void ParserToVirtualCodeGenerator::ArrayDeclaration()
{
    const auto kod_tab = (UnsignedInt)GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type;
    ArrayDeclarationLocal(kod_tab);
    ArrayDeclarationGlobal(kod_tab);
    ArrayDeclarationClass(kod_tab);
}

void ParserToVirtualCodeGenerator::VariableDeclarationLocal()
{
    //ZADBAC o zmienne i tablice PRZECIAZONE = ZAGNIEZDZONE
    //TypeOfVariableDeclaration = 1-parametr,2-local,3-global,4-in_class
    if (VariableType == LOCAL_LEVEL)
    {
        if (!ProgramFunctionPointer->VariablesContainer.contains(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type))
        {
            LastPointerToFunctionStack = PointerToFunctionStack;
            LastTokenSymbolType = static_cast<UnsignedInt>(TokenSymbolType);
            ProgramFunctionPointer->VariablesContainer[GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type] = make_shared<VariableDefinition>(PointerToFunctionStack += GetSizeForTokenType(TokenSymbolType), TypeOfVariableDeclaration, FindType(static_cast<UnsignedInt>(TokenSymbolType)));
            GetNextTokenSymbol();
        }
        else
            PrintError("local array already defined");
    }
}

void ParserToVirtualCodeGenerator::VariableDeclarationGlobal()
{
    if (VariableType == GLOBAL_LEVEL)
    {
        if (!ProgramPointer->VariablesContainer.contains(static_cast<UnsignedInt>(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type)))
        {
            TypeOfVariableDeclaration = 3;
            LastPointerToFunctionStack = PointerToGlobalStack;
            SaveGlobalData(static_cast<UnsignedInt>(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type), TokenSymbolType, 0);
            LastTokenSymbolType = static_cast<UnsignedInt>(TokenSymbolType);
            ProgramPointer->VariablesContainer[static_cast<UnsignedInt>(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type)] = make_shared<VariableDefinition>(PointerToGlobalStack += GetSizeForTokenType(TokenSymbolType), TypeOfVariableDeclaration, FindType(static_cast<UnsignedInt>(TokenSymbolType)));
            ProgramPointer->VariablesContainer[static_cast<UnsignedInt>(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type)]->IndexToDescribeInGlobalDataArray = GlobalData.size() - 1;
            GetNextTokenSymbol();
        }
        else
            PrintError("global variable already defined");
    }
}

void ParserToVirtualCodeGenerator::VariableDeclarationClass()
{
    if (VariableType == CLASS_LEVEL)
    {
        if (!ProgramClassPointer->VariablesContainer.contains(static_cast<UnsignedInt>(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type)))
        {
            TypeOfVariableDeclaration = 4;
            LastPointerToFunctionStack = PointerToClassStack;
            //put_global((UnsignedInt)GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type, typesym, 0);
            LastTokenSymbolType = static_cast<UnsignedInt>(TokenSymbolType);
            ProgramClassPointer->VariablesContainer[static_cast<UnsignedInt>(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type)] = make_shared<VariableDefinition>(PointerToClassStack += GetSizeForTokenType(TokenSymbolType), TypeOfVariableDeclaration, FindType(static_cast<UnsignedInt>(TokenSymbolType)));
            ProgramClassPointer->VariablesContainer[static_cast<UnsignedInt>(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type)]->SafetyLevel = SafenessLevelOfClass;
            GetNextTokenSymbol();
            printf("ADDITION TAB in CLASS %s\n", ClassName.c_str());
        }
        else
            PrintError("variable in that class or in inherited class is already defined");
    }
}

void ParserToVirtualCodeGenerator::VariableDeclaration()
{
    VariableDeclarationLocal();
    VariableDeclarationGlobal();
    VariableDeclarationClass();
}

void ParserToVirtualCodeGenerator::DirectDeclarator()
{
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::IdentifierSym)
    {
        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition + 1].Symbol == TokenSymbol::LeftSquareSym)
            ArrayDeclaration();
        else
            VariableDeclaration();
    }
    else
        PrintError("error 1");

    EnterFunctionIdentifier();
}

bool ParserToVirtualCodeGenerator::CompareFunctionParameters(const shared_ptr<FunctionDefinition>& FunctionPointer) const
{
    bool TheSame = true;

    if (FunctionPointer->ParametersTypes.size() != FunctionDeclarationParameters.size())
        TheSame = false;
    else
    for (UnsignedInt FunctionParameterIndex = 0; FunctionParameterIndex < FunctionDeclarationParameters.size(); FunctionParameterIndex++)
        if (FunctionPointer->ParametersTypes[FunctionParameterIndex] != FunctionDeclarationParameters[FunctionParameterIndex])
            TheSame = false;

    return TheSame;
}

void ParserToVirtualCodeGenerator::SearchProperFunctionAccordingToFoundParametersTypes(shared_ptr<FunctionDefinition>& FunctionToCheckPointer, const UnsignedInt FunctionNumericCode)
{
    bool TheSame = CompareFunctionParameters(FunctionToCheckPointer);
    UnsignedInt FunctionIndex;
    for (FunctionIndex = 0; FunctionIndex < FunctionToCheckPointer->FunctionsOverloaded.size(); FunctionIndex++)
    {
        auto FunctionPointer = FunctionToCheckPointer->FunctionsOverloaded[FunctionIndex];
        TheSame = CompareFunctionParameters(FunctionPointer);
        if (TheSame)
            FunctionToCheckPointer = FunctionPointer;
        printff(" I & brat%lu=%d;", FunctionIndex, TheSame);
    }
    NumberOfFunctionsOverloaded = FunctionIndex;
    if (!TheSame)
    {
        printff(" I overload function %s = %lu", ProgramFunctionPointer->FunctionNameString.c_str(), FunctionNumericCode);

        UnsignedInt counter = FunctionToCheckPointer->FunctionsOverloaded.size();
        FunctionToCheckPointer->FunctionsOverloaded.emplace_back(ProgramFunctionPointer);

        //if (counter >= MAX_FUN_OVERLOAD)
        //    PrintError("too many functions overloaded");

        NumberOfFunctionsOverloaded = FunctionToCheckPointer->FunctionsOverloaded.size();
    }

    UnsignedInt RememberDeclare = FunctionToCheckPointer->Declare;
    if (TheSame && RememberDeclare == 0 && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition + 1].Symbol == TokenSymbol::LeftCurlySym)
    {
        FunctionToCheckPointer->Declare = 1;
        FunctionToCheckPointer->VirtualAddress = NumberOfGeneratedVirtualCodeCommands - 1;
        printff(" I fill the body of predeclaration %s=%lu", ProgramFunctionPointer->FunctionNameString.c_str(), FunctionNumericCode);
        FunctionToCheckPointer->VariablesContainer = ProgramFunctionPointer->VariablesContainer;
        FunctionToCheckPointer->ArraysContainer = ProgramFunctionPointer->ArraysContainer;
        ProgramFunctionPointer = FunctionToCheckPointer;
    }
    if (TheSame && RememberDeclare == 0 && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition + 1].Symbol == TokenSymbol::SemicolonSym)
        PrintError("two declarations of functions have the list of the same types");
    if (TheSame && RememberDeclare == 1)
        PrintError("first you gave the body , after you want to redeclare");
}

void ParserToVirtualCodeGenerator::EnterFunctionIdentifier()
{
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::LeftParSym)
    {
        NumberOfFunctionsOverloaded = 0;
        SumOfParametersLength = 0;
        auto FunctionResultTokenSymbol = TokenSymbolType;
        auto FunctionNumericCode = (UnsignedInt)GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition - 1].Type;
        FunctionDeclarationParameters.clear();
        FunctionDeclarationParameters.emplace_back(FunctionNumericCode);
        UnsignedInt log_public = true;

        UnsignedInt kod_main = false;

        auto FunctionMapCode = NumericCodeToStringSymbolsMap.find(FunctionNumericCode);
        auto FunctionStringName = FunctionMapCode->second;
        if (FunctionStringName == "main")
            kod_main = true;

        GetNextTokenSymbol();
        TypeOfVariableDeclaration = 1;
        ParametersTypesList();
        TypeOfVariableDeclaration = 2;

        printf("%s = ", FunctionStringName.c_str());
        for (unsigned long FunctionDeclarationParameter : FunctionDeclarationParameters)
            printff("%lu_", FunctionDeclarationParameter);

        ProgramFunctionPointer->ParametersTypes = FunctionDeclarationParameters;

        ProgramFunctionPointer->TypeOfResult = FindType(static_cast<UnsignedInt>(FunctionResultTokenSymbol));
        ProgramFunctionPointer->FunctionNameString = FunctionStringName;
        if (InsideClassesNestedLevel > 0)
            ProgramFunctionPointer->SafetyLevel = SafenessLevelOfClass;

        UnsignedInt Level;
        auto FunctionPointer = FindFunction(FunctionNumericCode, Level);

        if (!FunctionPointer)
            ProgramClassPointer->FunctionsContainer[FunctionNumericCode] = ProgramFunctionPointer;

        if (InsideClassesNestedLevel > 0 && !FunctionPointer)
            printf(" ADDITION function to class %s", ClassName.c_str());

        if (FunctionPointer)
            SearchProperFunctionAccordingToFoundParametersTypes(FunctionPointer, FunctionNumericCode);

        if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::RightParSym)
            GetNextTokenSymbol();
        else
            PrintError("right paren expected");

        GenerateFunctionIdentifierCode(FunctionStringName, log_public, kod_main);
    }
}

void ParserToVirtualCodeGenerator::GenerateFunctionIdentifierCode(const string& StringCode, UnsignedInt PublicCode, UnsignedInt MainCode)
{
    if (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::SemicolonSym)
    {
        NumberOfGeneratedVirtualCodeCommands--; // by zmazac wczesniejsze gen( prz,0,0 );
        ProgramFunctionPointer->Declare = 0;
        ProgramFunctionPointer->OverloadNumber = NumberOfFunctionsOverloaded;
        ProgramFunctionPointer->VariablesContainer.clear();
        ProgramFunctionPointer->ArraysContainer.clear();
        //NIE delete funptr bo deklaracja musi przetrwac do wykrycia ciala !
        //lub dla Linkera ?
        ProgramFunctionPointer = nullptr; //by nie generowal gen( opr,0,END=-1 );
        PublicCode = false; //ze to tylko deklaracja
    }
    else
    if (MainCode)
        StartAddressOfProgram = ProgramFunctionPointer->VirtualAddress;

    if (PublicCode)
    {
        PublicData.emplace_back(StringCode, NumberOfFunctionsOverloaded);
        if (FunctionDeclarationParameters.size() <= NumberOfProperFastRegisters) //num_of_par_fun = NumberOfProperFastRegisters gdy 6 parametrow
        {
            UnsignedInt StackOffset = 4; //na starcie dx=4
            for (UnsignedInt FunctionParameterIndex = 1; FunctionParameterIndex < FunctionDeclarationParameters.size(); FunctionParameterIndex++)
            {
                //te typy parametrow to problem :
                //double,extended,strukturo_classy i tablice
                const auto TypeObject = FindType(FunctionDeclarationParameters[FunctionParameterIndex]);
                if (TypeObject != nullptr) //CORRECTA powoduje blad - nie generuje np: dla proba( , UnsignedInt& r )
                {
                    GenerateVirtualCodeCommand(VirtualCommandName::LDP, TypeObject->TypeNumericCode, 0, StackOffset);
                    GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Kind = FunctionParameterIndex;

                    StackOffset += TypeObject->Length;
                }
            }
        }
        else
        if (FunctionDeclarationParameters.size() > NumberOfProperFastRegisters) //num_of_par_fun = NumberOfProperFastRegisters gdy 6 parametrow
        {
            //UnsignedInt FunctionStackOffset = GeneratedVirtualCode[NumberOfGeneratedVirtualCodeCommands - 1].Value; // wartosc instrukcji prz czyli get
            for (UnsignedInt FunctionDeclarationParameterIndex = 1; FunctionDeclarationParameterIndex < FunctionDeclarationParameters.size(); FunctionDeclarationParameterIndex++)
            {
                const auto TypeObject = FindType(FunctionDeclarationParameters[FunctionDeclarationParameterIndex]);
                SumOfParametersLength += TypeObject->Length;
            }
            PointerToFunctionStack -= SumOfParametersLength;
        }
    }
    printff(" \n");
}

void ParserToVirtualCodeGenerator::ParametersTypesList()
{
    ParametersList();
}

void ParserToVirtualCodeGenerator::ParametersList()
{
    if (!(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::VoidSym && GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition + 1].Symbol == TokenSymbol::RightParSym))
    {
        ParameterDeclaration();
        FunctionDeclarationParameters.emplace_back(static_cast<UnsignedInt>(TokenSymbolType));
        while (GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol == TokenSymbol::CommaSym)
        {
            GetNextTokenSymbol();
            ParameterDeclaration();
            FunctionDeclarationParameters.emplace_back(static_cast<UnsignedInt>(TokenSymbolType));
        }
    }
    else
        GetNextTokenSymbol();
}

void ParserToVirtualCodeGenerator::ParameterDeclaration()
{
    DeclarationSpecifiers();
    Declarator();
}
