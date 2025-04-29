
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/vector.hpp>

#include <iostream>
#include <vector>
#include <stack>
#include <list>
#include <functional>
#include <cmath>

#include "Types.h"
#include "Constants.h"

#include "VirtualMachine.h"

using namespace std;

using ValidTypes = boost::mpl::vector<float, double, RealType, RealType, int8_t, int16_t, int32_t, SignedInt, uint8_t, uint16_t, uint32_t, UnsignedInt>;

const std::vector<VirtualCommandDataType> VirtualCommandDataTypeList =
{
    VirtualCommandDataType::FLOAT_TYPE,
    VirtualCommandDataType::DOUBLE_TYPE,
    VirtualCommandDataType::LONG_DOUBLE_TYPE,
    VirtualCommandDataType::EXTENDED_TYPE,

    VirtualCommandDataType::SIGNED_CHAR_TYPE,
    VirtualCommandDataType::SIGNED_SHORT_TYPE,
    VirtualCommandDataType::SIGNED_INT_TYPE,
    VirtualCommandDataType::SIGNED_LONG_INT_TYPE,

    VirtualCommandDataType::UNSIGNED_CHAR_TYPE,
    VirtualCommandDataType::UNSIGNED_SHORT_TYPE,
    VirtualCommandDataType::UNSIGNED_INT_TYPE,
    VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE,
};

union ArithmeticTypeUnion
{
    float FloatType;
    double DoubleType;
    RealType LongDoubleType;
    RealType ExtendedType;

    int8_t SignedCharType;
    int16_t SignedShortType;
    int32_t SignedIntType;
    SignedInt SignedLongIntType;

    uint8_t UnsignedCharType;
    uint16_t UnsignedShortType;
    uint32_t UnsignedIntType;
    UnsignedInt UnsignedLongIntType;
};

struct StackDataType
{
    VirtualCommandDataType Type{};
    ArithmeticTypeUnion Data{};

    StackDataType(const VirtualCommandDataType TypeParam, const UnsignedInt ValueParam) : Type(TypeParam)
    {
        Data.UnsignedLongIntType = ValueParam;
    }
    StackDataType(const VirtualCommandDataType TypeParam, const SignedInt ValueParam) : Type(TypeParam)
    {
        Data.SignedLongIntType = ValueParam;
    }

    StackDataType(const VirtualCommandDataType TypeParam, const uint32_t ValueParam) : Type(TypeParam)
    {
        Data.UnsignedIntType = ValueParam;
    }
    StackDataType(const VirtualCommandDataType TypeParam, const int32_t ValueParam) : Type(TypeParam)
    {
        Data.SignedIntType = ValueParam;
    }

    StackDataType(const VirtualCommandDataType TypeParam, const uint16_t ValueParam) : Type(TypeParam)
    {
        Data.UnsignedShortType = ValueParam;
    }
    StackDataType(const VirtualCommandDataType TypeParam, const int16_t ValueParam) : Type(TypeParam)
    {
        Data.SignedShortType = ValueParam;
    }

    StackDataType(const VirtualCommandDataType TypeParam, const uint8_t ValueParam) : Type(TypeParam)
    {
        Data.UnsignedCharType = ValueParam;
    }
    StackDataType(const VirtualCommandDataType TypeParam, const int8_t ValueParam) : Type(TypeParam)
    {
        Data.SignedCharType = ValueParam;
    }

    StackDataType(const VirtualCommandDataType TypeParam, const RealType ValueParam) : Type(TypeParam)
    {
        Data.LongDoubleType = ValueParam;
    }
    StackDataType(const VirtualCommandDataType TypeParam, const double ValueParam) : Type(TypeParam)
    {
        Data.DoubleType = ValueParam;
    }
    StackDataType(const VirtualCommandDataType TypeParam, const float ValueParam) : Type(TypeParam)
    {
        Data.FloatType = ValueParam;
    }
};

class VirtualMachine
{
private:
    UnsignedInt MemorySize{};
    UnsignedInt StackSize{};
    UnsignedInt StackStartAddressESP{ 0 };
    UnsignedInt StackActualAddressEBP{ 0 };
private:
    std::vector<std::uint8_t> VirtualMachineMemory;
    std::stack<StackDataType> VirtualMachineStack;
    std::list<StackDataType> VirtualMachineFunctionParametersList{};
private:
    std::vector<VirtualCodeCommand> ProgramCodeCommandsList;
    UnsignedInt RunningVirtualCommandIndex = 0;
    bool ProgramRunning = true;
private:
    template <class T>
    T ExecuteArithmeticOperation(const VirtualCommandOperationType& OperationType, const T& R1, const T& R2)
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

    template <class T>
    void PerformOperationForType(const VirtualCodeCommand& VirtualCodeCommandToExecute, StackDataType& R1, StackDataType& R2, const VirtualCommandDataType VirtualCommandDataTypeParam)
    {
        if (VirtualCodeCommandToExecute.Operation != VirtualCommandOperationType::FREE && VirtualCodeCommandToExecute.Operation != VirtualCommandOperationType::RET)
        {
            if (R1.Type == VirtualCommandDataTypeParam)
                VirtualMachineStack.emplace(R1.Type, ExecuteArithmeticOperation<T>(VirtualCodeCommandToExecute.Operation, *static_cast<T*>(reinterpret_cast<void*>(&R1.Data)), *static_cast<T*>(reinterpret_cast<void*>(&R2.Data))));
        }
        else
        {
            if (VirtualCodeCommandToExecute.Operation != VirtualCommandOperationType::FREE)
                FreeMemoryForFunctionOnStack(VirtualCodeCommandToExecute);
            if (VirtualCodeCommandToExecute.Operation != VirtualCommandOperationType::RET)
                ReturnFromFunction(VirtualCodeCommandToExecute);
        }
    }

    void PerformOperation(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        auto R2 = VirtualMachineStack.top();

        VirtualMachineStack.pop();

        auto R1 = (VirtualCodeCommandToExecute.Operation != VirtualCommandOperationType::NEG) ? VirtualMachineStack.top() : StackDataType(VirtualMachineStack.top().Type, static_cast<RealType>(0.0));

        if (VirtualCodeCommandToExecute.Operation != VirtualCommandOperationType::NEG)
            VirtualMachineStack.pop();

        UnsignedInt TypeIndex = 0;
        boost::mpl::for_each<ValidTypes>([&]<typename T0>(T0 TypeArg) { PerformOperationForType<T0>(VirtualCodeCommandToExecute, R1, R2, VirtualCommandDataTypeList[TypeIndex++]); } );
    }

    void ExecuteVirtualCommand(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        switch (VirtualCodeCommandToExecute.CommandName)
        {
            case VirtualCommandName::LDC: LoadConstantToStack(VirtualCodeCommandToExecute); break;
            case VirtualCommandName::LDV:
            case VirtualCommandName::LDVFPTR: GeneralLoadVariableToStack(VirtualCodeCommandToExecute); break;
            case VirtualCommandName::SVV:
            case VirtualCommandName::SVVFPTR: GeneralSaveValueFromStackToVariable(VirtualCodeCommandToExecute); break;

            case VirtualCommandName::LDPTROFV: LoadPointerOfVariableToStack(VirtualCodeCommandToExecute); break;

            case VirtualCommandName::OPR: PerformOperation(VirtualCodeCommandToExecute); break;

            case VirtualCommandName::LOAD: GeneralLoadVariableToFunctionParametersList(VirtualCodeCommandToExecute); break;
            case VirtualCommandName::LDP: GeneralSaveValueFromParametersListToVariable(VirtualCodeCommandToExecute); break;

            case VirtualCommandName::GET: GetMemoryForFunctionOnStack(VirtualCodeCommandToExecute); break;
            case VirtualCommandName::CALL: CallFunction(VirtualCodeCommandToExecute); break;
            case VirtualCommandName::VIRTRET: FreeMemoryForFunctionOnStack(VirtualCodeCommandToExecute); break;

            case VirtualCommandName::JMP: Jump(VirtualCodeCommandToExecute); break;
            case VirtualCommandName::JCON: JumpConditional(VirtualCodeCommandToExecute); break;
            case VirtualCommandName::END: ProgramRunning = false; break;
            case VirtualCommandName::PRINT: PrintMemory(); break;

            default: throw runtime_error("Unknown instruction.");
        }
    }

private:

    template <class T>
    void LoadConstantToStackForType(const VirtualCodeCommand& VirtualCodeCommandToExecute, const VirtualCommandDataType VirtualCommandDataTypeParam)
    {
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeParam)
            VirtualMachineStack.emplace(VirtualCodeCommandToExecute.Type, static_cast<T>(VirtualCodeCommandToExecute.Value));
    }

    void LoadConstantToStack(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        UnsignedInt TypeIndex = 0;
        boost::mpl::for_each<ValidTypes>([&]<typename T0>(T0 TypeArg) { LoadConstantToStackForType<T0>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
    }

    template <class T>
    void LoadLocalVariableToStackForType(const VirtualCodeCommand& VirtualCodeCommandToExecute, const VirtualCommandDataType VirtualCommandDataTypeParam)
    {
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeParam)
            VirtualMachineStack.emplace(VirtualCodeCommandToExecute.Type, *reinterpret_cast<T*>(VirtualMachineMemory.data() + StackActualAddressEBP + VirtualCodeCommandToExecute.TargetAddress));
    }

    template <class T>
    void LoadGlobalVariableFromPointerToStackForType(const VirtualCodeCommand& VirtualCodeCommandToExecute, const VirtualCommandDataType VirtualCommandDataTypeParam)
    {
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeParam)
            VirtualMachineStack.emplace(VirtualCodeCommandToExecute.Type, *reinterpret_cast<T*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress));
    }

    void GeneralLoadVariableToStack(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        UnsignedInt TypeIndex = 0;
        if (VirtualCodeCommandToExecute.Level == LOCAL_TYPE)
            boost::mpl::for_each<ValidTypes>([&]<typename T0>(T0 TypeArg) { LoadLocalVariableToStackForType<T0>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
        else
        if (VirtualCodeCommandToExecute.Level == GLOBAL_TYPE)
            boost::mpl::for_each<ValidTypes>([&]<typename T0>(T0 TypeArg) { LoadGlobalVariableFromPointerToStackForType<T0>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
        else
        if (VirtualCodeCommandToExecute.Level == CLASS_TYPE)
        {
        }
    }

    template <class T>
    void SaveValueFromStackToVariableForType(const VirtualCodeCommand& VirtualCodeCommandToExecute, const VirtualCommandDataType VirtualCommandDataTypeParam)
    {
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeParam)
            *reinterpret_cast<T*>(VirtualMachineMemory.data() + StackActualAddressEBP + VirtualCodeCommandToExecute.TargetAddress) = *static_cast<T*>(reinterpret_cast<void*>(&VirtualMachineStack.top().Data));
    }

    template <class T>
    void SaveValueFromStackToVariableFromPointerForType(const VirtualCodeCommand& VirtualCodeCommandToExecute, const VirtualCommandDataType VirtualCommandDataTypeParam)
    {
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeParam)
            *reinterpret_cast<T*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = *static_cast<T*>(reinterpret_cast<void*>(&VirtualMachineStack.top().Data));
    }

    void GeneralSaveValueFromStackToVariable(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        UnsignedInt TypeIndex = 0;
        if (VirtualCodeCommandToExecute.Level == LOCAL_TYPE)
            boost::mpl::for_each<ValidTypes>([&]<typename T0>(T0 TypeArg) { SaveValueFromStackToVariableForType<T0>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
        else
        if (VirtualCodeCommandToExecute.Level == GLOBAL_TYPE)
            boost::mpl::for_each<ValidTypes>([&]<typename T0>(T0 TypeArg) { SaveValueFromStackToVariableFromPointerForType<T0>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
        else
        if (VirtualCodeCommandToExecute.Level == CLASS_TYPE)
        {
        }

        VirtualMachineStack.pop();
    }

    void LoadPointerOfVariableToStack(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        VirtualMachineStack.emplace(VirtualCodeCommandToExecute.Type, VirtualCodeCommandToExecute.TargetAddress);
    }

    void GetMemoryForFunctionOnStack(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        StackStartAddressESP += static_cast<UnsignedInt>(VirtualCodeCommandToExecute.Value);
        VirtualMachineStack.emplace(VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE, StackActualAddressEBP);
        StackActualAddressEBP = StackStartAddressESP;
    }

    void CallFunction(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        VirtualMachineStack.emplace(VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE, RunningVirtualCommandIndex);
        RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress;
    }

    void FreeMemoryForFunctionOnStack(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        StackActualAddressEBP = VirtualMachineStack.top().Data.UnsignedLongIntType;
        VirtualMachineStack.pop();
        StackStartAddressESP -= static_cast<UnsignedInt>(VirtualCodeCommandToExecute.Value);
    }

    void ReturnFromFunction(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        RunningVirtualCommandIndex = VirtualMachineStack.top().Data.UnsignedLongIntType;
        VirtualMachineStack.pop();
    }

    template <class T>
    void JumpConditionalForType(const VirtualCodeCommand& VirtualCodeCommandToExecute, const VirtualCommandDataType VirtualCommandDataTypeParam)
    {
        if (VirtualMachineStack.top().Type == VirtualCommandDataTypeParam)
            if (*static_cast<T*>(reinterpret_cast<void*>(&VirtualMachineStack.top().Data)) == 0)
                RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress;
    }

    void JumpConditional(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        const auto R1 = VirtualMachineStack.top();
        UnsignedInt TypeIndex = 0;
        boost::mpl::for_each<ValidTypes>([&]<typename T0>(T0 TypeArg) { JumpConditionalForType<T0>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );

        VirtualMachineStack.pop();
    }

    void Jump(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress;
    }














    template <class T>
    void LoadLocalVariableToFunctionParametersListForType(const VirtualCodeCommand& VirtualCodeCommandToExecute, const VirtualCommandDataType VirtualCommandDataTypeParam)
    {
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeParam)
            VirtualMachineFunctionParametersList.emplace_back(VirtualCodeCommandToExecute.Type, *reinterpret_cast<T*>(VirtualMachineMemory.data() + StackActualAddressEBP + VirtualCodeCommandToExecute.TargetAddress));
    }

    template <class T>
    void LoadGlobalVariableFromPointerToFunctionParametersListForType(const VirtualCodeCommand& VirtualCodeCommandToExecute, const VirtualCommandDataType VirtualCommandDataTypeParam)
    {
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeParam)
            VirtualMachineFunctionParametersList.emplace_back(VirtualCodeCommandToExecute.Type, *reinterpret_cast<T*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress));
    }

    //LOAD
    void GeneralLoadVariableToFunctionParametersList(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        UnsignedInt TypeIndex = 0;
        if (VirtualCodeCommandToExecute.Level == LOCAL_TYPE)
            boost::mpl::for_each<ValidTypes>([&]<typename T0>(T0 TypeArg) { LoadLocalVariableToFunctionParametersListForType<T0>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
        else
        if (VirtualCodeCommandToExecute.Level == GLOBAL_TYPE)
            boost::mpl::for_each<ValidTypes>([&]<typename T0>(T0 TypeArg) { LoadGlobalVariableFromPointerToFunctionParametersListForType<T0>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
        else
        if (VirtualCodeCommandToExecute.Level == CLASS_TYPE)
        {
        }
    }




    template <class T>
    void SaveValueFromParametersListToVariableForType(const VirtualCodeCommand& VirtualCodeCommandToExecute, const VirtualCommandDataType VirtualCommandDataTypeParam)
    {
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeParam)
            *reinterpret_cast<T*>(VirtualMachineMemory.data() + StackActualAddressEBP + VirtualCodeCommandToExecute.TargetAddress) = *static_cast<T*>(reinterpret_cast<void*>(&VirtualMachineFunctionParametersList.front().Data));
    }

    template <class T>
    void SaveValueFromParametersListToVariableFromPointerForType(const VirtualCodeCommand& VirtualCodeCommandToExecute, const VirtualCommandDataType VirtualCommandDataTypeParam)
    {
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeParam)
            *reinterpret_cast<T*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = *static_cast<T*>(reinterpret_cast<void*>(&VirtualMachineFunctionParametersList.front().Data));
    }

    //LDP
    void GeneralSaveValueFromParametersListToVariable(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        UnsignedInt TypeIndex = 0;
        if (VirtualCodeCommandToExecute.Level == LOCAL_TYPE)
            boost::mpl::for_each<ValidTypes>([&]<typename T0>(T0 TypeArg) { SaveValueFromStackToVariableForType<T0>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
        else
        if (VirtualCodeCommandToExecute.Level == GLOBAL_TYPE)
            boost::mpl::for_each<ValidTypes>([&]<typename T0>(T0 TypeArg) { SaveValueFromStackToVariableFromPointerForType<T0>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
        else
        if (VirtualCodeCommandToExecute.Level == CLASS_TYPE)
        {
        }

        VirtualMachineFunctionParametersList.pop_front();
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
            cout << static_cast<SignedInt>(TemporaryVirtualMachineStack.top().Type) << " " << TemporaryVirtualMachineStack.top().Data.LongDoubleType << " " << TemporaryVirtualMachineStack.top().Data.SignedLongIntType << " " << TemporaryVirtualMachineStack.top().Data.UnsignedLongIntType << " " << endl;
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
        { VirtualCommandName::SVV, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, LOCAL_TYPE, 0, 0, 0, 15 },

        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 10, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 10, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 20, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::OPR, VirtualCommandOperationType::ADD, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::OPR, VirtualCommandOperationType::MUL, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::SVV, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, LOCAL_TYPE, 0, 0, 0, 20 },

        { VirtualCommandName::PRINT, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, 0, 0, 0, 0, 0 },

        { VirtualCommandName::LDV, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, LOCAL_TYPE, 0, 0, 0, 15 },
        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 1, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::OPR, VirtualCommandOperationType::ADD, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::SVV, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, LOCAL_TYPE, 0, 0, 0, 15 },

        { VirtualCommandName::LDV, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, LOCAL_TYPE, 0, 0, 0, 15 },
        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 10, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::OPR, VirtualCommandOperationType::EQU, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::JCON, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, 0, 0, 0, 0, 7 },
    };
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
