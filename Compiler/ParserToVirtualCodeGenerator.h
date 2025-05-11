
#ifndef PARSER_TO_VIRTUAL_CODE_GENERATOR_H
#define PARSER_TO_VIRTUAL_CODE_GENERATOR_H

#include <vector>
#include <string>
#include <unordered_map>

#include "Types.h"
#include "Constants.h"

class ParserToVirtualCodeGenerator
{
public:
    std::vector<VirtualCodeCommand> GeneratedVirtualCode;
    UnsignedInt NumberOfGeneratedVirtualCodeCommands;
public:
    bool VerbosePrintingOfInformation;
public:
    const std::vector<LexicalAnalysisTokenSymbol>& GeneratedLexicalAnalysisTokens;
public:
    const std::unordered_map<UnsignedInt, std::string>& NumericCodeToStringSymbolsMap;
public:
    UnsignedInt PointerToClassStack;
    UnsignedInt PointerToGlobalStack;
    UnsignedInt PointerToFunctionStack;
public:
    UnsignedInt LastPointerToFunctionStack;
    TokenSymbol TokenSymbolType;
    TokenSymbol NewLastTokenSymbolType;
    UnsignedInt LastTokenSymbolType;
    UnsignedInt VariableType;
    UnsignedInt NumberOfFunctionsOverloaded;
public:
    UnsignedInt LineInFile;
    UnsignedInt GeneratedLexicalAnalysisTokenPosition;
    UnsignedInt SumOfParametersLength;
    UnsignedInt SafenessLevelOfClass;
    UnsignedInt InsideFunctionsNextedLevel;
    UnsignedInt InsideLoopsNestedLevel;
    UnsignedInt InsideClassesNestedLevel;
public:
    UnsignedInt StartAddressOfProgram;
public:
    std::string ClassName;
    std::shared_ptr<ClassDefinition> ProgramPointer;
    std::shared_ptr<ClassDefinition> ProgramClassPointer;
    std::shared_ptr<FunctionDefinition> ProgramFunctionPointer;
public:
    std::vector<UnsignedInt> FunctionDeclarationParameters;
    std::vector<UnsignedInt> FunctionCallParameters;
    UnsignedInt TypeOfVariableDeclaration;
public:
    std::vector<AddressDefinition> FunctionsAddresses;
    std::vector<AddressDefinition> LabelsAddresses;
public:
    std::vector<GlobalDefinition> GlobalData;
public:
    std::vector<CallDefinition> ExternData;
    std::vector<CallDefinition> PublicData;
public:
    ParserToVirtualCodeGenerator(const std::vector<LexicalAnalysisTokenSymbol>& GeneratedLexicalAnalysisTokensParam, const std::unordered_map<UnsignedInt, std::string>& SymbolsIntParam) : NumericCodeToStringSymbolsMap(SymbolsIntParam), GeneratedVirtualCode(MaximalNumberOfGeneratedVirtualCodeCommands), GeneratedLexicalAnalysisTokens(GeneratedLexicalAnalysisTokensParam)
    {
    }
public:
    void ExpressionOneArgumentIncOrDec();
    void ExpressionOneArgumentBitAnd();
    void ExpressionOneArgumentStrPtr();
    void ExpressionOneArgumentIdentifier();
    void ExpressionOneArgumentMulLeftPar();
    void ExpressionOneArgumentMul();
    void ExpressionOneArgumentLeftPar1();
    void ExpressionOneArgumentLeftPar2();
    void ExpressionOneArgumentSizeOf();
    void ExpressionOneArgumentBitAndOrPlusOrMinusOrNotOrTilde();
public:
    void ExpressionSimpleIdentifierLeftSquare();
    void ExpressionSimpleIdentifierLeftPar(UnsignedInt& FunctionCallParameterIndex);
    void ExpressionSimpleFunctionCall(std::shared_ptr<FunctionDefinition>& FunctionPointer, UnsignedInt NumberOfFunctionParameters, UnsignedInt& CallAddress, UnsignedInt& MaxOk, UnsignedInt& PosMaxOk);
    void ExpressionSimpleIdentifierRightPar(const std::shared_ptr<FunctionDefinition>& FunctionPointer, const UnsignedInt& CallAddress, UnsignedInt NumericFunctionCode, UnsignedInt PositionMaximalOK);
    void ExpressionSimpleIdentifierLeftParFunctionCallRightPar();
    void ExpressionSimpleIdentifier();
    void ExpressionSimpleNumber();
    void ExpressionSimpleLeftRightPar();
    void ExpressionSimple();
public:
    void ExpressionSuffix();
    void ExpressionOneArgument();
    void ExpressionCast();
    void ExpressionPower();
    void ExpressionMultiplicative();
    void ExpressionAdditive();
    void ExpressionShift();
    void ExpressionRelation();
    void ExpressionComparison();
    void ExpressionAND();
    void ExpressionXOR();
    void ExpressionOR();
    void ExpressionLogicAND();
    void ExpressionLogicOR();
    void ExpressionConditional();
    void GenerateAssignmentWithAdditionalActionOperationCode(TokenSymbol Token, UnsignedInt lr1);
    void GenerateAssignmentOperationCode();
    void ExpressionAssignment();
    void Expression();
public:
    void FunctionDeclaration(UnsignedInt IsInline);
    void DeclarationSpecifiers();
    void TypeSpecifier();
    void Declarator();
    void DirectDeclarator();
    void Declaration(UnsignedInt IsSemicolon);
    void InitializedDeclaratorsList();
    void InitializedDeclarator();
    void ArrayDeclarationLocal(UnsignedInt ArrayNumericCode);
    void ArrayDeclarationGlobal(UnsignedInt ArrayNumericCode);
    void ArrayDeclarationClass(UnsignedInt ArrayNumericCode);
    void VariableDeclarationLocal();
    void VariableDeclarationGlobal();
    void VariableDeclarationClass();
    void ArrayDeclaration();
    void VariableDeclaration();
    void ParametersTypesList();
    void ParametersList();
    void ParameterDeclaration();
    void SaveGlobalData(UnsignedInt SymbolNumericCode, TokenSymbol GlobalDataType, UnsignedInt DataSize);
    UnsignedInt ArrayDeclaration(UnsignedInt& StackOffset, ArrayDefinition* ArrayDefinitionObject);
public:
    void ComplexInstruction();
    void InstructionLoopWhile();
    void InstructionLoopFor();
    void InstructionLoopDo();
    void InstructionLoop();
    void InstructionExpression();
    void InstructionContinue();
    void InstructionBreak();
    void InstructionGoto();
    void InstructionLabel();
    void InstructionDelete();
    void InstructionNew();
    void InstructionChoiceIf();
    void InstructionChoiceSwitch();
    void ChoiceInstruction();
    void JumpInstruction();
    void DeclarationInstruction();
    void Initiator();
    void Instruction();
    void ParseFunctionDefinition();
    void ParseInheritanceListInClassDefinition();
    void ParseClassElementsDefinition();
    void ParseClassDefinition();
    void UnionDefinition();
    void EnumDefinition();
    void ClassUnit();
    void ParserToVirtualCodeGenerationUnit();
    void EnterFunctionIdentifier();
    void SearchProperFunctionAccordingToFoundParametersTypes(std::shared_ptr<FunctionDefinition>& FunctionToCheckPointer, UnsignedInt FunctionNumericCode);
    void GenerateFunctionIdentifierCode(const std::string& StringCode, bool PublicCodeBool, bool MainFunctionBool);
    [[nodiscard]] bool CompareFunctionParameters(const std::shared_ptr<FunctionDefinition>& FunctionPointer) const;
public:
    [[nodiscard]] std::shared_ptr<VariableDefinition> CreateVariable(UnsignedInt Address, UnsignedInt Kind) const;
    [[nodiscard]] std::shared_ptr<ArrayDefinition> CreateArray(UnsignedInt Address, UnsignedInt TypeCode) const;
    static std::shared_ptr<FunctionDefinition> CreateFunction(UnsignedInt Address, UnsignedInt Declare, UnsignedInt IsInline);
    static std::shared_ptr<TypeDefinition> CreateDescriptionOfType(UnsignedInt TypeNumericCode, UnsignedInt TypeLength, UnsignedInt PointerPar, const std::shared_ptr<ClassDefinition>& ClassPointer,  const std::shared_ptr<FunctionDefinition>& FunctionPointer, const std::shared_ptr<ArrayDefinition>& ArrayPointer);
    void InitializeType(TokenSymbol TypeTokenSymbol, VirtualCommandDataType TypeNumericCode, UnsignedInt TypeLength, UnsignedInt Pointer, const std::shared_ptr<ClassDefinition>& ClassPointer, const std::shared_ptr<FunctionDefinition>& FunctionPointer, const std::shared_ptr<ArrayDefinition>& ArrayPointer) const;
public:
    void PointerExpressionSum(const UnsignedInt& OperationCode, const std::shared_ptr<VariableDefinition>& VariablePointer, UnsignedInt& InsidePointerExpression);
public:
    std::shared_ptr<ArrayDefinition> FindArray(UnsignedInt ArrayTypeCode, UnsignedInt& Level) const;
    std::shared_ptr<VariableDefinition> FindVariable(UnsignedInt VariableTypeCode, UnsignedInt& Level) const;
    std::shared_ptr<FunctionDefinition> FindFunction(UnsignedInt TypeCode, UnsignedInt& Level) const;
    [[nodiscard]] std::shared_ptr<TypeDefinition> FindType(UnsignedInt TypeCode) const;
public:
    void CheckBalanceOfParenthesis();
    void FillBreakContinueLabelsAddresses(UnsignedInt GeneratedVirtualCodeCommandIndex, UnsignedInt GeneratedVirtualCodeCommandIndexAdditional);
    void GetNextTokenSymbol()
    {
        GeneratedLexicalAnalysisTokenPosition++;
    };
    void InitDataForParserToVirtualCodeGenerator();
    void PrintError(std::string_view ErrorString);
    void PrintErrorToFile(UnsignedInt PositionInFile, std::string_view ErrorString);
    void GenerateVirtualCodeCommand(VirtualCommandName CommandNameParam, SignedInt TypeParam, RealType ValueParam, UnsignedInt TargetAddressParam, VirtualCommandOperationType OperationParam = VirtualCommandOperationType::NOP);
    //void GenerateVirtualCodeCommand(VirtualCommandName CommandNameParam, VirtualCommandDataType TypeParam, RealType ValueParam, UnsignedInt TargetAddressParam, VirtualCommandOperationType OperationParam = VirtualCommandOperationType::NOP);
    void GenerateAssignOperation(VirtualCommandOperationType z);
    [[nodiscard]] std::shared_ptr<TypeDefinition> CheckType(UnsignedInt GeneratedLexicalAnalysisTokenIndex) const;
    [[nodiscard]] UnsignedInt GetSizeForTokenType(TokenSymbol TokenSymbolToGetSize) const;
    static TokenSymbol GetTokenSymbolForType(VirtualCommandDataType TypeNumericCode);
public:
    void GeneratePushFunctionParameters(UnsignedInt GeneratedVirtualCodeCommandIndex);
    void GenerateFillReturnOperationAddresses(UnsignedInt GeneratedVirtualCodeCommandStartIndex, UnsignedInt GeneratedVirtualCodeCommandStopIndex, UnsignedInt StackSize);
    void GenerateFillUnknownCallAddresses();
    void GenerateFillUnknownJumpAddresses(const std::shared_ptr<FunctionDefinition>& LabelFunctionPointer, std::string_view ErrorString);
    void ChangeCodeForOptimizingJumps();
};

#endif
