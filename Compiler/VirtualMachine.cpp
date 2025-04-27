
#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>
#include <functional>
#include <cmath>

#include "Types.h"

#include "VirtualMachine.h"

using namespace std;

struct StackDataType
{
    VirtualCommandDataType Type;
    union
    {
        UnsignedInt UnsignedLongIntType;
        SignedInt SignedLongIntType;
        long double LongDoubleType;
    };
    StackDataType(const VirtualCommandDataType TypeParam, const long double ValueParam) : Type(TypeParam), LongDoubleType(ValueParam)
    {
    }
    StackDataType(const VirtualCommandDataType TypeParam, const SignedInt ValueParam) : Type(TypeParam), SignedLongIntType(ValueParam)
    {
    }
    StackDataType(const VirtualCommandDataType TypeParam, const UnsignedInt ValueParam) : Type(TypeParam), UnsignedLongIntType(ValueParam)
    {
    }
};

class VirtualMachine
{
private:
    std::stack<StackDataType> VirtualMachineStack;
    std::unordered_map<UnsignedInt, RealType> VirtualMachineMemory;
private:
    std::vector<VirtualCodeCommand> ProgramCodeCommandsList;
    UnsignedInt RunningVirtualCommandIndex = 0;
    bool ProgramRunning = true;
private:
    template <class T>
    T ExecuteArithmeticOperation(const VirtualCommandOperationType& OperationType, const T R1, const T R2)
    {
        T Result = 0;
        switch (OperationType)
        {
            case VirtualCommandOperationType::ADD: Result = R1 + R2; break;
            case VirtualCommandOperationType::SUB: Result = R1 - R2; break;
            case VirtualCommandOperationType::MUL: Result = R1 * R2; break;
            case VirtualCommandOperationType::DIV: Result = R1 / R2; break;
            case VirtualCommandOperationType::NEG: Result = -R2; break;
            case VirtualCommandOperationType::MOD: Result = fmod(R1, R2); break;
            case VirtualCommandOperationType::EQU: Result = (R1 == R2) ? 1 : 0; break;
            case VirtualCommandOperationType::NEQ: Result = (R1 != R2) ? 1 : 0; break;
            case VirtualCommandOperationType::LT: Result = (R1 < R2) ? 1 : 0; break;
            case VirtualCommandOperationType::GT: Result = (R1 > R2) ? 1 : 0; break;
            case VirtualCommandOperationType::LE: Result = (R1 <= R2) ? 1 : 0; break;
            case VirtualCommandOperationType::GE: Result = (R1 >= R2) ? 1 : 0; break;
            default: break;
        }
        return Result;
    }
private:
    void PerformOperation(const VirtualCommandOperationType& OperationType)
    {
        const auto R2 = VirtualMachineStack.top();

        VirtualMachineStack.pop();

        const auto R1 = (OperationType != VirtualCommandOperationType::NEG) ? VirtualMachineStack.top() : StackDataType(VirtualMachineStack.top().Type, static_cast<RealType>(0.0));

        if (OperationType != VirtualCommandOperationType::NEG)
            VirtualMachineStack.pop();

        if (R1.Type == VirtualCommandDataType::FLOAT_TYPE || R1.Type == VirtualCommandDataType::DOUBLE_TYPE || R1.Type == VirtualCommandDataType::LONG_DOUBLE_TYPE || R1.Type == VirtualCommandDataType::EXTENDED_TYPE)
            VirtualMachineStack.push(StackDataType(R1.Type, ExecuteArithmeticOperation<RealType>(OperationType, R1.LongDoubleType, R2.LongDoubleType)));
        else
        if (R1.Type == VirtualCommandDataType::SIGNED_CHAR_TYPE || R1.Type == VirtualCommandDataType::SIGNED_SHORT_TYPE || R1.Type == VirtualCommandDataType::SIGNED_INT_TYPE || R1.Type == VirtualCommandDataType::SIGNED_LONG_INT_TYPE)
             VirtualMachineStack.push(StackDataType(R1.Type, ExecuteArithmeticOperation<SignedInt>(OperationType, R1.SignedLongIntType, R2.SignedLongIntType)));
        else
        if (R1.Type == VirtualCommandDataType::UNSIGNED_CHAR_TYPE || R1.Type == VirtualCommandDataType::UNSIGNED_SHORT_TYPE || R1.Type == VirtualCommandDataType::UNSIGNED_INT_TYPE || R1.Type == VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE)
            VirtualMachineStack.push(StackDataType(R1.Type, ExecuteArithmeticOperation<UnsignedInt>(OperationType, R1.UnsignedLongIntType, R2.UnsignedLongIntType)));
    }

    void ExecuteVirtualCommand(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        switch (VirtualCodeCommandToExecute.CommandName)
        {
            case VirtualCommandName::LDC: LoadConstantToStack(VirtualCodeCommandToExecute); break;
            case VirtualCommandName::LDV: LoadVariableToStack(VirtualCodeCommandToExecute); break;
            case VirtualCommandName::SVV: SaveValueFromStackToVariable(VirtualCodeCommandToExecute); break;
            case VirtualCommandName::OPR: PerformOperation(VirtualCodeCommandToExecute.Operation); break;
            case VirtualCommandName::CALL: Call(VirtualCodeCommandToExecute); break;
            case VirtualCommandName::RET: Return(VirtualCodeCommandToExecute); break;
            case VirtualCommandName::JMP: Jump(VirtualCodeCommandToExecute); break;
            case VirtualCommandName::JCON: JumpConditional(VirtualCodeCommandToExecute); break;
            case VirtualCommandName::END: ProgramRunning = false; break;
            case VirtualCommandName::PRINT: PrintMemory(); break;

            default: throw runtime_error("Unknown instruction.");
        }
    }
private:

    void LoadConstantToStack(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::FLOAT_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::DOUBLE_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::LONG_DOUBLE_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::EXTENDED_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, VirtualCodeCommandToExecute.Value));
        else
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::SIGNED_CHAR_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::SIGNED_SHORT_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::SIGNED_INT_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::SIGNED_LONG_INT_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<SignedInt>(VirtualCodeCommandToExecute.Value)));
        else
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::UNSIGNED_CHAR_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::UNSIGNED_SHORT_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::UNSIGNED_INT_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<UnsignedInt>(VirtualCodeCommandToExecute.Value)));
    }

    void LoadVariableToStack(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::FLOAT_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::DOUBLE_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::LONG_DOUBLE_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::EXTENDED_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<RealType>(VirtualMachineMemory[VirtualCodeCommandToExecute.TargetAddress])));
        else
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::SIGNED_CHAR_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::SIGNED_SHORT_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::SIGNED_INT_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::SIGNED_LONG_INT_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<SignedInt>(VirtualMachineMemory[VirtualCodeCommandToExecute.TargetAddress])));
        else
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::UNSIGNED_CHAR_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::UNSIGNED_SHORT_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::UNSIGNED_INT_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<UnsignedInt>(VirtualMachineMemory[VirtualCodeCommandToExecute.TargetAddress])));

    }

    void SaveValueFromStackToVariable(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        if (VirtualMachineStack.top().Type == VirtualCommandDataType::FLOAT_TYPE || VirtualMachineStack.top().Type == VirtualCommandDataType::DOUBLE_TYPE || VirtualMachineStack.top().Type == VirtualCommandDataType::LONG_DOUBLE_TYPE || VirtualMachineStack.top().Type == VirtualCommandDataType::EXTENDED_TYPE)
            VirtualMachineMemory[VirtualCodeCommandToExecute.TargetAddress] = VirtualMachineStack.top().LongDoubleType;
        else
        if (VirtualMachineStack.top().Type == VirtualCommandDataType::SIGNED_CHAR_TYPE || VirtualMachineStack.top().Type == VirtualCommandDataType::SIGNED_SHORT_TYPE || VirtualMachineStack.top().Type == VirtualCommandDataType::SIGNED_INT_TYPE || VirtualMachineStack.top().Type == VirtualCommandDataType::SIGNED_LONG_INT_TYPE)
            VirtualMachineMemory[VirtualCodeCommandToExecute.TargetAddress] = VirtualMachineStack.top().SignedLongIntType;
        else
        if (VirtualMachineStack.top().Type == VirtualCommandDataType::UNSIGNED_CHAR_TYPE || VirtualMachineStack.top().Type == VirtualCommandDataType::UNSIGNED_SHORT_TYPE || VirtualMachineStack.top().Type == VirtualCommandDataType::UNSIGNED_INT_TYPE || VirtualMachineStack.top().Type == VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE)
            VirtualMachineMemory[VirtualCodeCommandToExecute.TargetAddress] = VirtualMachineStack.top().UnsignedLongIntType;
        VirtualMachineStack.pop();
    }

    void Call(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        VirtualMachineStack.push(StackDataType(VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE, RunningVirtualCommandIndex));
        RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress;
    }

    void Return(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        RunningVirtualCommandIndex = VirtualMachineStack.top().UnsignedLongIntType;
        VirtualMachineStack.pop();
    }

    void JumpConditional(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        const auto R1 = VirtualMachineStack.top();
        if (R1.Type == VirtualCommandDataType::FLOAT_TYPE || R1.Type == VirtualCommandDataType::DOUBLE_TYPE || R1.Type == VirtualCommandDataType::LONG_DOUBLE_TYPE || R1.Type == VirtualCommandDataType::EXTENDED_TYPE)
            if (R1.LongDoubleType == 0)
                RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress;
        if (R1.Type == VirtualCommandDataType::SIGNED_CHAR_TYPE || R1.Type == VirtualCommandDataType::SIGNED_SHORT_TYPE || R1.Type == VirtualCommandDataType::SIGNED_INT_TYPE || R1.Type == VirtualCommandDataType::SIGNED_LONG_INT_TYPE)
            if (R1.SignedLongIntType == 0)
                RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress;
        if (R1.Type == VirtualCommandDataType::UNSIGNED_CHAR_TYPE || R1.Type == VirtualCommandDataType::UNSIGNED_SHORT_TYPE || R1.Type == VirtualCommandDataType::UNSIGNED_INT_TYPE || R1.Type == VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE)
            if (R1.UnsignedLongIntType == 0)
                RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress;
        VirtualMachineStack.pop();
    }

    void Jump(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress;
    }

public:
    void LoadProgram(const std::vector<VirtualCodeCommand>& ProgramCode)
    {
        ProgramCodeCommandsList = ProgramCode;
        RunningVirtualCommandIndex = 0;
        ProgramRunning = true;
    }

    void RunProgram()
    {
        while (ProgramRunning && RunningVirtualCommandIndex < ProgramCodeCommandsList.size())
        {
            ExecuteVirtualCommand(ProgramCodeCommandsList[RunningVirtualCommandIndex]);
            RunningVirtualCommandIndex++;
        }
    }

    void PrintMemory() const
    {
        for (const auto& [addr, value] : VirtualMachineMemory)
            cout << "Memory[" << addr << "] = " << value << endl;
    }

    void PrintStack() const
    {
        auto TemporaryVirtualMachineStack = VirtualMachineStack;
        cout << "Stack (top to bottom): ";
        while (!TemporaryVirtualMachineStack.empty())
        {
            cout << static_cast<SignedInt>(TemporaryVirtualMachineStack.top().Type) << " " << TemporaryVirtualMachineStack.top().LongDoubleType << " " << TemporaryVirtualMachineStack.top().SignedLongIntType << " " << TemporaryVirtualMachineStack.top().UnsignedLongIntType << " " << endl;
            TemporaryVirtualMachineStack.pop();
        }
        cout << "END STACK" << endl;
    }
};

std::vector<VirtualCodeCommand> CreateSampleProgram()
{
    return
    {
        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 5, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::SVV, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, 0, 0, 0, 0, 15 },

        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 10, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 10, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 20, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::OPR, VirtualCommandOperationType::ADD, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::OPR, VirtualCommandOperationType::MUL, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::SVV, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, 0, 0, 0, 0, 20 },

        { VirtualCommandName::PRINT, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, 0, 0, 0, 0, 0 },

        { VirtualCommandName::LDV, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, 0, 0, 0, 0, 15 },
        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 1, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::OPR, VirtualCommandOperationType::ADD, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::SVV, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, 0, 0, 0, 0, 15 },

        { VirtualCommandName::LDV, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, 0, 0, 0, 0, 15 },
        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 10, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::OPR, VirtualCommandOperationType::EQU, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::JCON, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, 0, 0, 0, 0, 7 },
    };
}

int TestVirtualMachine()
{
    VirtualMachine VirtualMachinObject;

    const auto Program = CreateSampleProgram();
    VirtualMachinObject.LoadProgram(Program);

    VirtualMachinObject.RunProgram();

    VirtualMachinObject.PrintStack();
    VirtualMachinObject.PrintMemory();

    return 0;
}

template RealType VirtualMachine::ExecuteArithmeticOperation<RealType>(const VirtualCommandOperationType& OperationType, RealType R1, RealType R2);
template SignedInt VirtualMachine::ExecuteArithmeticOperation<SignedInt>(const VirtualCommandOperationType& OperationType, SignedInt R1, SignedInt R2);
template UnsignedInt VirtualMachine::ExecuteArithmeticOperation<UnsignedInt>(const VirtualCommandOperationType& OperationType, UnsignedInt R1, UnsignedInt R2);