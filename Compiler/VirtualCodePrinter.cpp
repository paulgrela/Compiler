
#include "Types.h"
#include "VirtualCodePrinter.h"

void VirtualCodePrinter::PrintVirtualCodeCommandsNamesToFile(FILE* VirtualCodeFile, const UnsignedInt VirtualCodeOrderIndex) const
{
    switch (ParserGeneratedVirtualCode[VirtualCodeOrderIndex].CommandName)
    {
        case VirtualCommandName::GET: fprintf(VirtualCodeFile, "GET "); break;
        case VirtualCommandName::LDP: fprintf(VirtualCodeFile, "LDP "); break;
        case VirtualCommandName::LDC: fprintf(VirtualCodeFile, "LDC "); break;
        case VirtualCommandName::LDV: fprintf(VirtualCodeFile, "LDV "); break;
        case VirtualCommandName::SVV: fprintf(VirtualCodeFile, "SVV "); break;
        case VirtualCommandName::LDPTROFV: fprintf(VirtualCodeFile, "LDPTROFV"); break;
        case VirtualCommandName::SVVFPTR: fprintf(VirtualCodeFile, "SVVFPTR"); break;
        case VirtualCommandName::LDVFPTR: fprintf(VirtualCodeFile, "LDVFPTR"); break;
        case VirtualCommandName::LOAD: fprintf(VirtualCodeFile, "LOAD "); break;
        case VirtualCommandName::LOADPUSH: fprintf(VirtualCodeFile, "LOADPUSH "); break;
        case VirtualCommandName::OPR: fprintf(VirtualCodeFile, "OPR "); break;
        case VirtualCommandName::CALL: fprintf(VirtualCodeFile, "CALL "); break;
        case VirtualCommandName::JMP: fprintf(VirtualCodeFile, "JMP "); break;
        case VirtualCommandName::JCON: fprintf(VirtualCodeFile, "JCON "); break;
        case VirtualCommandName::JGOTO: fprintf(VirtualCodeFile, "JGOTO "); break;
        case VirtualCommandName::JCONT: fprintf(VirtualCodeFile, "JCONT "); break;
        case VirtualCommandName::JBREAK: fprintf(VirtualCodeFile, "JBREAK "); break;
        case VirtualCommandName::VIRTCALL: fprintf(VirtualCodeFile, "VIRTCALL "); break;
        case VirtualCommandName::VIRTRET: fprintf(VirtualCodeFile, "VIRTRET "); break;
        case VirtualCommandName::FILLOPR: fprintf(VirtualCodeFile, "FILLOPR "); break;
        case VirtualCommandName::PUSHEDX: fprintf(VirtualCodeFile, "PUSHEDX "); break;
        case VirtualCommandName::POPEDX: fprintf(VirtualCodeFile, "POPEDX "); break;
        case VirtualCommandName::POPEDXPUSH: fprintf(VirtualCodeFile, "POPEDXPUSH "); break;
        case VirtualCommandName::POPEDX0: fprintf(VirtualCodeFile, "POPEDX0 "); break;
        case VirtualCommandName::STACK0: fprintf(VirtualCodeFile, "STACK0 "); break;
        case VirtualCommandName::NUL: fprintf(VirtualCodeFile, "NUL"); break;
        case VirtualCommandName::NOP: fprintf(VirtualCodeFile, "NOP"); break;
        case VirtualCommandName::NTH: fprintf(VirtualCodeFile, "NTH"); break;
        default: break;
    }
}

void VirtualCodePrinter::PrintVirtualCodeCommandsNamesExtendedToFile(FILE* VirtualCodeFile, const UnsignedInt VirtualCodeOrderIndex) const
{
    switch (ParserGeneratedVirtualCode[VirtualCodeOrderIndex].CommandName)
    {
        case VirtualCommandName::GET: fprintf(VirtualCodeFile, "; GetMemoryOnStackForParameters"); break;
        case VirtualCommandName::LDP: fprintf(VirtualCodeFile, "; LoadFunctionParameterToStack"); break;
        case VirtualCommandName::LDC: fprintf(VirtualCodeFile, "; LoadConstantValueToStack"); break;
        case VirtualCommandName::LDV: fprintf(VirtualCodeFile, "; LoadVariableToStack"); break;
        case VirtualCommandName::SVV: fprintf(VirtualCodeFile, "; SaveValueFromStackToVariable"); break;
        case VirtualCommandName::LDPTROFV: fprintf(VirtualCodeFile, "; LoadPointerOfVariableToStack"); break;
        case VirtualCommandName::SVVFPTR: fprintf(VirtualCodeFile, "; SaveValueFromStackToVariableFromPointer"); break;
        case VirtualCommandName::LDVFPTR: fprintf(VirtualCodeFile, "; LoadVariableFromPointerToStack"); break;
        case VirtualCommandName::LOAD: fprintf(VirtualCodeFile, "; LOAD"); break;
        case VirtualCommandName::LOADPUSH: fprintf(VirtualCodeFile, "; LOADPUSH"); break;
        case VirtualCommandName::OPR: fprintf(VirtualCodeFile, "; OperationOnTheTopOfStack "); break;
        case VirtualCommandName::CALL: fprintf(VirtualCodeFile, "; CallFunction"); break;
        case VirtualCommandName::JMP: fprintf(VirtualCodeFile, "; JumpUnconditional"); break;
        case VirtualCommandName::JCON: fprintf(VirtualCodeFile, "; JumpConditional"); break;
        case VirtualCommandName::JGOTO: fprintf(VirtualCodeFile, "; JumpGoto"); break;
        case VirtualCommandName::JCONT: fprintf(VirtualCodeFile, "; JumpContinue"); break;
        case VirtualCommandName::JBREAK: fprintf(VirtualCodeFile, "; JumpBreak"); break;
        case VirtualCommandName::VIRTCALL: fprintf(VirtualCodeFile, "; VirtualCallFunction"); break;
        case VirtualCommandName::VIRTRET: fprintf(VirtualCodeFile, "; VirtualReturnFromFunction "); break;
        case VirtualCommandName::FILLOPR: fprintf(VirtualCodeFile, "; FILLOPR"); break;
        case VirtualCommandName::PUSHEDX: fprintf(VirtualCodeFile, "; PUSHEDX"); break;
        case VirtualCommandName::POPEDX: fprintf(VirtualCodeFile, "; POPEDX"); break;
        case VirtualCommandName::POPEDXPUSH: fprintf(VirtualCodeFile, "; POPEDXPUSH"); break;
        case VirtualCommandName::POPEDX0: fprintf(VirtualCodeFile, "; POPEDX0"); break;
        case VirtualCommandName::STACK0: fprintf(VirtualCodeFile, "; STACK ZERO"); break;
        case VirtualCommandName::NUL: fprintf(VirtualCodeFile, "; NUL"); break;
        case VirtualCommandName::NOP: fprintf(VirtualCodeFile, "; NoOperation"); break;
        case VirtualCommandName::NTH: fprintf(VirtualCodeFile, "; Nothing"); break;
        default: break;
    }
}

void VirtualCodePrinter::PrintVirtualCodeToFile() const
{
    FILE* VirtualCodeFile = fopen(NameOfVirtualCodeOutputFile.c_str(), "w+");

    for (UnsignedInt VirtualCodeOrderIndex = 0; VirtualCodeOrderIndex < NumberOfGeneratedVirtualCodeCommands; VirtualCodeOrderIndex++)
    {
        printf("LRI %lu %lu\n", NumberOfGeneratedVirtualCodeCommands, VirtualCodeOrderIndex);
        fprintf(VirtualCodeFile, "%3lu : ", ParserGeneratedVirtualCode[VirtualCodeOrderIndex].LabelAddress);

        PrintVirtualCodeCommandsNamesToFile(VirtualCodeFile, VirtualCodeOrderIndex);

        fprintf(VirtualCodeFile, "%5ld  ", ParserGeneratedVirtualCode[VirtualCodeOrderIndex].Type);

        if (ParserGeneratedVirtualCode[VirtualCodeOrderIndex].CommandName != VirtualCommandName::OPR)
        {
            if (ParserGeneratedVirtualCode[VirtualCodeOrderIndex].CommandName == VirtualCommandName::NTH ||
                ParserGeneratedVirtualCode[VirtualCodeOrderIndex].CommandName == VirtualCommandName::GET ||
                ParserGeneratedVirtualCode[VirtualCodeOrderIndex].CommandName == VirtualCommandName::LDP ||
                ParserGeneratedVirtualCode[VirtualCodeOrderIndex].CommandName == VirtualCommandName::LDV ||
                ParserGeneratedVirtualCode[VirtualCodeOrderIndex].CommandName == VirtualCommandName::SVV ||
                ParserGeneratedVirtualCode[VirtualCodeOrderIndex].CommandName == VirtualCommandName::LDPTROFV ||
                ParserGeneratedVirtualCode[VirtualCodeOrderIndex].CommandName == VirtualCommandName::LDVFPTR ||
                ParserGeneratedVirtualCode[VirtualCodeOrderIndex].CommandName == VirtualCommandName::SVVFPTR ||
                ParserGeneratedVirtualCode[VirtualCodeOrderIndex].CommandName == VirtualCommandName::LOAD ||
                ParserGeneratedVirtualCode[VirtualCodeOrderIndex].CommandName == VirtualCommandName::LOADPUSH ||
                ParserGeneratedVirtualCode[VirtualCodeOrderIndex].CommandName == VirtualCommandName::VIRTCALL ||
                ParserGeneratedVirtualCode[VirtualCodeOrderIndex].CommandName == VirtualCommandName::VIRTRET ||
                ParserGeneratedVirtualCode[VirtualCodeOrderIndex].CommandName == VirtualCommandName::JMP ||
                ParserGeneratedVirtualCode[VirtualCodeOrderIndex].CommandName == VirtualCommandName::JCON ||
                ParserGeneratedVirtualCode[VirtualCodeOrderIndex].CommandName == VirtualCommandName::JCONT ||
                ParserGeneratedVirtualCode[VirtualCodeOrderIndex].CommandName == VirtualCommandName::JBREAK ||
                ParserGeneratedVirtualCode[VirtualCodeOrderIndex].CommandName == VirtualCommandName::JGOTO ||
                (ParserGeneratedVirtualCode[VirtualCodeOrderIndex].CommandName == VirtualCommandName::CALL && ParserGeneratedVirtualCode[VirtualCodeOrderIndex].TargetAddress != UINT64_MAX))
                fprintf(VirtualCodeFile, "%5Lf", static_cast<RealType>(ParserGeneratedVirtualCode[VirtualCodeOrderIndex].TargetAddress));
            else
            if (ParserGeneratedVirtualCode[VirtualCodeOrderIndex].CommandName == VirtualCommandName::CALL && ParserGeneratedVirtualCode[VirtualCodeOrderIndex].TargetAddress == UINT64_MAX)
            {
                const RealType R = -9999.0f;
                fprintf(VirtualCodeFile, "%5Lf", R);
            }
            else
                fprintf(VirtualCodeFile, "%5Lf", ParserGeneratedVirtualCode[VirtualCodeOrderIndex].Value);
        }
        else
            fprintf(VirtualCodeFile, "%5Lf", static_cast<long double>(ParserGeneratedVirtualCode[VirtualCodeOrderIndex].Operation));


        if (ParserGeneratedVirtualCode[VirtualCodeOrderIndex].CommandName == VirtualCommandName::SVV)
            fprintf(VirtualCodeFile, "  %2ld", ParserGeneratedVirtualCode[VirtualCodeOrderIndex].Kind);

        if (ParserGeneratedVirtualCode[VirtualCodeOrderIndex].CommandName == VirtualCommandName::LDV)
            fprintf(VirtualCodeFile, "  %2ld", ParserGeneratedVirtualCode[VirtualCodeOrderIndex].Kind);

        PrintVirtualCodeCommandsNamesExtendedToFile(VirtualCodeFile, VirtualCodeOrderIndex);

        fprintf(VirtualCodeFile, "\n");
    }

    fclose(VirtualCodeFile);
}
