
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

    LexicalAnalysisTokensGenerator LexicalAnalysisTokensGeneratorObject;
    LexicalAnalysisTokensGeneratorObject.GenerateLexicalAnalysisTokens(argv[4]);

    ParserToVirtualCodeGenerator ParserToVirtualCodeGeneratorObject(LexicalAnalysisTokensGeneratorObject.GeneratedLexicalAnalysisTokens, LexicalAnalysisTokensGeneratorObject.NumericCodeToStringSymbolsMap);
    ParserToVirtualCodeGeneratorObject.ParserToVirtualCodeGenerationUnit();

    printf("StartAddressOfProgram = %lu", ParserToVirtualCodeGeneratorObject.StartAddressOfProgram);

    if (std::string(argv[1]) == "-o")
        InlineFunctionsGenerator::MakeInline(3, ParserToVirtualCodeGeneratorObject.NumberOfGeneratedVirtualCodeCommands, ParserToVirtualCodeGeneratorObject.GeneratedVirtualCode);

    if (std::string(argv[2]) == "-v")
    {
        VirtualCodePrinter VirtualCodePrinterObject(std::string(argv[4]) + ".OUT", ParserToVirtualCodeGeneratorObject.GeneratedVirtualCode, ParserToVirtualCodeGeneratorObject.NumberOfGeneratedVirtualCodeCommands);
        VirtualCodePrinterObject.PrintVirtualCodeToFile();
    }

    if (std::string(argv[3]) == "-a")
    {
        IntelAssemblerCodeGenerator IntelAssemblerCodeGeneratorObject(std::string(argv[4]) + ".OSM", ParserToVirtualCodeGeneratorObject.GeneratedVirtualCode, ParserToVirtualCodeGeneratorObject.NumberOfGeneratedVirtualCodeCommands, ParserToVirtualCodeGeneratorObject.StartAddressOfProgram, LexicalAnalysisTokensGeneratorObject.NumericCodeToStringSymbolsMap, LexicalAnalysisTokensGeneratorObject.ListOfConstantStrings, ParserToVirtualCodeGeneratorObject.ExternData, ParserToVirtualCodeGeneratorObject.PublicData, ParserToVirtualCodeGeneratorObject.GlobalData);
        IntelAssemblerCodeGeneratorObject.PrintIntelAssemblerCodeToFile();
    }

    if (std::string(argv[3]) == "-e")
        ExecuteProgramOnVirtualMachine(ParserToVirtualCodeGeneratorObject.GeneratedVirtualCode, ParserToVirtualCodeGeneratorObject.StartAddressOfProgram);

    //TestVirtualMachine();

    return 0;
}
