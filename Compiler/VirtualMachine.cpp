
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
    std::vector<VirtualCodeCommand> program;
    std::stack<double> stack;
    std::unordered_map<int, double> memory;
    int pc = 0;
    bool running = true;

    void performOperation(VirtualCommandOperationType op)
    {
        double b = stack.top();
        stack.pop();
        double a = (op != VirtualCommandOperationType::NEG) ? stack.top() : 0;

        if (op != VirtualCommandOperationType::NEG)
            stack.pop();

        double result = 0;
        switch (op)
        {
            case VirtualCommandOperationType::ADD: result = a + b; break;
            case VirtualCommandOperationType::SUB: result = a - b; break;
            case VirtualCommandOperationType::MUL: result = a * b; break;
            case VirtualCommandOperationType::DIV: result = a / b; break;
            case VirtualCommandOperationType::MOD: result = std::fmod(a, b); break;
            case VirtualCommandOperationType::NEG: result = -b; break;
            case VirtualCommandOperationType::EQU: result = (a == b) ? 1 : 0; break;
            case VirtualCommandOperationType::NEQ: result = (a != b) ? 1 : 0; break;
            case VirtualCommandOperationType::LT: result = (a < b) ? 1 : 0; break;
            case VirtualCommandOperationType::GT: result = (a > b) ? 1 : 0; break;
            case VirtualCommandOperationType::LE: result = (a <= b) ? 1 : 0; break;
            case VirtualCommandOperationType::GE: result = (a >= b) ? 1 : 0; break;
        }
        stack.push(result);
    }

    void executeInstruction(const VirtualCodeCommand& instr)
    {
        switch (instr.CommandName)
        {
            case VirtualCommandName::LDC:
                stack.push(instr.Value);
                break;

            case VirtualCommandName::LDV:
                stack.push(memory[instr.TargetAddress]);
                break;

            case VirtualCommandName::SVV:
                memory[instr.TargetAddress] = stack.top();
                stack.pop();
                break;

            case VirtualCommandName::OPR:
                performOperation(instr.Operation);
                break;

            case VirtualCommandName::CALL:
                stack.push(pc);
                pc = instr.TargetAddress;
                break;

            case VirtualCommandName::RET:
                pc = static_cast<int>(stack.top());
                stack.pop();
                break;

            case VirtualCommandName::JMP:
                pc = instr.TargetAddress;
                break;

            case VirtualCommandName::JCON:
                if (stack.top() != 0)
                    pc = instr.TargetAddress;
                stack.pop();
                break;

            case VirtualCommandName::END:
                running = false;
                break;

            default:
                throw std::runtime_error("Unknown instruction.");
        }
    }

public:
    void loadProgram(const std::vector<VirtualCodeCommand>& prog)
    {
        program = prog;
        pc = 0;
        running = true;
    }

    void run()
    {
        while (running && pc < program.size())
        {
            executeInstruction(program[pc]);
            pc++;
        }
    }

    void printMemory() const
    {
        for (const auto& [addr, value] : memory)
        {
            std::cout << "Memory[" << addr << "] = " << value << std::endl;
        }
    }

    void printStack() const
    {
        std::stack<double> temp = stack;
        std::cout << "Stack (top to bottom): ";
        while (!temp.empty())
        {
            std::cout << temp.top() << " ";
            temp.pop();
        }
        std::cout << std::endl;
    }
};

std::vector<VirtualCodeCommand> createSampleProgram()
{
    return
    {
        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_TYPE, 10, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_TYPE, 10, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_TYPE, 20, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::OPR, VirtualCommandOperationType::ADD, VirtualCommandDataType::SIGNED_TYPE, 0, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::OPR, VirtualCommandOperationType::MUL, VirtualCommandDataType::SIGNED_TYPE, 0, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::SVV, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_TYPE, 0, 0, 0, 0, 0, 0, 20 },
        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_TYPE, 5, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::SVV, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_TYPE, 0, 0, 0, 0, 0, 0, 15 },
        //{ VirtualCommandName::RET, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_TYPE, 0, 0, 0, 0, 0, 0, 0 }
    };
}

int test()
{
    VirtualMachine vm;

    auto program = createSampleProgram();
    vm.loadProgram(program);

    vm.run();

    vm.printStack();
    vm.printMemory();

    return 0;
}
