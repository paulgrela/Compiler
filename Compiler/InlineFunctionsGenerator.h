
#ifndef INLINE_FUNCTIONS_GENRATOR_H
#define INLINE_FUNCTIONS_GENRATOR_H

#include <vector>

#include "Types.h"

class InlineFunctionsGenerator
{
public:
    static void MakeInline(UnsignedInt NumberOfInlineOperations, UnsignedInt& NumberOfGeneratedVirtualCodeCommands, std::vector<VirtualCodeCommand>& ParserGeneratedVirtualCode);
    static void MakeInline(std::vector<VirtualCodeCommand>& ParserGeneratedVirtualCode, const std::vector<VirtualCodeCommand>& ParserGeneratedVirtualCodeTemporary, UnsignedInt& NumberOfGeneratedVirtualCodeCommands, UnsignedInt& Label);
};

#endif