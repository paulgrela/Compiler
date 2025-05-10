// POSPRZATAC GENERATE gdy call -fun,proc dobrze

//UWAGA - USUWAC ZBEDNE ETYKIET Z ASM TZN USWAC TAKIE DO KTORYCH NIE MA SKOKU
//UZALEZNIC TO OD WOLI UZYTKOWNIKA BO ETYKIETY SA POTRZEBNE DO ANALIZY PROGRAMU

#include <stack>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "Types.h"
#include "Constants.h"
#include "IntelAssemblerCodeGenerator.h"

using namespace std;

RealType IntelAssemblerCodeGenerator::GetProperValueOrTargetAddress(const UnsignedInt VirtualCodeCommandIndex) const
{
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].TargetAddress > 0)
    {
        if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].TargetAddress == UINT64_MAX)
            return -9999;
        else
            return ParserGeneratedVirtualCode[VirtualCodeCommandIndex].TargetAddress;
    }
    else
        return ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Value;
}

void IntelAssemblerCodeGenerator::RememberJumpAddresses()
{
    JumpAddresses.clear();
    for (UnsignedInt GeneratedVirtualCodeCommandIndex = 0; GeneratedVirtualCodeCommandIndex < NumberOfGeneratedVirtualCodeCommands; GeneratedVirtualCodeCommandIndex++)
        if (ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].CommandName == VirtualCommandName::JCON ||
            ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].CommandName == VirtualCommandName::JMP ||
            ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].CommandName == VirtualCommandName::CALL ||
            ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].CommandName == VirtualCommandName::JCONT ||
            ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].CommandName == VirtualCommandName::VIRTCALL ||
            ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].CommandName == VirtualCommandName::VIRTRET ||
            ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].CommandName == VirtualCommandName::JBREAK ||
            ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].CommandName == VirtualCommandName::JGOTO)
            JumpAddresses.emplace_back(static_cast<UnsignedInt>(ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].TargetAddress));
}

void IntelAssemblerCodeGenerator::GenerateCondition(const UnsignedInt VirtualCodeCommandIndex, const string& ConditionString)
{
    fprintf(IntelAssemblerCodeFile, ConditionString.c_str());

    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName == VirtualCommandName::JCON)
        ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName = VirtualCommandName::NOP;

    VirtualCodeCommandIndex1++;
    AdditionalPrinting2 = 0;
}

void IntelAssemblerCodeGenerator::CountConditionalJump(const char* StringToPrint, const UnsignedInt VirtualCodeCommandIndex, FILE* IntelAssemblerCodeFile, UnsignedInt& log_a)
{
    const auto CommandValue = ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].Operation;
    const auto CommandName = ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName;
    if (CommandName == VirtualCommandName::OPR)
    {
        if (static_cast<SignedInt>(CommandValue) >= 8 && static_cast<SignedInt>(CommandValue) <= 13) //od JE do JBE
        {
            fprintf(IntelAssemblerCodeFile, "cmp eax,%s", StringToPrint);
            PushEAX--;
        }
        if (CommandValue == VirtualCommandOperationType::ADD)
        {
            fprintf(IntelAssemblerCodeFile, "add eax,%s", StringToPrint);
            ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].Type = static_cast<VirtualCommandDataType>(1000);
        }
        if (CommandValue == VirtualCommandOperationType::SUB)
        {
            fprintf(IntelAssemblerCodeFile, "sub eax,%s", StringToPrint);
            ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].Type = static_cast<VirtualCommandDataType>(1000);
        }
        if (CommandValue == VirtualCommandOperationType::SUB_ASS)
        {
            fprintf(IntelAssemblerCodeFile, "sub eax,%s", StringToPrint);
            ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].Type = static_cast<VirtualCommandDataType>(1000);
            NegEAX = true;
        }
    }
    if (CommandName == VirtualCommandName::OPR && CommandValue == VirtualCommandOperationType::MUL)
    {
        fprintf(IntelAssemblerCodeFile, "mov ebx,%s", StringToPrint);
        ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].Type = static_cast<VirtualCommandDataType>(1000);
        return;
    }
    else
    if (CommandName == VirtualCommandName::OPR && CommandValue == VirtualCommandOperationType::DIV)
    {
        fprintf(IntelAssemblerCodeFile, "mov ebx,%s", StringToPrint);
        ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].Type = static_cast<VirtualCommandDataType>(1000);
        return;
    }
    else
    if (CommandName == VirtualCommandName::OPR && CommandValue == VirtualCommandOperationType::DIV_ASS)
    {
        fprintf(IntelAssemblerCodeFile, "mov ebx,%s", StringToPrint);
        ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].Type = static_cast<VirtualCommandDataType>(1000);
        return;
    }
    else
    if (CommandName == VirtualCommandName::OPR && CommandValue == VirtualCommandOperationType::MOD)
    {
        fprintf(IntelAssemblerCodeFile, "mov ebx,%s", StringToPrint);
        ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].Type = static_cast<VirtualCommandDataType>(1000);
        return;
    }
    else
    if (CommandName == VirtualCommandName::OPR && CommandValue == VirtualCommandOperationType::MOD_ASS)
    {
        fprintf(IntelAssemblerCodeFile, "mov ebx,%s", StringToPrint);
        ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].Type = static_cast<VirtualCommandDataType>(1000);
        return;
    }
    else
    if (CommandName == VirtualCommandName::OPR && CommandValue == VirtualCommandOperationType::POW)
    {
        fprintf(IntelAssemblerCodeFile, "mov ebx,%s", StringToPrint);
        ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].Type = static_cast<VirtualCommandDataType>(1000);
        return;
    }
    else
    if ((CommandName == VirtualCommandName::OPR && ((static_cast<SignedInt>(CommandValue) < 8 || static_cast<SignedInt>(CommandValue) > 13) && CommandValue != VirtualCommandOperationType::ADD && CommandValue != VirtualCommandOperationType::SUB && CommandValue != VirtualCommandOperationType::SUB_ASS)) || CommandName != VirtualCommandName::OPR)
    {
        if (PushEAX >= DepthForPushPopEAX && ParserGeneratedVirtualCode[VirtualCodeCommandIndex - 1].CommandName != VirtualCommandName::GET)
            fprintf(IntelAssemblerCodeFile, "push eax\n%s", SpacesTabulator);
        fprintf(IntelAssemblerCodeFile, "mov eax,%s", StringToPrint);
        PushEAX++;
        DataTypeNumericCode = ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type;
    }

    if (CommandName == VirtualCommandName::JCON)
        AdditionalPrinting = 1;
    else
        AdditionalPrinting = 0;
}

bool IntelAssemblerCodeGenerator::CheckParameters(UnsignedInt VirtualCodeCommandIndex) const
{
    while (ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName == VirtualCommandName::LOAD && ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].Index > ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Index)
        VirtualCodeCommandIndex++;
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName == VirtualCommandName::CALL || ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName == VirtualCommandName::VIRTCALL)
        return true;
    else
        return false;
}

void IntelAssemblerCodeGenerator::SetFunctionParameters()
{
    for (UnsignedInt ParameterIndex = NumberOfProperFastRegisters - 1; ParameterIndex >= FunctionParameterUseEAXRegisterPos; ParameterIndex--)
        if (ParametersUsageArray[ParameterIndex][InsideFunctionParametersLevel] == 1)
        {
            fprintf(IntelAssemblerCodeFile, "pop %s\n%s", FunctionParameters[ParameterIndex].c_str(), SpacesTabulator);
            ParametersUsageArray[ParameterIndex][InsideFunctionParametersLevel] = 0;
        }
    InsideFunctionParametersLevel--;
}

void IntelAssemblerCodeGenerator::SetFunctionParametersUsageArrayToZero()
{
    for (UnsignedInt e = 0; e <= NumberOfProperFastRegisters - 1; e++)
        for (UnsignedInt f = 0; f <= MaximalNumberOfInsideFunctionsCallForUsageOfRegisters; f++)
            ParametersUsageArray[e][f] = 0;
}

void IntelAssemblerCodeGenerator::AfterCall(UnsignedInt VirtualCodeCommandIndex)
{
    auto NextCommandCode = ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName;
    //te if-y to jesli funkcja to push_eax++ jesli procedura to nie
    if (NextCommandCode == VirtualCommandName::SVV || NextCommandCode == VirtualCommandName::LOAD)
        PushEAX++;
    else
    if ((NextCommandCode == VirtualCommandName::LDC || NextCommandCode == VirtualCommandName::LDV) && ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 2].CommandName == VirtualCommandName::OPR && ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 2].Operation != VirtualCommandOperationType::NOP)
        PushEAX++;
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName == VirtualCommandName::OPR && ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].Operation != VirtualCommandOperationType::NOP && static_cast<UnsignedInt>(ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].Type) == 0)
        PushEAX++;
    else
    if ((NextCommandCode == VirtualCommandName::LDV || NextCommandCode == VirtualCommandName::LDC) && PushEAX >= 1)
        PushEAX++;
    //czy wszystkie warunki potrzebne
    //co gdy funkcja przed opr -2 -niemozliwe
    //co gdy funkcja przed ret = nie zwiekszaj eax
}


void IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForFunctionStackReserveOperation(const UnsignedInt VirtualCodeCommandIndex) //const
{
    const auto SymbolInt = NumericCodeToStringSymbolsMap.find(static_cast<SignedInt>(ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type));
    if (SymbolInt != NumericCodeToStringSymbolsMap.end() && ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].LabelAddress < ConstantForPointerToMultiply)
    {
        UnsignedInt ls = NumberOfSpacesToAlignAssemblerCommandInFile - (strlen(SymbolInt->second.c_str()) + 1);
        if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Level != 0)
        {
            fprintf(IntelAssemblerCodeFile, "%s_%lu_", SymbolInt->second.c_str(), ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Level);
            ls = ls - 2;
        }
        else
            fprintf(IntelAssemblerCodeFile, "%s_", SymbolInt->second.c_str());

        for (UnsignedInt ln = 1; ln <= ls; ln++)
            fprintf(IntelAssemblerCodeFile, " ");
        fprintf(IntelAssemblerCodeFile, "PROC\n");

        PushEAX = 0;
    }
    else
        if (SymbolInt != NumericCodeToStringSymbolsMap.end() && ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].LabelAddress >= ConstantForPointerToMultiply)
            fprintf(IntelAssemblerCodeFile, "; %s_ :\n", SymbolInt->second.c_str());
    if (SymbolInt->second == "main")
        fprintf(IntelAssemblerCodeFile, "%sfinit\n", SpacesTabulator);
}

void IntelAssemblerCodeGenerator::InitRegistersParameters()
{
    FunctionParameters[FunctionParameterUseEAXRegisterPos] = "eax";
    FunctionParameters[FunctionParameterUseEBXRegisterPos] = "ebx";
    FunctionParameters[FunctionParameterUseECXRegisterPos] = "ecx";
    FunctionParameters[FunctionParameterUseEDXRegisterPos] = "edx";
    FunctionParameters[FunctionParameterUseESIRegisterPos] = "esi";
    FunctionParameters[FunctionParameterUseEDIRegisterPos] = "edi";
}

void IntelAssemblerCodeGenerator::PrintExternVariablesDeclarations() const
{
    for (const auto& ExternDataObject : ExternData)
        if (ExternDataObject.NumberOfOverload != 0)
            fprintf(IntelAssemblerCodeFile, "\n%sEXTRN %s_%lu_ : far", SpacesTabulator, ExternDataObject.StringCode.c_str(), ExternDataObject.NumberOfOverload);
        else
            fprintf(IntelAssemblerCodeFile, "\n%sEXTRN %s_ : far", SpacesTabulator, ExternDataObject.StringCode.c_str());
}

void IntelAssemblerCodeGenerator::PrintPublicVariablesDeclarations() const
{
    for (const auto& PublicDataObject : PublicData)
        if (PublicDataObject.NumberOfOverload != 0)
            fprintf(IntelAssemblerCodeFile, "%sPUBLIC %s_%lu_\n", SpacesTabulator, PublicDataObject.StringCode.c_str(), PublicDataObject.NumberOfOverload);
        else
            fprintf(IntelAssemblerCodeFile, "%sPUBLIC %s_\n", SpacesTabulator, PublicDataObject.StringCode.c_str());
}

void IntelAssemblerCodeGenerator::PrintGlobalVariablesDeclarations() const
{
    fprintf(IntelAssemblerCodeFile, "\n");
    for (const auto& GlobalDataObject : GlobalData)
        fprintf(IntelAssemblerCodeFile, "%sPUBLIC _%s\n", SpacesTabulator, GlobalDataObject.Name.c_str());
    fprintf(IntelAssemblerCodeFile, "\n");

    for (const auto& GlobalDataObject : GlobalData)
    {
        UnsignedInt Position;
        const string DatatType[4] = { "db", "dw", "dd", "dq" };
        if (GlobalDataObject.TypeNumericCode == static_cast<UnsignedInt>(TokenSymbol::UnsignedCharSym))
            Position = 0;
        else
        if (GlobalDataObject.TypeNumericCode == static_cast<UnsignedInt>(TokenSymbol::SignedShortIntSym))
            Position = 1;
        else
        if (GlobalDataObject.TypeNumericCode == static_cast<UnsignedInt>(TokenSymbol::SignedIntSym))
            Position = 2;
        else
        if (GlobalDataObject.TypeNumericCode == static_cast<UnsignedInt>(TokenSymbol::DoubleSym))
            Position = 3;
        else
            Position = 2;

        if (GlobalDataObject.DataSize == 0)
            fprintf(IntelAssemblerCodeFile, "%s_%s %s %ld\n", SpacesTabulator, GlobalDataObject.Name.c_str(), DatatType[Position].c_str(), (SignedInt)GlobalDataObject.InitialValue);

        if (GlobalDataObject.DataSize > 0)
            fprintf(IntelAssemblerCodeFile, "%s_%s %s %lu dup(0)\n", SpacesTabulator, GlobalDataObject.Name.c_str(), DatatType[Position].c_str(), GlobalDataObject.DataSize);
    }
}

void IntelAssemblerCodeGenerator::PrintDataSegment() const
{
    fprintf(IntelAssemblerCodeFile, "\n%s.586\n", SpacesTabulator);
    fprintf(IntelAssemblerCodeFile, "\n%s.model  flat,c\n", SpacesTabulator);
    fprintf(IntelAssemblerCodeFile, "\ncode32      SEGMENT PARA PUBLIC USE32\n");

    PrintExternVariablesDeclarations();

    fprintf(IntelAssemblerCodeFile, "\n%sEXTRN delete : far", SpacesTabulator);
    fprintf(IntelAssemblerCodeFile, "\n%sEXTRN new : far\n", SpacesTabulator);
    fprintf(IntelAssemblerCodeFile, "\n");

    PrintPublicVariablesDeclarations();

    PrintGlobalVariablesDeclarations();

    fprintf(IntelAssemblerCodeFile, "%s___class_offset_ptr dd 0\n", SpacesTabulator);
    fprintf(IntelAssemblerCodeFile, "\n      fld_l MACRO number\n%sfld qword ptr number\n%sENDM\n", SpacesTabulator, SpacesTabulator);
    fprintf(IntelAssemblerCodeFile, "      fld_d MACRO number\n%sfld qword ptr number\n%sENDM\n", SpacesTabulator, SpacesTabulator);
    fprintf(IntelAssemblerCodeFile, "      fld_f MACRO number\n%sfld dword ptr number\n%sENDM\n\n", SpacesTabulator, SpacesTabulator);
}

void IntelAssemblerCodeGenerator::PrintProperNumberOfSpacesForOrderOrLabel(const UnsignedInt VirtualCodeCommandIndex) const
{
    fprintf(IntelAssemblerCodeFile, "l%lu : ", ParserGeneratedVirtualCode[VirtualCodeCommandIndex].LabelAddress);
    const UnsignedInt NumberOfSpaces = NumberOfSpacesToAlignAssemblerCommandInFile - to_string(ParserGeneratedVirtualCode[VirtualCodeCommandIndex].LabelAddress).length() - 4;
    for (UnsignedInt SpaceIndex = 1; SpaceIndex <= NumberOfSpaces; SpaceIndex++)
        fprintf(IntelAssemblerCodeFile, " ");
}

bool IntelAssemblerCodeGenerator::GenerateArithmeticalGroup1Operations(const UnsignedInt VirtualCodeCommandIndex)
{
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::ADD)
    {
        AdditionalPrinting1 = 0;
        if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type != static_cast<VirtualCommandDataType>(1000))
        {
            fprintf(IntelAssemblerCodeFile, "pop ebx\n%sadd eax,ebx", SpacesTabulator);
            PushEAX--;
            AdditionalPrinting1 = 0;
        }
        return true;
    }
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::SUB)
    {
        AdditionalPrinting1 = 0;
        if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type != static_cast<VirtualCommandDataType>(1000))
        {
            fprintf(IntelAssemblerCodeFile, "mov ebx,eax\n%spop eax\n%ssub eax,ebx", SpacesTabulator, SpacesTabulator);
            PushEAX--;
            AdditionalPrinting1 = 0;
        }
        return true;
    }
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::SUB_ASS)
    {
        AdditionalPrinting1 = 0;
        if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type != static_cast<VirtualCommandDataType>(1000))
        {
            fprintf(IntelAssemblerCodeFile, "pop ebx\n%ssub eax,ebx", SpacesTabulator);
            PushEAX--;
            AdditionalPrinting1 = 0;
            NegEAX = true;
        }
        return true;
    }
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::MUL)
    {
        if (PushEAX >= DepthForPushPopEAX + 1 && ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type != static_cast<VirtualCommandDataType>(1000))
        {
            fprintf(IntelAssemblerCodeFile, "pop ebx\n%s", SpacesTabulator);
            PushEAX--;
        }
        fprintf(IntelAssemblerCodeFile, "mul ebx");
        AdditionalPrinting1 = 0;
        //oper("mul ebx")
        return true;
    }
    return false;
}

bool IntelAssemblerCodeGenerator::GenerateArithmeticalGroup2Operations(const UnsignedInt VirtualCodeCommandIndex)
{
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::DIV)
    {
        if (PushEAX >= DepthForPushPopEAX + 1 && ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type != static_cast<VirtualCommandDataType>(1000))
        {
            fprintf(IntelAssemblerCodeFile, "mov ebx,eax\n%spop eax\n%s", SpacesTabulator, SpacesTabulator);
            PushEAX--;
        }
        fprintf(IntelAssemblerCodeFile, "xor edx,edx\n%sdiv ebx", SpacesTabulator);
        AdditionalPrinting1 = 0;
        return true;
    }
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::DIV_ASS)
    {
        if (PushEAX >= DepthForPushPopEAX + 1 && ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type != static_cast<VirtualCommandDataType>(1000))
        {
            fprintf(IntelAssemblerCodeFile, "mov ebx,eax\n%spop eax\n%s", SpacesTabulator, SpacesTabulator);
            PushEAX--;
        }
        fprintf(IntelAssemblerCodeFile, "xchg eax,ebx\n%sxor edx,edx\n%sdiv ebx", SpacesTabulator, SpacesTabulator);
        AdditionalPrinting1 = 0;
        return true;
    }
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::MOD)
    {
        if (PushEAX >= DepthForPushPopEAX + 1 && ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type != static_cast<VirtualCommandDataType>(1000))
        {
            fprintf(IntelAssemblerCodeFile, "mov ebx,eax\n%spop eax\n%s", SpacesTabulator, SpacesTabulator);
            PushEAX--;
        }
        fprintf(IntelAssemblerCodeFile, "xor edx,edx\n%sdiv ebx\n%smov eax,edx", SpacesTabulator, SpacesTabulator);
        AdditionalPrinting1 = 0;
        return true;
    }
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::MOD_ASS)
    {
        if (PushEAX >= DepthForPushPopEAX + 1 && ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type != static_cast<VirtualCommandDataType>(1000))
        {
            fprintf(IntelAssemblerCodeFile, "mov ebx,eax\n%spop eax\n%s", SpacesTabulator, SpacesTabulator);
            PushEAX--;
        }
        fprintf(IntelAssemblerCodeFile, "xchg eax,ebx\n%sxor edx,edx\n%sdiv ebx\n%smov eax,edx", SpacesTabulator, SpacesTabulator, SpacesTabulator);
        AdditionalPrinting1 = 0;
        return true;
    }
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::POW)
    {
        if (PushEAX >= DepthForPushPopEAX + 1 && ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type != static_cast<VirtualCommandDataType>(1000))
        {
            fprintf(IntelAssemblerCodeFile, "pop ebx\n%s", SpacesTabulator);
            PushEAX--;
        }
        fprintf(IntelAssemblerCodeFile, "call power ; eax = eax^ebx");
        AdditionalPrinting1 = 0;
        return true;
    }
    return false;
}

void IntelAssemblerCodeGenerator::GenerateConditionOperation(const UnsignedInt VirtualCodeCommandIndex, const char* FirstCondition, const char* SecondCondition)
{
    if (static_cast<SignedInt>(ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].Type) == 0)
        GenerateCondition(VirtualCodeCommandIndex, FirstCondition);
    else
        GenerateCondition(VirtualCodeCommandIndex, SecondCondition);
}

void IntelAssemblerCodeGenerator::GenerateConditionalOperations(const UnsignedInt VirtualCodeCommandIndex)
{
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::EQU)
        GenerateConditionOperation(VirtualCodeCommandIndex, "jne l", "je l");
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::NEQ)
        GenerateConditionOperation(VirtualCodeCommandIndex, "je l", "jne l");
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::JB && DataTypeNumericCode == VirtualCommandDataType::UNSIGNED_INT_TYPE)
        GenerateConditionOperation(VirtualCodeCommandIndex, "jae l", "jb l");
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::JAE && DataTypeNumericCode == VirtualCommandDataType::UNSIGNED_INT_TYPE)
        GenerateConditionOperation(VirtualCodeCommandIndex, "jb l", "jae l");
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::JA && DataTypeNumericCode == VirtualCommandDataType::UNSIGNED_INT_TYPE)
        GenerateConditionOperation(VirtualCodeCommandIndex, "jbe l", "ja l");
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::JBE && DataTypeNumericCode == VirtualCommandDataType::UNSIGNED_INT_TYPE)
        GenerateConditionOperation(VirtualCodeCommandIndex, "ja l", "jbe l");
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::JB && DataTypeNumericCode == VirtualCommandDataType::SIGNED_INT_TYPE)
        GenerateConditionOperation(VirtualCodeCommandIndex, "jge l", "jl l");
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::JAE && DataTypeNumericCode == VirtualCommandDataType::SIGNED_INT_TYPE)
        GenerateConditionOperation(VirtualCodeCommandIndex, "jl l", "jge l");
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::JA && DataTypeNumericCode == VirtualCommandDataType::SIGNED_INT_TYPE)
        GenerateConditionOperation(VirtualCodeCommandIndex, "jle l", "jg l");
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::JBE && DataTypeNumericCode == VirtualCommandDataType::SIGNED_INT_TYPE)
        GenerateConditionOperation(VirtualCodeCommandIndex, "jg l", "jle l");
}

void IntelAssemblerCodeGenerator::GenerateIncrementationOrDecrementationOperations(const UnsignedInt VirtualCodeCommandIndex, const std::string& ofst, const char* Str1, const char* Str2)
{
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Level == LOCAL_LEVEL)
        fprintf(IntelAssemblerCodeFile, "%s dword ptr [ ebp ][ %ld ],1 ; %s", Str1, ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type, Str2);
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Level == GLOBAL_LEVEL)
        fprintf(IntelAssemblerCodeFile, "%s dword ptr %s,1 ; %s ; off=%ld", Str1, ofst.c_str(), Str2, ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type);
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName != VirtualCommandName::LDV && ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName != VirtualCommandName::JCONT && ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName != VirtualCommandName::JBREAK)
        if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName != VirtualCommandName::STACK0)
        {
            if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Level == LOCAL_LEVEL)
                fprintf(IntelAssemblerCodeFile, "\n%smov eax,[ ebp ][ %ld ]", SpacesTabulator, ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type);
            if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Level == GLOBAL_LEVEL)
                fprintf(IntelAssemblerCodeFile, "\n%smov eax,%s ; off=%ld", SpacesTabulator, ofst.c_str(), ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type);
            PushEAX++;
        }
    AdditionalPrinting1 = 0;
}

void IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForEndFunction(const UnsignedInt VirtualCodeCommandIndex)
{
    if (const auto SymbolInt = NumericCodeToStringSymbolsMap.find(static_cast<SignedInt>(ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type)); SymbolInt != NumericCodeToStringSymbolsMap.end())
    {
        fprintf(IntelAssemblerCodeFile, ";\n");
        UnsignedInt ls = NumberOfSpacesToAlignAssemblerCommandInFile - (strlen(SymbolInt->second.c_str()) + 1);
        if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Level != 0)
        {
            fprintf(IntelAssemblerCodeFile, "%s_%lu_", SymbolInt->second.c_str(), ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Level);
            ls = ls - 2;
        }
        else
            fprintf(IntelAssemblerCodeFile, "%s_", SymbolInt->second.c_str());
        for (UnsignedInt ln = 1; ln <= ls; ln++)
            fprintf(IntelAssemblerCodeFile, " ");
        fprintf(IntelAssemblerCodeFile, "ENDP\n", SpacesTabulator);
        AdditionalPrinting1 = 0;
    }
}

bool IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForNegOrNotOperations(const UnsignedInt VirtualCodeCommandIndex)
{
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::MINUS)
    {
        //oper("neg eax")
        fprintf(IntelAssemblerCodeFile, "neg eax");
        AdditionalPrinting1 = 0;
        return true;
    }
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::NOT)
    {
        fprintf(IntelAssemblerCodeFile, "shl eax,31\n%sneg eax\n%sshr eax,31", SpacesTabulator, SpacesTabulator);
        AdditionalPrinting1 = 0;
        return true;
    }
    return false;
}

void IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForCallOrIncOrDecOperations(const char* Str)
{
    fprintf(IntelAssemblerCodeFile, Str);
    AdditionalPrinting1 = 0;
}

void IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForReturnFromFunctionOperation(const UnsignedInt VirtualCodeCommandIndex)
{
    fprintf(IntelAssemblerCodeFile, "pop ebp\n%sadd esp,%ld\n%sret", SpacesTabulator, ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Size, SpacesTabulator);
    AdditionalPrinting1 = 0;
    if (PushEAX == 1)
        PushEAX--;
}

void IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForOperation(const UnsignedInt VirtualCodeCommandIndex, const std::string& ofst)
{
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::LN)
        GenerateIntelAssemblerCodeForCallOrIncOrDecOperations(";call ln ; ln( eax )");
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::EXP)
        GenerateIntelAssemblerCodeForCallOrIncOrDecOperations(";call exp ; exp( eax )");
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::INC && ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Kind == -200)
        GenerateIncrementationOrDecrementationOperations(VirtualCodeCommandIndex, ofst, "add", "inc");
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::DEC && ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Kind == -200)
        GenerateIncrementationOrDecrementationOperations(VirtualCodeCommandIndex, ofst, "sub", "dec");
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::INC && ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Kind == -100)
        GenerateIntelAssemblerCodeForCallOrIncOrDecOperations("inc eax");
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Operation == VirtualCommandOperationType::DEC && ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Kind == -100)
        GenerateIntelAssemblerCodeForCallOrIncOrDecOperations("dec eax");
    else
    if (GenerateArithmeticalGroup1Operations(VirtualCodeCommandIndex) == false)
        if (GenerateArithmeticalGroup2Operations(VirtualCodeCommandIndex) == false)
            if (GenerateIntelAssemblerCodeForNegOrNotOperations(VirtualCodeCommandIndex) == false)
                GenerateConditionalOperations(VirtualCodeCommandIndex);
}

void IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForIncDecWithJump(const UnsignedInt VirtualCodeCommandIndex, const std::string& ofst, UnsignedInt& quest, UnsignedInt& log_bracket)
{
    if (!(ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName == VirtualCommandName::OPR && (ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].Operation == VirtualCommandOperationType::INC || ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].Operation == VirtualCommandOperationType::DEC)))
    {
        if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Kind != -ConstantForSymbolToAdd)
        {
            if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Level == LOCAL_LEVEL)
            {
                CountConditionalJump("[ ebp ][ ", VirtualCodeCommandIndex, IntelAssemblerCodeFile, AdditionalPrinting1);
                log_bracket = 1;
            }
            else
            if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Level == GLOBAL_LEVEL)
            {
                CountConditionalJump(ofst.c_str(), VirtualCodeCommandIndex, IntelAssemblerCodeFile, AdditionalPrinting1);
                fprintf(IntelAssemblerCodeFile, " ; off=");
                log_bracket = 0;
            }
            else
            if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Level == CLASS_LEVEL)
            {
                fprintf(IntelAssemblerCodeFile, "mov ebx,___class_offset_ptr\n%s", SpacesTabulator);
                CountConditionalJump("[ ebx ][ ", VirtualCodeCommandIndex, IntelAssemblerCodeFile, AdditionalPrinting1);
                log_bracket = 1;
            }
        }
        if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Kind == -ConstantForSymbolToAdd)
            CountConditionalJump("offset pstr_", VirtualCodeCommandIndex, IntelAssemblerCodeFile, AdditionalPrinting1);
        quest = 1;
    }
    else
    {
        AdditionalPrinting1 = 0;
        ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].Kind = -200;
        quest = 0;
    }
}

void IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForSavingVariable(const UnsignedInt VirtualCodeCommandIndex, const std::string& ofst)
{
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Level == LOCAL_LEVEL)
        fprintf(IntelAssemblerCodeFile, "mov [ ebp ][ %lu ],eax ", (UnsignedInt)ParserGeneratedVirtualCode[VirtualCodeCommandIndex1].TargetAddress);
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Level == GLOBAL_LEVEL)
        fprintf(IntelAssemblerCodeFile, "mov %s,eax ; off=%lu", ofst.c_str(), (UnsignedInt)ParserGeneratedVirtualCode[VirtualCodeCommandIndex1].TargetAddress);
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Level == CLASS_LEVEL)
    {
        fprintf(IntelAssemblerCodeFile, "mov ebx,___class_offset_ptr\n%s", SpacesTabulator);
        fprintf(IntelAssemblerCodeFile, "mov [ ebx ][ %lu ],eax ", (UnsignedInt)ParserGeneratedVirtualCode[VirtualCodeCommandIndex1].TargetAddress);
    }

    AdditionalPrinting1 = 0;
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName != VirtualCommandName::OPR)
        if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName != VirtualCommandName::SVV) //dodane 3 marca
            if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName != VirtualCommandName::LOAD) //dodane 3 marca
                if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName != VirtualCommandName::VIRTRET) //dodane 3 marca
                    PushEAX--;

    fprintf(IntelAssemblerCodeFile, " ; %ld", ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type);
}

void IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForLoadingAddress(const UnsignedInt VirtualCodeCommandIndex)
{
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Level == LOCAL_LEVEL)
    {
        CountConditionalJump("ebp", VirtualCodeCommandIndex, IntelAssemblerCodeFile, AdditionalPrinting1);
        fprintf(IntelAssemblerCodeFile, "\n%sadd eax,%lu", SpacesTabulator, (UnsignedInt)ParserGeneratedVirtualCode[VirtualCodeCommandIndex].TargetAddress);
    }
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Level == GLOBAL_LEVEL)
    {
        CountConditionalJump(" ", VirtualCodeCommandIndex, IntelAssemblerCodeFile, AdditionalPrinting1);
        fprintf(IntelAssemblerCodeFile, "%lu ; tablica globalna", (UnsignedInt)ParserGeneratedVirtualCode[VirtualCodeCommandIndex].TargetAddress);
    }
    else
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Level == CLASS_LEVEL)
    {
        fprintf(IntelAssemblerCodeFile, "mov ebx,___class_offset_ptr\n%s", SpacesTabulator);
        CountConditionalJump("ebx", VirtualCodeCommandIndex, IntelAssemblerCodeFile, AdditionalPrinting1);
        fprintf(IntelAssemblerCodeFile, "\n%sadd eax,%lu", SpacesTabulator, (UnsignedInt)ParserGeneratedVirtualCode[VirtualCodeCommandIndex].TargetAddress);
    }
    AdditionalPrinting1 = 0;
}

void IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForSavingAddress(const UnsignedInt VirtualCodeCommandIndex)
{
    fprintf(IntelAssemblerCodeFile, "mov [ebx],eax");
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName != VirtualCommandName::OPR) //dodane 6 marca
        if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName != VirtualCommandName::SVV) //dodane 6 marca
            if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName != VirtualCommandName::LOAD) //dodane 6 marca
                if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName != VirtualCommandName::VIRTRET) //dodane 6 marca
                    PushEAX--;
    AdditionalPrinting1 = 0;
}

void IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForLoadingFromAddress(const UnsignedInt VirtualCodeCommandIndex)
{
    fprintf(IntelAssemblerCodeFile, "mov ebx,eax ;for loading address\n%s", SpacesTabulator);
    if (static_cast<SignedInt>(ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type) != 100)
        fprintf(IntelAssemblerCodeFile, "mov eax,[ebx]");
    if (static_cast<SignedInt>(ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type) == 100 && ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName == VirtualCommandName::SVVFPTR)
    {
        fprintf(IntelAssemblerCodeFile, "pop eax ;next=pamadr");
        PushEAX--;
    }
    AdditionalPrinting1 = 0;
}

void IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForLoading(const UnsignedInt VirtualCodeCommandIndex)
{
    AdditionalPrinting1 = 0;
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Index == 1)
    {
        InsideFunctionParametersLevel++;
        if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName != VirtualCommandName::CALL && ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName != VirtualCommandName::VIRTCALL)
            if (CheckParameters(VirtualCodeCommandIndex) == false)
            {
                if (!(ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName == VirtualCommandName::LDV && PushEAX > 0))
                    fprintf(IntelAssemblerCodeFile, "push eax");
                ParametersUsageArray[FunctionParameterUseEAXRegisterPos][InsideFunctionParametersLevel] = 1;
            }
    }
    else
    for (UnsignedInt k = NumberOfProperFastRegisters - 1; k >= 2; k--)
        if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Index == k)
        {
            if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Kind == CONSTANT_TYPE)
            {
                fprintf(IntelAssemblerCodeFile, "mov %s,%ld", FunctionParameters[k].c_str(), (SignedInt)GetProperValueOrTargetAddress(VirtualCodeCommandIndex));
                PushEAX++;
            }
            else
            if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Kind > 0)
            {
                fprintf(IntelAssemblerCodeFile, "mov %s,[ ebp ][ %lu ] ; %ld", FunctionParameters[k].c_str(), (UnsignedInt)ParserGeneratedVirtualCode[VirtualCodeCommandIndex].TargetAddress, ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Kind);
                PushEAX++;
            }
            else
                fprintf(IntelAssemblerCodeFile, "mov %s,eax", FunctionParameters[k].c_str());
            if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName != VirtualCommandName::CALL && ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName != VirtualCommandName::VIRTCALL)
                if (CheckParameters(VirtualCodeCommandIndex) == false)
                {
                    if (!(ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName == VirtualCommandName::LDV && PushEAX > 0))
                        //czy powyzsza linia potrzebna
                        fprintf(IntelAssemblerCodeFile, "\n%spush %s", SpacesTabulator, FunctionParameters[k].c_str());
                    ParametersUsageArray[2][InsideFunctionParametersLevel] = 1;
                }
        }
    PushEAX--;
}

void IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForLoadingPushing(const UnsignedInt VirtualCodeCommandIndex)
{
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Index == 1)
        if (!(ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName == VirtualCommandName::LDV && PushEAX > 0))
            fprintf(IntelAssemblerCodeFile, "push eax ; load");

    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Index > 1)
    {
        if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Kind == CONSTANT_TYPE)
        {
            fprintf(IntelAssemblerCodeFile, "push %lu ; load", (SignedInt)GetProperValueOrTargetAddress(VirtualCodeCommandIndex));
            PushEAX++;
        }
        else
        if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Kind > 0)
        {
            fprintf(IntelAssemblerCodeFile, "push [ ebp ][ %lu ] ; %ld load", (UnsignedInt)ParserGeneratedVirtualCode[VirtualCodeCommandIndex].TargetAddress, ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Kind);
            PushEAX++;
        }
        else
        if (!(ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName == VirtualCommandName::LDV && PushEAX > 0))
            //czy powyzsza linia potrzebna
            fprintf(IntelAssemblerCodeFile, "push eax ; load");
    }
    AdditionalPrinting1 = 0;
    PushEAX--;
}

void IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForGettingMemoryOnStack(const UnsignedInt VirtualCodeCommandIndex)
{
    fprintf(IntelAssemblerCodeFile, "sub esp,%lu\n", ParserGeneratedVirtualCode[VirtualCodeCommandIndex].TargetAddress);
    fprintf(IntelAssemblerCodeFile, "%spush ebp\n%smov ebp,esp", SpacesTabulator, SpacesTabulator);
    AdditionalPrinting1 = 0;
}

void IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForNoRet(const UnsignedInt VirtualCodeCommandIndex)
{
    fprintf(IntelAssemblerCodeFile, "pop ebp\n%sadd esp,%ld\n%s", SpacesTabulator, ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Size, SpacesTabulator);
    if (!(ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].Operation == VirtualCommandOperationType::END_INLINE_INSIDE_FUNCTION))
        fprintf(IntelAssemblerCodeFile, "jmp l");
   else
       AdditionalPrinting1 = 0;

    if (!(PushEAX >= 1 && ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].Operation == VirtualCommandOperationType::END_INLINE_INSIDE_FUNCTION &&
        (ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 2].CommandName == VirtualCommandName::SVV
        || ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 2].CommandName == VirtualCommandName::LOAD
        || ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 2].CommandName == VirtualCommandName::LDV
        || ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 2].CommandName == VirtualCommandName::LDC //problematyczne
        )))
        PushEAX--;
}

void IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForUnconditionalJump(const UnsignedInt VirtualCodeCommandIndex)
{
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].TargetAddress != VirtualCodeCommandIndex + 1)
        fprintf(IntelAssemblerCodeFile, "jmp l");
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].TargetAddress == VirtualCodeCommandIndex + 1)
        AdditionalPrinting1 = 0;
}

void IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForSpecialPopEdx(const UnsignedInt VirtualCodeCommandIndex)
{
    fprintf(IntelAssemblerCodeFile, "pop edx\n%s", SpacesTabulator);
    CountConditionalJump("edx", VirtualCodeCommandIndex, IntelAssemblerCodeFile, AdditionalPrinting1);
    AdditionalPrinting1 = 0;
}

void IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForSpecialPopEdx0(const UnsignedInt VirtualCodeCommandIndex)
{
    fprintf(IntelAssemblerCodeFile, "pop edx");
    AdditionalPrinting1 = 0;
}

void IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForSpecialPopEdxPush(const UnsignedInt VirtualCodeCommandIndex)
{
    fprintf(IntelAssemblerCodeFile, "cmp eax,[esp+4]");
    PushEAX--;
    AdditionalPrinting1 = 0;
}

void IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForSta(const UnsignedInt VirtualCodeCommandIndex)
{
    CountConditionalJump("", VirtualCodeCommandIndex, IntelAssemblerCodeFile, AdditionalPrinting1);
    DataTypeNumericCode = VirtualCommandDataType::SIGNED_INT_TYPE; // stala zawsze UnsignedInt - a trzeba rozopoznac
}

void IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForCallingFunction(const UnsignedInt VirtualCodeCommandIndex)
{
    SetFunctionParameters();
    AfterCall(VirtualCodeCommandIndex);
    if (static_cast<SignedInt>(ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type) > 0)
    {
        auto SymbolInt = NumericCodeToStringSymbolsMap.find(static_cast<SignedInt>(ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type));

        if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Level != 0)
            fprintf(IntelAssemblerCodeFile, "call %s_%lu_ ; call l", SymbolInt->second.c_str(), ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Level);
        else
            fprintf(IntelAssemblerCodeFile, "call %s_ ; call l", SymbolInt->second.c_str());
    }
    if (static_cast<SignedInt>(ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type) == -2000)
    {
        fprintf(IntelAssemblerCodeFile, "call delete");
        AdditionalPrinting1 = 0;
    }
    if (static_cast<SignedInt>(ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type) == -3000)
    {
        fprintf(IntelAssemblerCodeFile, "call new");
        AdditionalPrinting1 = 0;
    }
}

void IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForNoCall(const UnsignedInt VirtualCodeCommandIndex)
{
    SetFunctionParameters();
    fprintf(IntelAssemblerCodeFile, "; call l");
    RegisterStack.push(PushEAX);
    PushEAX = 0;
}

void IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForNull(const UnsignedInt VirtualCodeCommandIndex)
{
    AdditionalPrinting1 = 0;
    if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].CommandName == VirtualCommandName::SVVFPTR)
    {
        PushEAX--;
        fprintf(IntelAssemblerCodeFile, "mov ebx,eax ;from null\n%s", SpacesTabulator);
        fprintf(IntelAssemblerCodeFile, "pop eax");
    }
}

void IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForStack0(const UnsignedInt VirtualCodeCommandIndex)
{
    if (PushEAX == 1)
    {
        fprintf(IntelAssemblerCodeFile, "add esp,4 ; stack_0");
        PushEAX = 0;
    }
    AdditionalPrinting1 = 0;
}

void IntelAssemblerCodeGenerator::GenerateIntelAssemblerCodeForSavePar(const UnsignedInt VirtualCodeCommandIndex)
{
    fprintf(IntelAssemblerCodeFile, "mov [ ebp ][ %lu ],%s ; %ld", ParserGeneratedVirtualCode[VirtualCodeCommandIndex].TargetAddress, FunctionParameters[static_cast<SignedInt>(ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Kind)].c_str(), ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type);
    AdditionalPrinting1 = 0;
}

void IntelAssemblerCodeGenerator::PrintIntelAssemblerCodeToFile()
{
    InitRegistersParameters();
    PushEAX = 0;
    InsideFunctionParametersLevel = 0;

    SetFunctionParametersUsageArrayToZero();

    RememberJumpAddresses();

    UnsignedInt Question;

    IntelAssemblerCodeFile = fopen(NameOfIntelAssemblerOutputFile.c_str(), "w+");

    PrintDataSegment();

    for (UnsignedInt VirtualCodeCommandIndex = 0; VirtualCodeCommandIndex < NumberOfGeneratedVirtualCodeCommands; VirtualCodeCommandIndex++)
    {
        NegEAX = false;
        AdditionalPrinting1 = 1;
        AdditionalPrinting2 = 0;
        UnsignedInt PrintAdditionalBracket = 0;
        AdditionalPrinting = 0;
        VirtualCodeCommandIndex1 = VirtualCodeCommandIndex2 = VirtualCodeCommandIndex;

        if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].CommandName == VirtualCommandName::GET)
            GenerateIntelAssemblerCodeForFunctionStackReserveOperation(VirtualCodeCommandIndex);

        PrintProperNumberOfSpacesForOrderOrLabel(VirtualCodeCommandIndex);

        string StringOffset = "_";

        if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Level == GLOBAL_LEVEL)
            StringOffset += GlobalData[ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Index].Name;

        switch (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].CommandName)
        {
            case VirtualCommandName::GET: GenerateIntelAssemblerCodeForGettingMemoryOnStack(VirtualCodeCommandIndex); break;
            case VirtualCommandName::FREE: GenerateIntelAssemblerCodeForReturnFromFunctionOperation(VirtualCodeCommandIndex); break;
            case VirtualCommandName::LDP: GenerateIntelAssemblerCodeForSavePar(VirtualCodeCommandIndex); break;
            case VirtualCommandName::LDC: GenerateIntelAssemblerCodeForSta(VirtualCodeCommandIndex); break;
            case VirtualCommandName::LDV: GenerateIntelAssemblerCodeForIncDecWithJump(VirtualCodeCommandIndex, StringOffset, Question, PrintAdditionalBracket); break;
            case VirtualCommandName::SVV: GenerateIntelAssemblerCodeForSavingVariable(VirtualCodeCommandIndex, StringOffset); break;
            case VirtualCommandName::LDPTROFV: GenerateIntelAssemblerCodeForLoadingAddress(VirtualCodeCommandIndex); break;
            case VirtualCommandName::SVVFPTR: GenerateIntelAssemblerCodeForSavingAddress(VirtualCodeCommandIndex); break;
            case VirtualCommandName::LDVFPTR: GenerateIntelAssemblerCodeForLoadingFromAddress(VirtualCodeCommandIndex); break;
            case VirtualCommandName::LOAD: GenerateIntelAssemblerCodeForLoading(VirtualCodeCommandIndex); break;
            case VirtualCommandName::LOADPUSH: GenerateIntelAssemblerCodeForLoadingPushing(VirtualCodeCommandIndex); break;
            case VirtualCommandName::OPR: GenerateIntelAssemblerCodeForOperation(VirtualCodeCommandIndex, StringOffset); break;
            case VirtualCommandName::CALL: GenerateIntelAssemblerCodeForCallingFunction(VirtualCodeCommandIndex); break;
            case VirtualCommandName::RET: GenerateIntelAssemblerCodeForEndFunction(VirtualCodeCommandIndex); break;
            case VirtualCommandName::JMP: GenerateIntelAssemblerCodeForUnconditionalJump(VirtualCodeCommandIndex); break;
            case VirtualCommandName::JGOTO: fprintf(IntelAssemblerCodeFile, "jmp l"); break;
            case VirtualCommandName::JCONT: fprintf(IntelAssemblerCodeFile, "jmp l"); break;
            case VirtualCommandName::JBREAK: fprintf(IntelAssemblerCodeFile, "jmp l"); break;
            case VirtualCommandName::FILLOPR: fprintf(IntelAssemblerCodeFile, "fillopr"); AdditionalPrinting1 = 0; break;
            case VirtualCommandName::VIRTCALL: GenerateIntelAssemblerCodeForNoCall(VirtualCodeCommandIndex); break;
            case VirtualCommandName::VIRTRET: GenerateIntelAssemblerCodeForNoRet(VirtualCodeCommandIndex); break;
            case VirtualCommandName::PUSHEDX: fprintf(IntelAssemblerCodeFile, "push eax"); PushEAX--; AdditionalPrinting1 = 0; break;
            case VirtualCommandName::POPEDX: GenerateIntelAssemblerCodeForSpecialPopEdx(VirtualCodeCommandIndex); break;
            case VirtualCommandName::POPEDXPUSH: GenerateIntelAssemblerCodeForSpecialPopEdxPush(VirtualCodeCommandIndex); break;
            case VirtualCommandName::POPEDX0: GenerateIntelAssemblerCodeForSpecialPopEdx0(VirtualCodeCommandIndex); break;
            case VirtualCommandName::STACK0: GenerateIntelAssemblerCodeForStack0(VirtualCodeCommandIndex); break;
            case VirtualCommandName::NUL: GenerateIntelAssemblerCodeForNull(VirtualCodeCommandIndex); break;
            case VirtualCommandName::NOP: fprintf(IntelAssemblerCodeFile, ""); AdditionalPrinting1 = 0; break;
            case VirtualCommandName::NTH: fprintf(IntelAssemblerCodeFile, ""); AdditionalPrinting1 = 0; break;
            default: break;
        }


        //if( push_eax == -1 ) { printf( "PUSH_EAX=-1 %d\n",ParserGeneratedVirtualCode[VirtualCodeCommandIndex].ety ); break; }
        if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].CommandName == VirtualCommandName::STACK0 && PushEAX > 1)
        {
            printf("stack_0 > 1 %lu\n", ParserGeneratedVirtualCode[VirtualCodeCommandIndex].LabelAddress);
            break;
        }

        if (AdditionalPrinting2)
            fprintf(IntelAssemblerCodeFile, "%ld", ParserGeneratedVirtualCode[VirtualCodeCommandIndex2].Type);
        if (AdditionalPrinting1)
            fprintf(IntelAssemblerCodeFile, "%ld", (SignedInt)GetProperValueOrTargetAddress(VirtualCodeCommandIndex1));
        if (PrintAdditionalBracket)
            fprintf(IntelAssemblerCodeFile, " ] ; %ld", ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Type);

        if (AdditionalPrinting)
        {
            fprintf(IntelAssemblerCodeFile, "\n%sor eax,eax\n%s", SpacesTabulator, SpacesTabulator);
            if (static_cast<SignedInt>(ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].Type) == 0)
                GenerateCondition(VirtualCodeCommandIndex, "jz l");
            else
                GenerateCondition(VirtualCodeCommandIndex, "jnz l");

            fprintf(IntelAssemblerCodeFile, "%lu", (UnsignedInt)ParserGeneratedVirtualCode[VirtualCodeCommandIndex + 1].TargetAddress);
            PushEAX--;
        }
        if (ParserGeneratedVirtualCode[VirtualCodeCommandIndex].CommandName == VirtualCommandName::SVV)
            fprintf(IntelAssemblerCodeFile, "; %2ld", ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Kind);
        if (Question && ParserGeneratedVirtualCode[VirtualCodeCommandIndex].CommandName == VirtualCommandName::LDV)
            fprintf(IntelAssemblerCodeFile, "; %2ld", ParserGeneratedVirtualCode[VirtualCodeCommandIndex].Kind);


        fprintf(IntelAssemblerCodeFile, ";\n");
        //fprintf( IntelAssemblerCodeFile," P=%d ;\n",push_eax );
        //fprintf( IntelAssemblerCodeFile," P=%d %d %d;\n",tab[par_eax][inside],tab[par_ebx][inside],inside );

        if (NegEAX)
            fprintf(IntelAssemblerCodeFile, "%sneg eax\n", SpacesTabulator);
    }
    for (UnsignedInt ConstantStringIndex = 0; ConstantStringIndex < ListOfConstantStrings.size(); ConstantStringIndex++)
        fprintf(IntelAssemblerCodeFile, "%spstr_%lu db '%s'\n", SpacesTabulator, ConstantStringIndex, ListOfConstantStrings[ConstantStringIndex].c_str());
    fprintf(IntelAssemblerCodeFile, ";\n");
    fprintf(IntelAssemblerCodeFile, "code32      ENDS\n");
    if (StartAddress > 0)
        fprintf(IntelAssemblerCodeFile, "%sEND main_ ; END l%lu", SpacesTabulator, StartAddress);
    fclose(IntelAssemblerCodeFile);
}
