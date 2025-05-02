
#include <cstdio>
#include <cstring>

#include "Types.h"
#include "Constants.h"
#include "VirtualMachine.h"
#include "LexicalAnalysisTokensGenerator.h"
#include "ParserToVirtualCodeGenerator.h"
#include "VirtualCodePrinter.h"
#include "IntelAssemblerCodeGenerator.h"
#include "InlineFunctionsGenerator.h"

void HelpInformation(UnsignedInt argc)
{
    if (argc == 1)
    {
        printf("SUPER C++ Compiler\n");
        printf("Machine Assembler Code Generation\n");
    }
}

int main(int argc, char* argv[])
{
    HelpInformation(argc);

    char num_file = 2;
    if (argv[1][0] != '-')
        num_file = 1;

    LexicalAnalysisTokensGenerator LexicalAnalysisTokensGeneratorObject;
    LexicalAnalysisTokensGeneratorObject.GenerateLexicalAnalysisTokens(argv[num_file]);

    ParserToVirtualCodeGenerator ParserToVirtualCodeGeneratorObject(LexicalAnalysisTokensGeneratorObject.GeneratedLexicalAnalysisTokens, LexicalAnalysisTokensGeneratorObject.NumericCodeToStringSymbolsMap);
    ParserToVirtualCodeGeneratorObject.ParserToVirtualCodeGenerationUnit();

    if (!strcmp("-o", argv[1]))
        InlineFunctionsGenerator::MakeInline(3, ParserToVirtualCodeGeneratorObject.NumberOfGeneratedVirtualCodeCommands, ParserToVirtualCodeGeneratorObject.GeneratedVirtualCode);

    VirtualCodePrinter VirtualCodePrinterObject(std::string(argv[num_file]) + ".OUT", ParserToVirtualCodeGeneratorObject.GeneratedVirtualCode, ParserToVirtualCodeGeneratorObject.NumberOfGeneratedVirtualCodeCommands);
    VirtualCodePrinterObject.PrintVirtualCodeToFile();

    IntelAssemblerCodeGenerator IntelAssemblerCodeGeneratorObject(std::string(argv[num_file]) + ".OSM", ParserToVirtualCodeGeneratorObject.GeneratedVirtualCode, ParserToVirtualCodeGeneratorObject.NumberOfGeneratedVirtualCodeCommands, ParserToVirtualCodeGeneratorObject.StartAddressOfProgram, LexicalAnalysisTokensGeneratorObject.NumericCodeToStringSymbolsMap, LexicalAnalysisTokensGeneratorObject.ListOfConstantStrings, ParserToVirtualCodeGeneratorObject.ExternData, ParserToVirtualCodeGeneratorObject.PublicData, ParserToVirtualCodeGeneratorObject.GlobalData);
    IntelAssemblerCodeGeneratorObject.PrintIntelAssemblerCodeToFile();

    //TestVirtualMachine();
    ExecuteProgramOnVirtualMachine(ParserToVirtualCodeGeneratorObject.GeneratedVirtualCode);

    return 0;
}
