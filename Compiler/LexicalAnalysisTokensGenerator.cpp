
#include <iostream>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <malloc.h>
#include <cmath>
#include <string>
#include <unordered_map>

#include "Types.h"
#include "Constants.h"
#include "LexicalAnalysisTokensGenerator.h"

using namespace std;

void LexicalAnalysisTokensGenerator::EnterTokenSymbol(const string& IdentifierString, UnsignedInt& IdentifierPosition)
{
    IdentifierPosition++;
    const UnsignedInt IdentifierNumericCode = (static_cast<UnsignedInt>(IdentifierString[0]) + static_cast<UnsignedInt>(IdentifierString[1])) * ConstantForSymbolToMultiply + IdentifierPosition;
    StringToNumericCodeSymbolsMap[IdentifierString] = IdentifierNumericCode;
    NumericCodeToStringSymbolsMap[IdentifierNumericCode] = IdentifierString;
}

void LexicalAnalysisTokensGenerator::GetWordTokenChars()
{
    string StrId;
    do
    {
        StrId += static_cast<char>(CurrentChar);
        CurrentChar = fgetc(FileForLexicalAnalysis);
    }
    while (isalnum(static_cast<char>(CurrentChar)) || CurrentChar == '_');

    if (auto KeyWordIter = word.find(StrId); KeyWordIter != word.end())
    {
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol = KeyWordIter->second;
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar = CurrentChar;
    }
    else
    {
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol = TokenSymbol::IdentifierSym;
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar = CurrentChar;

        if (auto SymbolStr = StringToNumericCodeSymbolsMap.find(StrId); SymbolStr == StringToNumericCodeSymbolsMap.end())
        {
            EnterTokenSymbol(StrId, IdentifierPosition);
            GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type = StringToNumericCodeSymbolsMap[StrId];
        }
        else
            GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type = SymbolStr->second;
    }
}

void LexicalAnalysisTokensGenerator::EnterOneToken(const TokenSymbol Symbol1, const TokenSymbol Symbol2)
{
    CurrentChar = fgetc(FileForLexicalAnalysis);
    if (CurrentChar == '=')
    {
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol = Symbol1;
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar = CurrentChar;
        CurrentChar = fgetc(FileForLexicalAnalysis);
    }
    else
    {
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol = Symbol2;
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar = CurrentChar;
    }
}

void LexicalAnalysisTokensGenerator::EnterTwoTokens(const char CodeChar, const TokenSymbol Symbol1, const TokenSymbol Symbol2, const TokenSymbol Symbol3)
{
    CurrentChar = fgetc(FileForLexicalAnalysis);
    if (CurrentChar == '=')
    {
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol = Symbol1;
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar = CurrentChar;
        CurrentChar = fgetc(FileForLexicalAnalysis);
    }
    else
    if (CurrentChar == CodeChar)
    {
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol = Symbol2;
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar = CurrentChar;
        CurrentChar = fgetc(FileForLexicalAnalysis);
    }
    else
    {
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol = Symbol3;
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar = CurrentChar;
    }
}

RealType LexicalAnalysisTokensGenerator::TakeExponent()
{
    RealType exp = 0;
    UnsignedInt sign = 1;

    if (CurrentChar == '+')
        CurrentChar = fgetc(FileForLexicalAnalysis);
    else
    if (CurrentChar == '-')
    {
        CurrentChar = fgetc(FileForLexicalAnalysis);
        sign = -1;
    }

    if (isdigit(static_cast<char>(CurrentChar)))
    {
        do
        {
            exp = 10 * exp + CurrentChar - '0';
            CurrentChar = fgetc(FileForLexicalAnalysis);
        }
        while (isdigit(static_cast<char>(CurrentChar)));
    }
    else
        printf("error in take_exponent");

    return pow(10, sign * exp);
}


void LexicalAnalysisTokensGenerator::GetNumberTokenChars()
{
    RealType pom = 10;
    UnsignedInt k = 0;
    RealType num = 0;
    while (isdigit(static_cast<char>(CurrentChar)))
    {
        num = 10 * num + (CurrentChar - '0');
        k++;
        CurrentChar = fgetc(FileForLexicalAnalysis);
    }
    if (k > MaximalNumberOfCharactersInNumber)
        printf("too many characters in declared number");

    if (CurrentChar == '.')
        CurrentChar = fgetc(FileForLexicalAnalysis);
    while (isdigit(static_cast<char>(CurrentChar)))
    {
        num = num + (CurrentChar - '0') / pom;
        pom *= 10;
        CurrentChar = fgetc(FileForLexicalAnalysis);
    }
    if (CurrentChar == 'E' || CurrentChar == 'e')
    {
        CurrentChar = fgetc(FileForLexicalAnalysis);
        num *= TakeExponent();
    }
    GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol = TokenSymbol::NumberSym;
    GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type = num;
    GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar = CurrentChar;
}

void LexicalAnalysisTokensGenerator::GetMinusChar()
{
    CurrentChar = fgetc(FileForLexicalAnalysis);
    if (CurrentChar == '=')
    {
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol = TokenSymbol::AssignMinusSym;
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar = CurrentChar;
        CurrentChar = fgetc(FileForLexicalAnalysis);
    }
    else if (CurrentChar == '-')
    {
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol = TokenSymbol::DecSym;
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar = CurrentChar;
        CurrentChar = fgetc(FileForLexicalAnalysis);
    }
    else if (CurrentChar == '>')
    {
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol = TokenSymbol::ArrowSym;
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar = CurrentChar;
        CurrentChar = fgetc(FileForLexicalAnalysis);
    }
    else
    {
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol = TokenSymbol::MinusSym;
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar = CurrentChar;
    }
}

void LexicalAnalysisTokensGenerator::GetGreaterThanEqualOrShlTokenChars()
{
    CurrentChar = fgetc(FileForLexicalAnalysis);
    if (CurrentChar == '=')
    {
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol = TokenSymbol::GreaterOrEqualSym;
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar = CurrentChar;
        CurrentChar = fgetc(FileForLexicalAnalysis);
    }
    else if (CurrentChar == '>')
    {
        CurrentChar = fgetc(FileForLexicalAnalysis);
        if (CurrentChar == '=')
        {
            GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol = TokenSymbol::AssignShrSym;
            GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar = CurrentChar;
            CurrentChar = fgetc(FileForLexicalAnalysis);
        }
        else
        {
            GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol = TokenSymbol::ShrSym;
            GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar = CurrentChar;
        }
    }
    else
    {
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol = TokenSymbol::GreaterThan;
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar = CurrentChar;
    }
}

void LexicalAnalysisTokensGenerator::GetLowerThanEqualOrShrTokenChars()
{
    CurrentChar = fgetc(FileForLexicalAnalysis);
    if (CurrentChar == '=')
    {
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol = TokenSymbol::LowerOrEqualSym;
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar = CurrentChar;
        CurrentChar = fgetc(FileForLexicalAnalysis);
    }
    else if (CurrentChar == '<')
    {
        CurrentChar = fgetc(FileForLexicalAnalysis);
        if (CurrentChar == '=')
        {
            GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol = TokenSymbol::AssignShlSym;
            GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar = CurrentChar;
            CurrentChar = fgetc(FileForLexicalAnalysis);
        }
        else
        {
            GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol = TokenSymbol::ShlSym;
            GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar = CurrentChar;
        }
    }
    else
    {
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol = TokenSymbol::LowerThan;
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar = CurrentChar;
    }
}

void LexicalAnalysisTokensGenerator::GetEndOfStringChar(UnsignedInt PrevString)
{
    CurrentChar = fgetc(FileForLexicalAnalysis);
    PrevString = CurrentChar;
    GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol = TokenSymbol::NumberSym;
    GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type = CurrentChar;
    GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar = CurrentChar;
    CurrentChar = fgetc(FileForLexicalAnalysis);
    if (PrevString == 92 && CurrentChar == '0')
    {
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type = 0;
        CurrentChar = fgetc(FileForLexicalAnalysis);
    }
    if (CurrentChar == 39)
        CurrentChar = fgetc(FileForLexicalAnalysis);
    else
        printf("unmatched '");
}

void LexicalAnalysisTokensGenerator::GetChar34(bool& open_str)
{
    CurrentChar = fgetc(FileForLexicalAnalysis);
    if (!open_str)
    {
        UnsignedInt pos_str = 0;
        open_str = true;
        string pstr;
        strstart:
            while (CurrentChar != 34)
            {
                pstr += (char)CurrentChar;
                CurrentChar = fgetc(FileForLexicalAnalysis);
            }
        CurrentChar = fgetc(FileForLexicalAnalysis);
        if (CurrentChar == 34)
        {
            CurrentChar = fgetc(FileForLexicalAnalysis);
            goto strstart;
        }
        open_str = false;
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol = TokenSymbol::StrPtrSym;
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Type = ListOfConstantStrings.size();
        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar = CurrentChar;
        ListOfConstantStrings.emplace_back(pstr);
    }

}

void LexicalAnalysisTokensGenerator::GetNewSingleTokenChar(const TokenSymbol TokenSymbolElement)
{
    GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].CurrentChar = CurrentChar;
    CurrentChar = fgetc(FileForLexicalAnalysis);
    GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol = TokenSymbolElement;
}

void LexicalAnalysisTokensGenerator::GetSpaceCharsAndEndOfLineChars(UnsignedInt& num_line)
{
    while ((CurrentChar == ' ') || (CurrentChar == '\n') || (CurrentChar == '\t'))
    {
        if (CurrentChar == '\n')
            num_line++;
        CurrentChar = fgetc(FileForLexicalAnalysis);
    }
}

void LexicalAnalysisTokensGenerator::GenerateLexicalAnalysisTokens(char* namefile)
{
    UnsignedInt PrevChar;
    bool OpenString = false;
    GeneratedLexicalAnalysisTokenPosition = 0;
    IdentifierPosition = 0;

    FileForLexicalAnalysis = fopen(namefile, "r");
    CurrentChar = fgetc(FileForLexicalAnalysis);
    UnsignedInt LineNumber = 1;

    while (CurrentChar != EOF)
    {
        GetSpaceCharsAndEndOfLineChars(LineNumber);

        if (isalpha(static_cast<char>(CurrentChar)) || CurrentChar == '_')
            GetWordTokenChars();
        else
        if (isdigit(static_cast<char>(CurrentChar)))
            GetNumberTokenChars();
        else
        switch (CurrentChar)
        {
            case '=': EnterOneToken(TokenSymbol::EqualSym, TokenSymbol::AssignSym); break;
            case '*': EnterOneToken(TokenSymbol::AssignMulSym, TokenSymbol::MulSym); break;
            case '/': EnterOneToken(TokenSymbol::AssignDivSym, TokenSymbol::DivSymbol); break;
            case '+': EnterTwoTokens('+', TokenSymbol::AssignPlusSym, TokenSymbol::IncSym, TokenSymbol::PlusSym); break;
            case '-': GetMinusChar(); break;
            case '%': EnterOneToken(TokenSymbol::AssignModuloSym, TokenSymbol::ModSym); break;
            case '!': EnterOneToken(TokenSymbol::NegativeSym, TokenSymbol::NotSym); break;
            case '&': EnterTwoTokens('&', TokenSymbol::AssignBitAndSym, TokenSymbol::AndSym, TokenSymbol::BitAndSym); break;
            case '^': EnterOneToken(TokenSymbol::AssignBitXorSym, TokenSymbol::BitXorSym); break;
            case '|': EnterTwoTokens('|', TokenSymbol::AssignBitOrSym, TokenSymbol::OrSym, TokenSymbol::BitOrSym); break;
            case '>': GetGreaterThanEqualOrShlTokenChars(); break;
            case '<': GetLowerThanEqualOrShrTokenChars(); break;
            case 39: GetEndOfStringChar(PrevChar); break;
            case 34: GetChar34(OpenString); break;
            case '?': GetNewSingleTokenChar(TokenSymbol::QuestionSym); break;
            case ':': GetNewSingleTokenChar(TokenSymbol::ColonSym); break;
            case '@': GetNewSingleTokenChar(TokenSymbol::PowerSym); break;
            case '.': GetNewSingleTokenChar(TokenSymbol::PointSym); break;
            case '~': GetNewSingleTokenChar(TokenSymbol::TildeSym); break;
            case '[': GetNewSingleTokenChar(TokenSymbol::LeftSquareSym); break;
            case ']': GetNewSingleTokenChar(TokenSymbol::RightSquareSym); break;
            case ',': GetNewSingleTokenChar(TokenSymbol::CommaSym); break;
            case ';': GetNewSingleTokenChar(TokenSymbol::SemicolonSym); break;
            case '{': GetNewSingleTokenChar(TokenSymbol::LeftCurlySym); break;
            case '}': GetNewSingleTokenChar(TokenSymbol::RightCurlySym); break;
            case '(': GetNewSingleTokenChar(TokenSymbol::LeftParSym); break;
            case ')': GetNewSingleTokenChar(TokenSymbol::RightParSym); break;

            default: CurrentChar = fgetc(FileForLexicalAnalysis); break;
        }

        GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].LineInFile = LineNumber;

        if (static_cast<UnsignedInt>(GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol) != 0)
            GeneratedLexicalAnalysisTokenPosition++;
    }

    GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenPosition].Symbol = TokenSymbol::NoTokenSym;

    for (UnsignedInt GeneratedLexicalAnalysisTokenIndex = 0; GeneratedLexicalAnalysisTokenIndex < GeneratedLexicalAnalysisTokenPosition; GeneratedLexicalAnalysisTokenIndex++)
        printf("LEXICAL ANALYSIS TOKEN = %lu %lu %ld %Lf\n", GeneratedLexicalAnalysisTokenIndex, GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenIndex].LineInFile, GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenIndex].Symbol, GeneratedLexicalAnalysisTokens[GeneratedLexicalAnalysisTokenIndex].Type);

    printf("POSITION = %lu\n", GeneratedLexicalAnalysisTokenPosition);

    fclose(FileForLexicalAnalysis);
}
