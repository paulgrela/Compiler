
#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>
#include <functional>
#include <cmath>

#include "Types.h"

#include "VirtualMachine.h"

using namespace std;

const std::vector<VirtualCommandDataType> VirtualCommandDataTypeList =
{
    VirtualCommandDataType::SIGNED_CHAR_TYPE,
    VirtualCommandDataType::UNSIGNED_CHAR_TYPE,

    VirtualCommandDataType::SIGNED_SHORT_TYPE,
    VirtualCommandDataType::UNSIGNED_SHORT_TYPE,

    VirtualCommandDataType::SIGNED_INT_TYPE,
    VirtualCommandDataType::UNSIGNED_INT_TYPE,

    VirtualCommandDataType::SIGNED_LONG_INT_TYPE,
    VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE,

    VirtualCommandDataType::FLOAT_TYPE,
    VirtualCommandDataType::DOUBLE_TYPE,
    VirtualCommandDataType::LONG_DOUBLE_TYPE,
    VirtualCommandDataType::EXTENDED_TYPE,

    VirtualCommandDataType::BCD_TYPE,
    VirtualCommandDataType::VOID_TYPE
};

struct StackDataType
{
    VirtualCommandDataType Type;
    union
    {
        uint8_t UnsignedCharType;
        int8_t SignedCharType;

        uint16_t UnsignedShortType;
        int16_t SignedShortType;

        uint32_t UnsignedIntType;
        int32_t SignedIntType;

        UnsignedInt UnsignedLongIntType;
        SignedInt SignedLongIntType;

        float FloatType;
        double DoubleType;
        RealType LongDoubleType;
    };

    StackDataType(const VirtualCommandDataType TypeParam, const UnsignedInt ValueParam) : Type(TypeParam), UnsignedLongIntType(ValueParam)
    {
    }
    StackDataType(const VirtualCommandDataType TypeParam, const SignedInt ValueParam) : Type(TypeParam), SignedLongIntType(ValueParam)
    {
    }

    StackDataType(const VirtualCommandDataType TypeParam, const uint32_t ValueParam) : Type(TypeParam), UnsignedIntType(ValueParam)
    {
    }
    StackDataType(const VirtualCommandDataType TypeParam, const int32_t ValueParam) : Type(TypeParam), SignedIntType(ValueParam)
    {
    }

    StackDataType(const VirtualCommandDataType TypeParam, const uint16_t ValueParam) : Type(TypeParam), UnsignedShortType(ValueParam)
    {
    }
    StackDataType(const VirtualCommandDataType TypeParam, const int16_t ValueParam) : Type(TypeParam), SignedShortType(ValueParam)
    {
    }

    StackDataType(const VirtualCommandDataType TypeParam, const uint8_t ValueParam) : Type(TypeParam), UnsignedCharType(ValueParam)
    {
    }
    StackDataType(const VirtualCommandDataType TypeParam, const int8_t ValueParam) : Type(TypeParam), SignedCharType(ValueParam)
    {
    }

    StackDataType(const VirtualCommandDataType TypeParam, const RealType ValueParam) : Type(TypeParam), LongDoubleType(ValueParam)
    {
    }
    StackDataType(const VirtualCommandDataType TypeParam, const double ValueParam) : Type(TypeParam), DoubleType(ValueParam)
    {
    }
    StackDataType(const VirtualCommandDataType TypeParam, const float ValueParam) : Type(TypeParam), FloatType(ValueParam)
    {
    }
};

class VirtualMachine
{
private:
    UnsignedInt MemorySize{};
private:
    std::stack<StackDataType> VirtualMachineStack;
    std::vector<std::uint8_t> VirtualMachineMemory;
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

        // if (R1.Type == VirtualCommandDataType::FLOAT_TYPE || R1.Type == VirtualCommandDataType::DOUBLE_TYPE || R1.Type == VirtualCommandDataType::LONG_DOUBLE_TYPE || R1.Type == VirtualCommandDataType::EXTENDED_TYPE)
        //     VirtualMachineStack.push(StackDataType(R1.Type, ExecuteArithmeticOperation<RealType>(OperationType, R1.LongDoubleType, R2.LongDoubleType)));
        // else
        // if (R1.Type == VirtualCommandDataType::SIGNED_CHAR_TYPE || R1.Type == VirtualCommandDataType::SIGNED_SHORT_TYPE || R1.Type == VirtualCommandDataType::SIGNED_INT_TYPE || R1.Type == VirtualCommandDataType::SIGNED_LONG_INT_TYPE)
        //      VirtualMachineStack.push(StackDataType(R1.Type, ExecuteArithmeticOperation<SignedInt>(OperationType, R1.SignedLongIntType, R2.SignedLongIntType)));
        // else
        // if (R1.Type == VirtualCommandDataType::UNSIGNED_CHAR_TYPE || R1.Type == VirtualCommandDataType::UNSIGNED_SHORT_TYPE || R1.Type == VirtualCommandDataType::UNSIGNED_INT_TYPE || R1.Type == VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE)
        //     VirtualMachineStack.push(StackDataType(R1.Type, ExecuteArithmeticOperation<UnsignedInt>(OperationType, R1.UnsignedLongIntType, R2.UnsignedLongIntType)));

        if (R1.Type == VirtualCommandDataType::FLOAT_TYPE)
            VirtualMachineStack.push(StackDataType(R1.Type, ExecuteArithmeticOperation<float>(OperationType, R1.LongDoubleType, R2.LongDoubleType)));
        else
        if (R1.Type == VirtualCommandDataType::DOUBLE_TYPE)
            VirtualMachineStack.push(StackDataType(R1.Type, ExecuteArithmeticOperation<double>(OperationType, R1.LongDoubleType, R2.LongDoubleType)));
        else
        if (R1.Type == VirtualCommandDataType::LONG_DOUBLE_TYPE)
            VirtualMachineStack.push(StackDataType(R1.Type, ExecuteArithmeticOperation<RealType>(OperationType, R1.LongDoubleType, R2.LongDoubleType)));
        else
        if (R1.Type == VirtualCommandDataType::EXTENDED_TYPE)
            VirtualMachineStack.push(StackDataType(R1.Type, ExecuteArithmeticOperation<RealType>(OperationType, R1.LongDoubleType, R2.LongDoubleType)));
        else

        if (R1.Type == VirtualCommandDataType::SIGNED_CHAR_TYPE)
            VirtualMachineStack.push(StackDataType(R1.Type, ExecuteArithmeticOperation<int8_t>(OperationType, R1.SignedLongIntType, R2.SignedLongIntType)));
        else
        if (R1.Type == VirtualCommandDataType::SIGNED_SHORT_TYPE)
            VirtualMachineStack.push(StackDataType(R1.Type, ExecuteArithmeticOperation<int16_t>(OperationType, R1.SignedLongIntType, R2.SignedLongIntType)));
        else
        if (R1.Type == VirtualCommandDataType::SIGNED_INT_TYPE)
            VirtualMachineStack.push(StackDataType(R1.Type, ExecuteArithmeticOperation<int32_t>(OperationType, R1.SignedLongIntType, R2.SignedLongIntType)));
        else
        if (R1.Type == VirtualCommandDataType::SIGNED_LONG_INT_TYPE)
            VirtualMachineStack.push(StackDataType(R1.Type, ExecuteArithmeticOperation<SignedInt>(OperationType, R1.SignedLongIntType, R2.SignedLongIntType)));
        else

        if (R1.Type == VirtualCommandDataType::UNSIGNED_CHAR_TYPE)
            VirtualMachineStack.push(StackDataType(R1.Type, ExecuteArithmeticOperation<uint8_t>(OperationType, R1.UnsignedLongIntType, R2.UnsignedLongIntType)));
        else
        if (R1.Type == VirtualCommandDataType::UNSIGNED_SHORT_TYPE)
            VirtualMachineStack.push(StackDataType(R1.Type, ExecuteArithmeticOperation<uint16_t>(OperationType, R1.UnsignedLongIntType, R2.UnsignedLongIntType)));
        else
        if (R1.Type == VirtualCommandDataType::UNSIGNED_INT_TYPE)
            VirtualMachineStack.push(StackDataType(R1.Type, ExecuteArithmeticOperation<uint32_t>(OperationType, R1.UnsignedLongIntType, R2.UnsignedLongIntType)));
        else
        if (R1.Type == VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE)
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

    // void LoadConstantToStack(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    // {
    //     if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::FLOAT_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::DOUBLE_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::LONG_DOUBLE_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::EXTENDED_TYPE)
    //         VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, VirtualCodeCommandToExecute.Value));
    //     else
    //     if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::SIGNED_CHAR_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::SIGNED_SHORT_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::SIGNED_INT_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::SIGNED_LONG_INT_TYPE)
    //         VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<SignedInt>(VirtualCodeCommandToExecute.Value)));
    //     else
    //     if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::UNSIGNED_CHAR_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::UNSIGNED_SHORT_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::UNSIGNED_INT_TYPE || VirtualCodeCommandToExecute.Type == VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE)
    //         VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<UnsignedInt>(VirtualCodeCommandToExecute.Value)));
    // }

    void LoadConstantToStack(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::FLOAT_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<float>(VirtualCodeCommandToExecute.Value)));
        else
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::DOUBLE_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<double>(VirtualCodeCommandToExecute.Value)));
        else
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::LONG_DOUBLE_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<RealType>(VirtualCodeCommandToExecute.Value)));
        else
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::EXTENDED_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<RealType>(VirtualCodeCommandToExecute.Value)));
        else

        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::SIGNED_CHAR_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<int8_t>(VirtualCodeCommandToExecute.Value)));
        else
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::SIGNED_SHORT_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<int16_t>(VirtualCodeCommandToExecute.Value)));
        else
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::SIGNED_INT_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<int32_t>(VirtualCodeCommandToExecute.Value)));
        else
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::SIGNED_LONG_INT_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<SignedInt>(VirtualCodeCommandToExecute.Value)));
        else

        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::UNSIGNED_CHAR_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<uint8_t>(VirtualCodeCommandToExecute.Value)));
        else
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::UNSIGNED_SHORT_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<uint16_t>(VirtualCodeCommandToExecute.Value)));
        else
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::UNSIGNED_INT_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<uint32_t>(VirtualCodeCommandToExecute.Value)));
        else
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<UnsignedInt>(VirtualCodeCommandToExecute.Value)));
    }

    template <class T1, class T2>
    bool f1(const VirtualCodeCommand& VirtualCodeCommandToExecute, const VirtualCommandDataType VirtualCommandDataTypeParam)
    {
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeParam)
        {
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<T2>(*reinterpret_cast<T1>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress))));
            return true;
        }
        return false;
    }
    template <class T>
    bool fx(const VirtualCodeCommand& VirtualCodeCommandToExecute, const VirtualCommandDataType VirtualCommandDataTypeParam)
    {
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeParam)
        {
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, *reinterpret_cast<T>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress)));
            return true;
        }
        return false;
    }

    // auto Printer = [](auto&&... args)
    // {
    //     (args.show(), ...);
    // };
    //
    // Printer(w1, w2, w3, w4, w5, w6, t1, t2, t3);
    //(fff<RealType, float*>(VirtualCodeCommandToExecute, VirtualCommandDataType::FLOAT_TYPE), ...);

    void LoadVariableToStack(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        // auto Printer = [=](auto&&... args)
        // {
        //     (fx<args>(VirtualCodeCommandToExecute, VirtualCommandDataType::FLOAT_TYPE), ...);
        // };
        // Printer(float, double, RealType, RealType);
        // if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::FLOAT_TYPE)
        //     VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<args>(*reinterpret_cast<float*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress)))),


        // if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::FLOAT_TYPE)
        //     VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<RealType>(*reinterpret_cast<float*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress))));
        // else
        // if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::DOUBLE_TYPE)
        //     VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<RealType>(*reinterpret_cast<double*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress))));
        // else
        // if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::LONG_DOUBLE_TYPE)
        //     VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, *reinterpret_cast<RealType*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress)));
        // else
        // if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::EXTENDED_TYPE)
        //     VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, *reinterpret_cast<RealType*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress)));
        // else
        // if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::SIGNED_CHAR_TYPE)
        //     VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<SignedInt>(*reinterpret_cast<int8_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress))));
        // else
        // if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::SIGNED_SHORT_TYPE)
        //     VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<SignedInt>(*reinterpret_cast<int16_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress))));
        // else
        // if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::SIGNED_INT_TYPE)
        //     VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<SignedInt>(*reinterpret_cast<int32_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress))));
        // else
        // if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::SIGNED_LONG_INT_TYPE)
        //     VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, *reinterpret_cast<SignedInt*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress)));
        // else
        // if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::UNSIGNED_CHAR_TYPE)
        //     VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<UnsignedInt>(*reinterpret_cast<uint8_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress))));
        // else
        // if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::UNSIGNED_SHORT_TYPE)
        //     VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<UnsignedInt>(*reinterpret_cast<uint16_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress))));
        // else
        // if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::UNSIGNED_INT_TYPE)
        //     VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, static_cast<UnsignedInt>(*reinterpret_cast<uint32_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress))));
        // else
        // if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE)
        //     VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, *reinterpret_cast<UnsignedInt*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress)));

        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::FLOAT_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, *reinterpret_cast<float*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress)));
        else
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::DOUBLE_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, *reinterpret_cast<double*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress)));
        else
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::LONG_DOUBLE_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, *reinterpret_cast<RealType*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress)));
        else
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::EXTENDED_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, *reinterpret_cast<RealType*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress)));
        else
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::SIGNED_CHAR_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, *reinterpret_cast<int8_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress)));
        else
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::SIGNED_SHORT_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, *reinterpret_cast<int16_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress)));
        else
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::SIGNED_INT_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, *reinterpret_cast<int32_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress)));
        else
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::SIGNED_LONG_INT_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, *reinterpret_cast<SignedInt*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress)));
        else
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::UNSIGNED_CHAR_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, *reinterpret_cast<uint8_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress)));
        else
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::UNSIGNED_SHORT_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, *reinterpret_cast<uint16_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress)));
        else
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::UNSIGNED_INT_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, *reinterpret_cast<uint32_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress)));
        else
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE)
            VirtualMachineStack.push(StackDataType(VirtualCodeCommandToExecute.Type, *reinterpret_cast<UnsignedInt*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress)));
    }

    void SaveValueFromStackToVariable(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        // if (VirtualMachineStack.top().Type == VirtualCommandDataType::FLOAT_TYPE)
        //     *reinterpret_cast<float*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = static_cast<float>(VirtualMachineStack.top().LongDoubleType);
        // else
        // if (VirtualMachineStack.top().Type == VirtualCommandDataType::DOUBLE_TYPE)
        //     *reinterpret_cast<double*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = static_cast<double>(VirtualMachineStack.top().LongDoubleType);
        // else
        // if (VirtualMachineStack.top().Type == VirtualCommandDataType::LONG_DOUBLE_TYPE)
        //     *reinterpret_cast<RealType*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = VirtualMachineStack.top().LongDoubleType;
        // else
        // if (VirtualMachineStack.top().Type == VirtualCommandDataType::EXTENDED_TYPE)
        //     *reinterpret_cast<RealType*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = VirtualMachineStack.top().LongDoubleType;
        // else
        // if (VirtualMachineStack.top().Type == VirtualCommandDataType::SIGNED_CHAR_TYPE)
        //     *reinterpret_cast<int8_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = static_cast<int8_t>(VirtualMachineStack.top().SignedLongIntType);
        // else
        // if (VirtualMachineStack.top().Type == VirtualCommandDataType::SIGNED_SHORT_TYPE)
        //     *reinterpret_cast<int16_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = static_cast<int16_t>(VirtualMachineStack.top().SignedLongIntType);
        // else
        // if (VirtualMachineStack.top().Type == VirtualCommandDataType::SIGNED_INT_TYPE)
        //     *reinterpret_cast<int32_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = static_cast<int32_t>(VirtualMachineStack.top().SignedLongIntType);
        // else
        // if (VirtualMachineStack.top().Type == VirtualCommandDataType::SIGNED_LONG_INT_TYPE)
        //     *reinterpret_cast<int64_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = VirtualMachineStack.top().SignedLongIntType;
        // else
        // if (VirtualMachineStack.top().Type == VirtualCommandDataType::UNSIGNED_CHAR_TYPE)
        //     *reinterpret_cast<uint8_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = static_cast<uint8_t>(VirtualMachineStack.top().UnsignedLongIntType);
        // else
        // if (VirtualMachineStack.top().Type == VirtualCommandDataType::UNSIGNED_SHORT_TYPE)
        //     *reinterpret_cast<uint16_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = static_cast<uint16_t>(VirtualMachineStack.top().UnsignedLongIntType);
        // else
        // if (VirtualMachineStack.top().Type == VirtualCommandDataType::UNSIGNED_INT_TYPE)
        //     *reinterpret_cast<uint32_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = static_cast<int32_t>(VirtualMachineStack.top().UnsignedLongIntType);
        // else
        // if (VirtualMachineStack.top().Type == VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE)
        //     *reinterpret_cast<uint64_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = VirtualMachineStack.top().UnsignedLongIntType;


        if (VirtualMachineStack.top().Type == VirtualCommandDataType::FLOAT_TYPE)
            *reinterpret_cast<float*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = static_cast<float>(VirtualMachineStack.top().FloatType);
        else
        if (VirtualMachineStack.top().Type == VirtualCommandDataType::DOUBLE_TYPE)
            *reinterpret_cast<double*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = static_cast<double>(VirtualMachineStack.top().DoubleType);
        else
        if (VirtualMachineStack.top().Type == VirtualCommandDataType::LONG_DOUBLE_TYPE)
            *reinterpret_cast<RealType*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = VirtualMachineStack.top().LongDoubleType;
        else
        if (VirtualMachineStack.top().Type == VirtualCommandDataType::EXTENDED_TYPE)
            *reinterpret_cast<RealType*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = VirtualMachineStack.top().LongDoubleType;
        else
        if (VirtualMachineStack.top().Type == VirtualCommandDataType::SIGNED_CHAR_TYPE)
            *reinterpret_cast<int8_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = static_cast<int8_t>(VirtualMachineStack.top().SignedCharType);
        else
        if (VirtualMachineStack.top().Type == VirtualCommandDataType::SIGNED_SHORT_TYPE)
            *reinterpret_cast<int16_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = static_cast<int16_t>(VirtualMachineStack.top().SignedShortType);
        else
        if (VirtualMachineStack.top().Type == VirtualCommandDataType::SIGNED_INT_TYPE)
            *reinterpret_cast<int32_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = static_cast<int32_t>(VirtualMachineStack.top().SignedIntType);
        else
        if (VirtualMachineStack.top().Type == VirtualCommandDataType::SIGNED_LONG_INT_TYPE)
            *reinterpret_cast<int64_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = VirtualMachineStack.top().SignedLongIntType;
        else
        if (VirtualMachineStack.top().Type == VirtualCommandDataType::UNSIGNED_CHAR_TYPE)
            *reinterpret_cast<uint8_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = static_cast<uint8_t>(VirtualMachineStack.top().UnsignedCharType);
        else
        if (VirtualMachineStack.top().Type == VirtualCommandDataType::UNSIGNED_SHORT_TYPE)
            *reinterpret_cast<uint16_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = static_cast<uint16_t>(VirtualMachineStack.top().UnsignedShortType);
        else
        if (VirtualMachineStack.top().Type == VirtualCommandDataType::UNSIGNED_INT_TYPE)
            *reinterpret_cast<uint32_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = static_cast<int32_t>(VirtualMachineStack.top().UnsignedIntType);
        else
        if (VirtualMachineStack.top().Type == VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE)
            *reinterpret_cast<uint64_t*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = VirtualMachineStack.top().UnsignedLongIntType;

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

        // if (R1.Type == VirtualCommandDataType::FLOAT_TYPE || R1.Type == VirtualCommandDataType::DOUBLE_TYPE || R1.Type == VirtualCommandDataType::LONG_DOUBLE_TYPE || R1.Type == VirtualCommandDataType::EXTENDED_TYPE)
        //     if (R1.LongDoubleType == 0)
        //         RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress;
        // if (R1.Type == VirtualCommandDataType::SIGNED_CHAR_TYPE || R1.Type == VirtualCommandDataType::SIGNED_SHORT_TYPE || R1.Type == VirtualCommandDataType::SIGNED_INT_TYPE || R1.Type == VirtualCommandDataType::SIGNED_LONG_INT_TYPE)
        //     if (R1.SignedLongIntType == 0)
        //         RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress;
        // if (R1.Type == VirtualCommandDataType::UNSIGNED_CHAR_TYPE || R1.Type == VirtualCommandDataType::UNSIGNED_SHORT_TYPE || R1.Type == VirtualCommandDataType::UNSIGNED_INT_TYPE || R1.Type == VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE)
        //     if (R1.UnsignedLongIntType == 0)
        //         RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress;

        if (R1.Type == VirtualCommandDataType::FLOAT_TYPE)
            if (R1.FloatType == 0)
                RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress;
        if (R1.Type == VirtualCommandDataType::DOUBLE_TYPE)
            if (R1.DoubleType == 0)
                RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress;
        if (R1.Type == VirtualCommandDataType::LONG_DOUBLE_TYPE)
            if (R1.LongDoubleType == 0)
                RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress;
        if (R1.Type == VirtualCommandDataType::EXTENDED_TYPE)
            if (R1.LongDoubleType == 0)
                RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress;

        if (R1.Type == VirtualCommandDataType::SIGNED_CHAR_TYPE )
            if (R1.SignedCharType == 0)
                RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress;
        if (R1.Type == VirtualCommandDataType::SIGNED_SHORT_TYPE)
            if (R1.SignedShortType == 0)
                RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress;
        if (R1.Type == VirtualCommandDataType::SIGNED_INT_TYPE)
            if (R1.SignedIntType == 0)
                RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress;
        if (R1.Type == VirtualCommandDataType::SIGNED_LONG_INT_TYPE)
            if (R1.SignedLongIntType == 0)
                RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress;

        if (R1.Type == VirtualCommandDataType::UNSIGNED_CHAR_TYPE)
            if (R1.UnsignedCharType == 0)
                RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress;
        if (R1.Type == VirtualCommandDataType::UNSIGNED_SHORT_TYPE)
            if (R1.UnsignedShortType == 0)
                RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress;
        if (R1.Type == VirtualCommandDataType::UNSIGNED_INT_TYPE)
            if (R1.UnsignedIntType == 0)
                RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress;
        if (R1.Type == VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE)
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
        for (UnsignedInt ByteIndex = 0; ByteIndex < VirtualMachineMemory.size(); ByteIndex++)
            cout << "Memory[" << to_string(ByteIndex) << "] = " << to_string(VirtualMachineMemory[ByteIndex]) << endl;
        getchar();
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

    explicit VirtualMachine(const UnsignedInt MemorySizeParam) : MemorySize(MemorySizeParam), VirtualMachineMemory(MemorySize)
    {
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

void ReinterpretCastTest()
{
    vector<unsigned char> ByteVector(128);

    cout << "REINTERPRET CAST DATA" << endl;

    for (auto& Byte : ByteVector)
        Byte = 0;

    ByteVector[8] = 255;
    ByteVector[9] = 255;
    cout << reinterpret_cast<uint64_t*>(ByteVector.data())[1] << endl;
    cout << *reinterpret_cast<uint64_t*>(ByteVector.data() + 8) << endl;
}

int TestVirtualMachine()
{
    VirtualMachine VirtualMachinObject(32);

    const auto Program = CreateSampleProgram();

    VirtualMachinObject.LoadProgram(Program);
    VirtualMachinObject.RunProgram();
    VirtualMachinObject.PrintStack();
    VirtualMachinObject.PrintMemory();

    return 0;
}

template RealType VirtualMachine::ExecuteArithmeticOperation<RealType>(const VirtualCommandOperationType& OperationType, RealType R1, RealType R2);
template double VirtualMachine::ExecuteArithmeticOperation<double>(const VirtualCommandOperationType& OperationType, double R1, double R2);
template float VirtualMachine::ExecuteArithmeticOperation<float>(const VirtualCommandOperationType& OperationType, float R1, float R2);

template SignedInt VirtualMachine::ExecuteArithmeticOperation<SignedInt>(const VirtualCommandOperationType& OperationType, SignedInt R1, SignedInt R2);
template int32_t VirtualMachine::ExecuteArithmeticOperation<int32_t>(const VirtualCommandOperationType& OperationType, int32_t R1, int32_t R2);
template int16_t VirtualMachine::ExecuteArithmeticOperation<int16_t>(const VirtualCommandOperationType& OperationType, int16_t R1, int16_t R2);
template int8_t VirtualMachine::ExecuteArithmeticOperation<int8_t>(const VirtualCommandOperationType& OperationType, int8_t R1, int8_t R2);

template UnsignedInt VirtualMachine::ExecuteArithmeticOperation<UnsignedInt>(const VirtualCommandOperationType& OperationType, UnsignedInt R1, UnsignedInt R2);
template uint32_t VirtualMachine::ExecuteArithmeticOperation<uint32_t>(const VirtualCommandOperationType& OperationType, uint32_t R1, uint32_t R2);
template uint16_t VirtualMachine::ExecuteArithmeticOperation<uint16_t>(const VirtualCommandOperationType& OperationType, uint16_t R1, uint16_t R2);
template uint8_t VirtualMachine::ExecuteArithmeticOperation<uint8_t>(const VirtualCommandOperationType& OperationType, uint8_t R1, uint8_t R2);
