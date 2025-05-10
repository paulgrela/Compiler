
#ifndef ASSEMBLER_CODE_GENERATOR_H
#define ASSEMBLER_CODE_GENERATOR_H

#include <stack>
#include <string>
#include <vector>
#include <unordered_map>

class IntelAssemblerCodeGenerator
{
    const char* SpacesTabulator = "            ";
private:
    UnsignedInt NumberOfGeneratedVirtualCodeCommands;
    std::vector<VirtualCodeCommand>& ParserGeneratedVirtualCode;
private:
    FILE* IntelAssemblerCodeFile{};
    std::string NameOfIntelAssemblerOutputFile{};
private:
    std::stack<UnsignedInt> RegisterStack{};
private:
    UnsignedInt InsideFunctionParametersLevel{};
    std::array<std::string, 100> FunctionParameters{};
    std::array<std::array<UnsignedInt, MaximalNumberOfInsideFunctionsCallForUsageOfRegisters>, 7> ParametersUsageArray{};
private:
    UnsignedInt VirtualCodeCommandIndex1{};
    UnsignedInt VirtualCodeCommandIndex2{};
    UnsignedInt AdditionalPrinting1{};
    UnsignedInt AdditionalPrinting2{};
    UnsignedInt AdditionalPrinting{};
    VirtualCommandDataType DataTypeNumericCode{};
    UnsignedInt NegEAX{};
    SignedInt PushEAX{};
    FreeRegistersDefinition FreeRegisters{};
private:
    UnsignedInt StartAddress;
private:
    std::vector<std::string>& ListOfConstantStrings;
    std::vector<CallDefinition>& ExternData;
    std::vector<CallDefinition>& PublicData;
    std::vector<GlobalDefinition>& GlobalData;
private:
    std::vector<UnsignedInt> JumpAddresses{};
private:
    std::unordered_map<UnsignedInt, std::string>& NumericCodeToStringSymbolsMap;
public:
    IntelAssemblerCodeGenerator(std::string NameOfOutputIntelAssemblerFileParam, std::vector<VirtualCodeCommand>& ParserGeneratedVirtualCodeParam, UnsignedInt NumberOfGeneratedVirtualCodeCommandsParam, UnsignedInt StartAddressParam, std::unordered_map<UnsignedInt, std::string>& NumericCodeToStringSymbolsMapParam, std::vector<std::string>& ListOfConstantStringsParam, std::vector<CallDefinition>& ExternDataParam, std::vector<CallDefinition>& PublicDataParam, std::vector<GlobalDefinition>& GlobalDataParam) : NameOfIntelAssemblerOutputFile(std::move(NameOfOutputIntelAssemblerFileParam)), ParserGeneratedVirtualCode(ParserGeneratedVirtualCodeParam), NumberOfGeneratedVirtualCodeCommands(NumberOfGeneratedVirtualCodeCommandsParam), StartAddress(StartAddressParam), NumericCodeToStringSymbolsMap(NumericCodeToStringSymbolsMapParam), ListOfConstantStrings(ListOfConstantStringsParam), ExternData(ExternDataParam), PublicData(PublicDataParam), GlobalData(GlobalDataParam)
    {
    }
public:
    void PrintIntelAssemblerCodeToFile();
private:
    [[nodiscard]] bool CheckParameters(UnsignedInt VirtualCodeCommandIndex) const;
private:
    void RememberJumpAddresses();
    void CountConditionalJump(const char* StringToPrint, UnsignedInt VirtualCodeCommandIndex, FILE* IntelAssemblerCodeFile, UnsignedInt& log_a);
    void SetFunctionParameters();
    void SetFunctionParametersUsageArrayToZero();
    void AfterCall(UnsignedInt i);
    void GenerateIntelAssemblerCodeForFunctionStackReserveOperation(UnsignedInt VirtualCodeCommandIndex);
    void InitRegistersParameters();
    void PrintExternVariablesDeclarations() const;
    void PrintPublicVariablesDeclarations() const;
    void PrintGlobalVariablesDeclarations() const;
    void PrintProperNumberOfSpacesForOrderOrLabel(UnsignedInt VirtualCodeCommandIndex) const;
    void PrintDataSegment() const;
    void GenerateIntelAssemblerCodeForEndFunction(UnsignedInt VirtualCodeCommandIndex);
    void GenerateIncrementationOrDecrementationOperations(UnsignedInt VirtualCodeCommandIndex, const std::string& ofst, const char* Str1, const char* Str2);
    bool GenerateArithmeticalGroup1Operations(UnsignedInt VirtualCodeCommandIndex);
    bool GenerateArithmeticalGroup2Operations(UnsignedInt VirtualCodeCommandIndex);
    void GenerateCondition(UnsignedInt VirtualCodeCommandIndex, const std::string& ConditionString);
    void GenerateConditionalOperations(UnsignedInt VirtualCodeCommandIndex);
    void GenerateConditionOperation(UnsignedInt VirtualCodeCommandIndex, const char* FirstCondition, const char* SecondCondition);
    void GenerateIntelAssemblerCodeForOperation(UnsignedInt VirtualCodeCommandIndex, const std::string& ofst);
    bool GenerateIntelAssemblerCodeForNegOrNotOperations(UnsignedInt VirtualCodeCommandIndex);
    void GenerateIntelAssemblerCodeForCallOrIncOrDecOperations(const char* Str);
    void GenerateIntelAssemblerCodeForReturnFromFunctionOperation(UnsignedInt VirtualCodeCommandIndex);
    void GenerateIntelAssemblerCodeForIncDecWithJump(UnsignedInt VirtualCodeCommandIndex, const std::string& ofst, UnsignedInt& quest, UnsignedInt& log_bracket);
    void GenerateIntelAssemblerCodeForSavingVariable(UnsignedInt VirtualCodeCommandIndex, const std::string& ofst);
    void GenerateIntelAssemblerCodeForLoadingAddress(UnsignedInt VirtualCodeCommandIndex);
    void GenerateIntelAssemblerCodeForSavingAddress(UnsignedInt VirtualCodeCommandIndex);
    void GenerateIntelAssemblerCodeForLoadingFromAddress(UnsignedInt VirtualCodeCommandIndex);
    void GenerateIntelAssemblerCodeForLoading(UnsignedInt VirtualCodeCommandIndex);
    void GenerateIntelAssemblerCodeForLoadingPushing(UnsignedInt VirtualCodeCommandIndex);
    void GenerateIntelAssemblerCodeForGettingMemoryOnStack(UnsignedInt VirtualCodeCommandIndex);
    void GenerateIntelAssemblerCodeForUnconditionalJump(UnsignedInt VirtualCodeCommandIndex);
    void GenerateIntelAssemblerCodeForNoRet(UnsignedInt VirtualCodeCommandIndex);
    void GenerateIntelAssemblerCodeForCallingFunction(UnsignedInt VirtualCodeCommandIndex);
    void GenerateIntelAssemblerCodeForSpecialPopEdx(UnsignedInt VirtualCodeCommandIndex);
    void GenerateIntelAssemblerCodeForSpecialPopEdx0(UnsignedInt VirtualCodeCommandIndex);
    void GenerateIntelAssemblerCodeForSpecialPopEdxPush(UnsignedInt VirtualCodeCommandIndex);
    void GenerateIntelAssemblerCodeForSta(UnsignedInt VirtualCodeCommandIndex);
    void GenerateIntelAssemblerCodeForNoCall(UnsignedInt VirtualCodeCommandIndex);
    void GenerateIntelAssemblerCodeForNull(UnsignedInt VirtualCodeCommandIndex);
    void GenerateIntelAssemblerCodeForStack0(UnsignedInt VirtualCodeCommandIndex);
    void GenerateIntelAssemblerCodeForSavePar(UnsignedInt VirtualCodeCommandIndex);

    [[nodiscard]] RealType GetProperValueOrTargetAddress(UnsignedInt VirtualCodeCommandIndex) const;
};

#endif
