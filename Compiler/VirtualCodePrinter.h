
#ifndef VIRTUAL_CODE_PRINTER_H
#define VIRTUAL_CODE_PRINTER_H

#include "Types.h"

class VirtualCodePrinter
{
private:
    std::string NameOfVirtualCodeOutputFile{};
public:
    std::vector<VirtualCodeCommand>& ParserGeneratedVirtualCode;
    UnsignedInt NumberOfGeneratedVirtualCodeCommands;
public:
    void PrintVirtualCodeToFile() const;
    void PrintVirtualCodeCommandsNamesToFile(FILE* VirtualCodeFile, UnsignedInt VirtualCodeOrderIndex) const;
    void PrintVirtualCodeCommandsNamesExtendedToFile(FILE* VirtualCodeFile, UnsignedInt VirtualCodeOrderIndex) const;
public:
    VirtualCodePrinter(std::string NameOfVirtualCodeOutputFileParam, std::vector<VirtualCodeCommand>& IntermediateCodeParam, const UnsignedInt NumberOfGeneratedIntermediateCodeCommandsParam) : NameOfVirtualCodeOutputFile(std::move(NameOfVirtualCodeOutputFileParam)), ParserGeneratedVirtualCode(IntermediateCodeParam), NumberOfGeneratedVirtualCodeCommands(NumberOfGeneratedIntermediateCodeCommandsParam)
    {
    }
};

#endif
