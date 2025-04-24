
#ifndef LEXICAL_ANALYSIS_TOKENS_GENERATOR_H
#define LEXICAL_ANALYSIS_TOKENS_GENERATOR_H

#include <string>
#include <vector>
#include <unordered_map>

#include "Types.h"

class LexicalAnalysisTokensGenerator
{
private :
    UnsignedInt CurrentChar{};
    FILE* FileForLexicalAnalysis{};
    UnsignedInt GeneratedLexicalAnalysisTokenPosition{};
private:
    std::unordered_map<std::string, UnsignedInt> StringToNumericCodeSymbolsMap{};
public:
    std::unordered_map<UnsignedInt, std::string> NumericCodeToStringSymbolsMap{};
public:
    std::vector<std::string> ListOfConstantStrings{};
private:
    UnsignedInt IdentifierPosition{};
public:
    std::vector<LexicalAnalysisTokenSymbol> GeneratedLexicalAnalysisTokens;
public:
    LexicalAnalysisTokensGenerator() : GeneratedLexicalAnalysisTokens(MaximalNumberOfGeneratedTokenSymbols)
    {
    }
public:
    RealType TakeExponent();
    void GenerateLexicalAnalysisTokens(char* namefile);
    void GetChar34(bool& open_str);
    void GetSpaceCharsAndEndOfLineChars(UnsignedInt& num_line);
    void GetEndOfStringChar(UnsignedInt PrevString);
    void GetLowerThanEqualOrShrTokenChars();
    void GetGreaterThanEqualOrShlTokenChars();
    void GetMinusChar();
    void GetNumberTokenChars();
    void GetWordTokenChars();
    void GetNewSingleTokenChar(TokenSymbol TokenSymbolElement);
    void EnterTokenSymbol(const std::string& IdentifierString, UnsignedInt& IdentifierPosition);
    void EnterOneToken(TokenSymbol Symbol1, TokenSymbol Symbol2);
    void EnterTwoTokens(char CodeChar, TokenSymbol Symbol1, TokenSymbol Symbol2, TokenSymbol Symbol3);
};

#endif
