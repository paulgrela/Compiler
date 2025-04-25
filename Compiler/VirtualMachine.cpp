
#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>
#include <functional>
#include <cmath>

#include "Types.h"

#include "VirtualMachine.h"

class VirtualMachine
{
private:
    std::stack<RealType> VirtualMachineStack;
    std::unordered_map<UnsignedInt, RealType> VirtualMachineMemory;
private:
    std::vector<VirtualCodeCommand> ProgramCodeCommandsList;
    UnsignedInt RunningVirtualCommandIndex = 0;
    bool ProgramRunning = true;
private:
    void PerformOperation(const VirtualCommandOperationType& OperationType)
    {
        const auto R2 = VirtualMachineStack.top();

        VirtualMachineStack.pop();

        const auto R1 = (OperationType != VirtualCommandOperationType::NEG) ? VirtualMachineStack.top() : 0;

        if (OperationType != VirtualCommandOperationType::NEG)
            VirtualMachineStack.pop();

        RealType Result = 0;
        switch (OperationType)
        {
            case VirtualCommandOperationType::ADD: Result = R1 + R2; break;
            case VirtualCommandOperationType::SUB: Result = R1 - R2; break;
            case VirtualCommandOperationType::MUL: Result = R1 * R2; break;
            case VirtualCommandOperationType::DIV: Result = R1 / R2; break;
            case VirtualCommandOperationType::NEG: Result = -R2; break;
            case VirtualCommandOperationType::MOD: Result = std::fmod(R1, R2); break;
            case VirtualCommandOperationType::EQU: Result = (R1 == R2) ? 1 : 0; break;
            case VirtualCommandOperationType::NEQ: Result = (R1 != R2) ? 1 : 0; break;
            case VirtualCommandOperationType::LT: Result = (R1 < R2) ? 1 : 0; break;
            case VirtualCommandOperationType::GT: Result = (R1 > R2) ? 1 : 0; break;
            case VirtualCommandOperationType::LE: Result = (R1 <= R2) ? 1 : 0; break;
            case VirtualCommandOperationType::GE: Result = (R1 >= R2) ? 1 : 0; break;
            default: break;
        }
        VirtualMachineStack.push(Result);
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

            default: throw std::runtime_error("Unknown instruction.");
        }
    }
private:

    void LoadConstantToStack(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        VirtualMachineStack.push(VirtualCodeCommandToExecute.Value);
    }

    void LoadVariableToStack(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        VirtualMachineStack.push(VirtualMachineMemory[VirtualCodeCommandToExecute.TargetAddress]);
    }

    void SaveValueFromStackToVariable(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        VirtualMachineMemory[VirtualCodeCommandToExecute.TargetAddress] = VirtualMachineStack.top();
        VirtualMachineStack.pop();
    }

    void Call(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        VirtualMachineStack.push(RunningVirtualCommandIndex);
        RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress;
    }

    void Return(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        RunningVirtualCommandIndex = static_cast<UnsignedInt>(VirtualMachineStack.top());
        VirtualMachineStack.pop();
    }

    void JumpConditional(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        if (VirtualMachineStack.top() == 0)
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
            std::cout << "Memory[" << addr << "] = " << value << std::endl;
    }

    void PrintStack() const
    {
        auto TemporaryVirtualMachineStack = VirtualMachineStack;
        std::cout << "Stack (top to bottom): ";
        while (!TemporaryVirtualMachineStack.empty())
        {
            std::cout << TemporaryVirtualMachineStack.top() << " ";
            TemporaryVirtualMachineStack.pop();
        }
        std::cout << std::endl;
    }
};

std::vector<VirtualCodeCommand> CreateSampleProgram()
{
    return
    {
        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_TYPE, 5, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::SVV, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_TYPE, 0, 0, 0, 0, 0, 0, 15 },

        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_TYPE, 10, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_TYPE, 10, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_TYPE, 20, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::OPR, VirtualCommandOperationType::ADD, VirtualCommandDataType::SIGNED_TYPE, 0, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::OPR, VirtualCommandOperationType::MUL, VirtualCommandDataType::SIGNED_TYPE, 0, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::SVV, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_TYPE, 0, 0, 0, 0, 0, 0, 20 },

        { VirtualCommandName::PRINT, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_TYPE, 0, 0, 0, 0, 0, 0, 0 },

        { VirtualCommandName::LDV, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_TYPE, 0, 0, 0, 0, 0, 0, 15 },
        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_TYPE, 1, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::OPR, VirtualCommandOperationType::ADD, VirtualCommandDataType::SIGNED_TYPE, 0, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::SVV, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_TYPE, 0, 0, 0, 0, 0, 0, 15 },

        { VirtualCommandName::LDV, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_TYPE, 0, 0, 0, 0, 0, 0, 15 },
        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_TYPE, 10, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::OPR, VirtualCommandOperationType::EQU, VirtualCommandDataType::SIGNED_TYPE, 0, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::JCON, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_TYPE, 0, 0, 0, 0, 0, 0, 7 },
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
