
#ifndef VIRTUAL_CODE_PRINTER_H
#define VIRTUAL_CODE_PRINTER_H

#include "Types.h"

class VirtualCodePrinter
{
public:
    std::vector<VirtualCodeCommand>& ParserGeneratedVirtualCode;
    UnsignedInt NumberOfGeneratedVirtualCodeCommands;
public:
    void PrintVirtualCodeToFile() const;
public:
    VirtualCodePrinter(std::vector<VirtualCodeCommand>& IntermediateCodeParam, const UnsignedInt NumberOfGeneratedIntermediateCodeCommandsParam) : ParserGeneratedVirtualCode(IntermediateCodeParam), NumberOfGeneratedVirtualCodeCommands(NumberOfGeneratedIntermediateCodeCommandsParam)
    {
    }
};

#endif
