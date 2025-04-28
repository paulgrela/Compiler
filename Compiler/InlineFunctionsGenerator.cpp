
#include <cstdio>

#include "Types.h"
#include "Constants.h"
#include "InlineFunctionsGenerator.h"

void InlineFunctionsGenerator::MakeInline(const UnsignedInt NumberOfInlineOperations, UnsignedInt& NumberOfGeneratedVirtualCodeCommands, std::vector<VirtualCodeCommand>& ParserGeneratedVirtualCode)
{
    const std::vector<VirtualCodeCommand> ParserGeneratedVirtualCodeTemporary = ParserGeneratedVirtualCode;

    UnsignedInt LabelLevel = 0;
    for (UnsignedInt InlineOperationCounter = 1; InlineOperationCounter <= NumberOfInlineOperations; InlineOperationCounter++)
    {
        printf("Inlining %lu\n", InlineOperationCounter);
        MakeInline(ParserGeneratedVirtualCode, ParserGeneratedVirtualCodeTemporary, NumberOfGeneratedVirtualCodeCommands, LabelLevel);
    }
}

void InlineFunctionsGenerator::MakeInline(std::vector<VirtualCodeCommand>& ParserGeneratedVirtualCode, const std::vector<VirtualCodeCommand>& ParserGeneratedVirtualCodeTemporary, UnsignedInt& NumberOfGeneratedVirtualCodeCommands, UnsignedInt& Label)
{
    UnsignedInt LastFunction = 0;

    for (UnsignedInt GeneratedVirtualCodeCommandIndex = 0; GeneratedVirtualCodeCommandIndex < NumberOfGeneratedVirtualCodeCommands; GeneratedVirtualCodeCommandIndex++)
    {
        if (ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].CommandName == VirtualCommandName::OPR && ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].Operation == VirtualCommandOperationType::RET)
            LastFunction = ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex + 1].LabelAddress;

        LabelLoop:

        if (ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].CommandName == VirtualCommandName::CALL && ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].Value > 0 && ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].Kind == 1)
        {
            if (ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].Value == LastFunction)
            {
                printf("recurrence in line l%lu for function l%lu\n", ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].LabelAddress, static_cast<UnsignedInt>(ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].Value));
                ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].Kind = 0;
                goto LabelLoop;
            }

            UnsignedInt ParserGeneratedVirtualCodeIndex1 = 0;

            while (ParserGeneratedVirtualCode[ParserGeneratedVirtualCodeIndex1].LabelAddress != static_cast<UnsignedInt>(ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].Value))
                ParserGeneratedVirtualCodeIndex1++;

            UnsignedInt ParserGeneratedVirtualCodeIndex2 = ParserGeneratedVirtualCodeIndex1;

            while (!(ParserGeneratedVirtualCode[ParserGeneratedVirtualCodeIndex2].CommandName == VirtualCommandName::OPR && ParserGeneratedVirtualCode[ParserGeneratedVirtualCodeIndex2].Operation == VirtualCommandOperationType::RET))
                ParserGeneratedVirtualCodeIndex2++;
            ParserGeneratedVirtualCodeIndex2++; // przekopiuj opr -1

            UnsignedInt Difference = ParserGeneratedVirtualCodeIndex2 - ParserGeneratedVirtualCodeIndex1;

            if (Difference < MaximalNumberOfInlineCommands)
            {
                ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].CommandName = VirtualCommandName::VIRTCALL;

                Label++;
                std::array<VirtualCodeCommand, MaximalNumberOfInlineCommands + 1> TemporaryFunctionVirtualCode{};
                printf("in line %lu put inline function %lu\n", ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].LabelAddress, ParserGeneratedVirtualCode[ParserGeneratedVirtualCodeIndex1].LabelAddress);

                UnsignedInt l = 0;
                auto w = static_cast<UnsignedInt>(ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].Value);
                while (!(ParserGeneratedVirtualCodeTemporary[w].CommandName == VirtualCommandName::OPR && ParserGeneratedVirtualCodeTemporary[w].Operation == VirtualCommandOperationType::RET))
                    w++;
                for (auto k = static_cast<UnsignedInt>(ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].Value); k <= w; k++, l++)
                    TemporaryFunctionVirtualCode[l] = ParserGeneratedVirtualCodeTemporary[k];

                Difference = l;
                UnsignedInt MaxNumberOfGeneratedVirtualCodeCommands = l;
                l = NumberOfGeneratedVirtualCodeCommands + Difference;

                UnsignedInt GeneratedVirtualCodeCommandIndex1;
                for (GeneratedVirtualCodeCommandIndex1 = NumberOfGeneratedVirtualCodeCommands; GeneratedVirtualCodeCommandIndex1 >= GeneratedVirtualCodeCommandIndex + 1; GeneratedVirtualCodeCommandIndex1--, l--)
                    ParserGeneratedVirtualCode[l] = ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex1];

                for (UnsignedInt FunctionVirtualCodeCommandIndex = 0; FunctionVirtualCodeCommandIndex <= MaxNumberOfGeneratedVirtualCodeCommands; FunctionVirtualCodeCommandIndex++)
                    if (TemporaryFunctionVirtualCode[FunctionVirtualCodeCommandIndex].CommandName == VirtualCommandName::JCON ||
                        TemporaryFunctionVirtualCode[FunctionVirtualCodeCommandIndex].CommandName == VirtualCommandName::JMP ||
                        TemporaryFunctionVirtualCode[FunctionVirtualCodeCommandIndex].CommandName == VirtualCommandName::JGOTO ||
                        TemporaryFunctionVirtualCode[FunctionVirtualCodeCommandIndex].CommandName == VirtualCommandName::JCONT ||
                        TemporaryFunctionVirtualCode[FunctionVirtualCodeCommandIndex].CommandName == VirtualCommandName::JBREAK)
                        TemporaryFunctionVirtualCode[FunctionVirtualCodeCommandIndex].Value = TemporaryFunctionVirtualCode[FunctionVirtualCodeCommandIndex].Value + Label * ConstantSymbolForLabelInlineToMultiply;

                for (UnsignedInt FunctionIntermediateCodeCommandIndex = 0; FunctionIntermediateCodeCommandIndex <= MaxNumberOfGeneratedVirtualCodeCommands; FunctionIntermediateCodeCommandIndex++)
                    if (TemporaryFunctionVirtualCode[FunctionIntermediateCodeCommandIndex].CommandName == VirtualCommandName::OPR && TemporaryFunctionVirtualCode[FunctionIntermediateCodeCommandIndex].Operation == VirtualCommandOperationType::FREE)
                    {
                        TemporaryFunctionVirtualCode[FunctionIntermediateCodeCommandIndex].CommandName = VirtualCommandName::VIRTRET;
                        TemporaryFunctionVirtualCode[FunctionIntermediateCodeCommandIndex].Value = ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex].LabelAddress + 1;
                    }

                for (UnsignedInt IntermediateCodeCommandIndexL1 = 0, VirtualCodeCommandIndexL2 = GeneratedVirtualCodeCommandIndex + 1; VirtualCodeCommandIndexL2 <= GeneratedVirtualCodeCommandIndex + MaxNumberOfGeneratedVirtualCodeCommands; VirtualCodeCommandIndexL2++, IntermediateCodeCommandIndexL1++)
                {
                    ParserGeneratedVirtualCode[VirtualCodeCommandIndexL2] = TemporaryFunctionVirtualCode[IntermediateCodeCommandIndexL1];
                    ParserGeneratedVirtualCode[VirtualCodeCommandIndexL2].LabelAddress += Label * ConstantSymbolForLabelInlineToMultiply;
                }

                ParserGeneratedVirtualCode[GeneratedVirtualCodeCommandIndex1 - 1].Operation = VirtualCommandOperationType::END_INLINE_INSIDE_FUNCTION; //opr -1 => opr -2

                NumberOfGeneratedVirtualCodeCommands = NumberOfGeneratedVirtualCodeCommands + Difference;

                if (NumberOfGeneratedVirtualCodeCommands > MaximalNumberOfGeneratedVirtualCodeCommands)
                {
                    printf("error - too large code file\n");
                    break;
                }

                GeneratedVirtualCodeCommandIndex += Difference;
            }
        }
    }
}
