
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "Types.h"

constexpr UnsignedInt MaximalNumberOfCharactersInNumber = 14;

constexpr UnsignedInt ConstantSymbolForLabelInlineToMultiply = 1E+6;
constexpr UnsignedInt ConstantForPointerToMultiply = 1E+6;
constexpr UnsignedInt ConstantForSymbolToAdd = 1E+6;
constexpr UnsignedInt ConstantForSymbolToMultiply = 1E+4;
constexpr UnsignedInt ConstantBiggerForSymbolToAdd = 1E+8;

constexpr UnsignedInt MaximalNumberOfGeneratedTokenSymbols = 30000;
constexpr UnsignedInt MaximalNumberOfGeneratedVirtualCodeCommands = 5000;

constexpr UnsignedInt MaximalNumberOfPossiblesAssignmentsInExpression = 20;
constexpr UnsignedInt MaximalNumberOfInlineCommands = 70;
constexpr UnsignedInt MaximalNumberOfInsideFunctionsCallForUsageOfRegisters = 30;

constexpr SignedInt CONSTANT_TYPE = -11;

constexpr UnsignedInt CLASS_TYPE = 5000;
constexpr UnsignedInt LOCAL_TYPE = 6000;
constexpr UnsignedInt GLOBAL_TYPE = 7000;

constexpr UnsignedInt PUBLIC_TYPE = 8000;
constexpr UnsignedInt PRIVATE_TYPE = 9000;
constexpr UnsignedInt PROTECTED_TYPE = 10000;

constexpr UnsignedInt NumberOfSpacesToAlignAssemblerCommandInFile = 12;

constexpr SignedInt DepthForPushPopEAX = 1;

constexpr UnsignedInt NumberOfProperFastRegisters = 7;

constexpr UnsignedInt FunctionParameterUseEAXRegisterPos = 1;
constexpr UnsignedInt FunctionParameterUseEDXRegisterPos = 2;
constexpr UnsignedInt FunctionParameterUseEBXRegisterPos = 3;
constexpr UnsignedInt FunctionParameterUseECXRegisterPos = 4;
constexpr UnsignedInt FunctionParameterUseESIRegisterPos = 5;
constexpr UnsignedInt FunctionParameterUseEDIRegisterPos = 6;

#endif
