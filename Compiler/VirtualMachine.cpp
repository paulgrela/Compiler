
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
#include "ExceptionsMacro.h"

#include "VirtualMachine.h"

constexpr bool PrintExecutedCommandData = true;
constexpr bool PrintExecutedStackData = true;

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

    template <class T>
    void PerformOperationForType(const VirtualCodeCommand& VirtualCodeCommandToExecute, StackDataType& R1, StackDataType& R2, const VirtualCommandDataType VirtualCommandDataTypeParam)
    {
        if (R1.Type == VirtualCommandDataTypeParam)
            VirtualMachineStack.emplace(R1.Type, ExecuteArithmeticOperation<T>(VirtualCodeCommandToExecute.Operation, *static_cast<T*>(reinterpret_cast<void*>(&R1.Data)), *static_cast<T*>(reinterpret_cast<void*>(&R2.Data))));
    }

    void PerformOperation(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        auto R2 = VirtualMachineStack.top();

        VirtualMachineStack.pop();

        auto R1 = (VirtualCodeCommandToExecute.Operation != VirtualCommandOperationType::NEG) ? VirtualMachineStack.top() : StackDataType(VirtualMachineStack.top().Type, static_cast<RealType>(0.0));

        if (VirtualCodeCommandToExecute.Operation != VirtualCommandOperationType::NEG)
            VirtualMachineStack.pop();

        if (VirtualCodeCommandToExecute.Operation != VirtualCommandOperationType::FREE && VirtualCodeCommandToExecute.Operation != VirtualCommandOperationType::RET)
        {
            UnsignedInt TypeIndex = 0;
            boost::mpl::for_each<ValidTypes>([&]<typename T>(T TypeArg) { PerformOperationForType<T>(VirtualCodeCommandToExecute, R1, R2, VirtualCommandDataTypeList[TypeIndex++]); } );
        }
        else
        {
            if (VirtualCodeCommandToExecute.Operation != VirtualCommandOperationType::FREE)
                FreeMemoryForFunctionOnStack(VirtualCodeCommandToExecute);
            if (VirtualCodeCommandToExecute.Operation != VirtualCommandOperationType::RET)
                ReturnFromFunction(VirtualCodeCommandToExecute);
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
        boost::mpl::for_each<ValidTypes>([&]<typename T>(T TypeArg) { LoadConstantToStackForType<T>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
    }

private:

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
        if (VirtualCodeCommandToExecute.Level == LOCAL_LEVEL)
            boost::mpl::for_each<ValidTypes>([&]<typename T>(T TypeArg) { LoadLocalVariableToStackForType<T>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
        else
        if (VirtualCodeCommandToExecute.Level == GLOBAL_LEVEL)
            boost::mpl::for_each<ValidTypes>([&]<typename T>(T TypeArg) { LoadGlobalVariableFromPointerToStackForType<T>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
        else
        if (VirtualCodeCommandToExecute.Level == CLASS_LEVEL)
        {
        }
    }

    template <class T>
    void LoadLocalVariableFromOffsetFromStackToStackForType(const VirtualCodeCommand& VirtualCodeCommandToExecute, const VirtualCommandDataType VirtualCommandDataTypeParam)
    {
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeParam)
        {
            const UnsignedInt Offset = *static_cast<T*>(reinterpret_cast<void*>(&VirtualMachineStack.top().Data));
            VirtualMachineStack.pop();
            VirtualMachineStack.emplace(VirtualCodeCommandToExecute.Type, *reinterpret_cast<T*>(VirtualMachineMemory.data() + StackActualAddressEBP + Offset));
        }
    }

    template <class T>
    void LoadGlobalVariableFromPointerFromOffsetFromStackToStackForType1(const VirtualCodeCommand& VirtualCodeCommandToExecute, const VirtualCommandDataType VirtualCommandDataTypeParam)
    {
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeParam)
        {
            const UnsignedInt Offset = *static_cast<T*>(reinterpret_cast<void*>(&VirtualMachineStack.top().Data));
            VirtualMachineStack.pop();
            VirtualMachineStack.emplace(VirtualCodeCommandToExecute.Type, *reinterpret_cast<T*>(VirtualMachineMemory.data() + Offset));
        }
    }

    void GeneralLoadVariableFromOffsetFromStackToStack(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        UnsignedInt TypeIndex = 0;
        if (VirtualCodeCommandToExecute.Level == LOCAL_LEVEL)
            boost::mpl::for_each<ValidTypes>([&]<typename T>(T TypeArg) { LoadLocalVariableFromOffsetFromStackToStackForType<T>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
        else
        if (VirtualCodeCommandToExecute.Level == GLOBAL_LEVEL)
            boost::mpl::for_each<ValidTypes>([&]<typename T>(T TypeArg) { LoadGlobalVariableFromPointerFromOffsetFromStackToStackForType1<T>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
        else
        if (VirtualCodeCommandToExecute.Level == CLASS_LEVEL)
        {
        }
    }

private:

    template <class T>
    void SaveValueFromStackToVariableForType(const VirtualCodeCommand& VirtualCodeCommandToExecute, const VirtualCommandDataType VirtualCommandDataTypeParam)
    {
        if (VirtualMachineStack.top().Type == VirtualCodeCommandToExecute.Type)
        {
            if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeParam)
                *reinterpret_cast<T*>(VirtualMachineMemory.data() + StackActualAddressEBP + VirtualCodeCommandToExecute.TargetAddress) = *static_cast<T*>(reinterpret_cast<void*>(&VirtualMachineStack.top().Data));
        }
        else
        {
            UnsignedInt TypeIndex = 0;
            if (VirtualMachineStack.top().Type == VirtualCommandDataTypeParam)
                boost::mpl::for_each<ValidTypes>([&]<typename T1>(T1 TypeArg)
                {
                    if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeList[TypeIndex++])
                        *reinterpret_cast<T1*>(VirtualMachineMemory.data() + StackActualAddressEBP + VirtualCodeCommandToExecute.TargetAddress) = static_cast<T1>(*static_cast<T*>(reinterpret_cast<void*>(&VirtualMachineStack.top().Data)));
                });
        }
    }

    template <class T>
    void SaveValueFromStackToVariableFromPointerForType(const VirtualCodeCommand& VirtualCodeCommandToExecute, const VirtualCommandDataType VirtualCommandDataTypeParam)
    {
        if (VirtualMachineStack.top().Type == VirtualCodeCommandToExecute.Type)
        {
            if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeParam)
                *reinterpret_cast<T*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = *static_cast<T*>(reinterpret_cast<void*>(&VirtualMachineStack.top().Data));
        }
        else
        {
            UnsignedInt TypeIndex = 0;
            if (VirtualMachineStack.top().Type == VirtualCommandDataTypeParam)
                boost::mpl::for_each<ValidTypes>([&]<typename T1>(T1 TypeArg)
                {
                    if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeList[TypeIndex++])
                        *reinterpret_cast<T1*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = static_cast<T1>(*static_cast<T*>(reinterpret_cast<void*>(&VirtualMachineStack.top().Data)));
                });
        }
    }

    void GeneralSaveValueFromStackToVariable(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        UnsignedInt TypeIndex = 0;
        if (VirtualCodeCommandToExecute.Level == LOCAL_LEVEL)
            boost::mpl::for_each<ValidTypes>([&]<typename T>(T TypeArg) { SaveValueFromStackToVariableForType<T>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
        else
        if (VirtualCodeCommandToExecute.Level == GLOBAL_LEVEL)
            boost::mpl::for_each<ValidTypes>([&]<typename T>(T TypeArg) { SaveValueFromStackToVariableFromPointerForType<T>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
        else
        if (VirtualCodeCommandToExecute.Level == CLASS_LEVEL)
        {
        }

        VirtualMachineStack.pop();
    }

    template <class T>
    void SaveValueFromStackToVariableFromOffsetFromStackForType(const VirtualCodeCommand& VirtualCodeCommandToExecute, const VirtualCommandDataType VirtualCommandDataTypeParam, const UnsignedInt Offset)
    {
        if (VirtualMachineStack.top().Type == VirtualCodeCommandToExecute.Type)
        {
            if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeParam)
                *reinterpret_cast<T*>(VirtualMachineMemory.data() + StackActualAddressEBP + Offset) = *static_cast<T*>(reinterpret_cast<void*>(&VirtualMachineStack.top().Data));
        }
        else
        {
            UnsignedInt TypeIndex = 0;
            if (VirtualMachineStack.top().Type == VirtualCommandDataTypeParam)
                boost::mpl::for_each<ValidTypes>([&]<typename T1>(T1 TypeArg)
                {
                    if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeList[TypeIndex++])
                        *reinterpret_cast<T1*>(VirtualMachineMemory.data() + StackActualAddressEBP + Offset) = static_cast<T1>(*static_cast<T*>(reinterpret_cast<void*>(&VirtualMachineStack.top().Data)));
                });
        }
    }

    template <class T>
    void SaveValueFromStackToVariableFromPointerFromOffsetFromStackForType(const VirtualCodeCommand& VirtualCodeCommandToExecute, const VirtualCommandDataType VirtualCommandDataTypeParam, const UnsignedInt Offset)
    {
        if (VirtualMachineStack.top().Type == VirtualCodeCommandToExecute.Type)
        {
            if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeParam)
                *reinterpret_cast<T*>(VirtualMachineMemory.data() + Offset) = *static_cast<T*>(reinterpret_cast<void*>(&VirtualMachineStack.top().Data));
        }
        else
        {
            UnsignedInt TypeIndex = 0;
            if (VirtualMachineStack.top().Type == VirtualCommandDataTypeParam)
                boost::mpl::for_each<ValidTypes>([&]<typename T1>(T1 TypeArg)
                {
                    if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeList[TypeIndex++])
                        *reinterpret_cast<T1*>(VirtualMachineMemory.data() + Offset) = static_cast<T1>(*static_cast<T*>(reinterpret_cast<void*>(&VirtualMachineStack.top().Data)));
                });
        }
    }

    void GeneralSaveValueFromStackToVariableFromOffsetFromStack(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        const UnsignedInt Offset = *static_cast<UnsignedInt*>(reinterpret_cast<void*>(&VirtualMachineStack.top().Data));
        VirtualMachineStack.pop();

        UnsignedInt TypeIndex = 0;
        if (VirtualCodeCommandToExecute.Level == LOCAL_LEVEL)
            boost::mpl::for_each<ValidTypes>([&]<typename T>(T TypeArg) { SaveValueFromStackToVariableFromOffsetFromStackForType<T>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++], Offset); } );
        else
        if (VirtualCodeCommandToExecute.Level == GLOBAL_LEVEL)
            boost::mpl::for_each<ValidTypes>([&]<typename T>(T TypeArg) { SaveValueFromStackToVariableFromPointerFromOffsetFromStackForType<T>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++], Offset); } );
        else
        if (VirtualCodeCommandToExecute.Level == CLASS_LEVEL)
        {
        }

        VirtualMachineStack.pop();
    }

private:

    void LoadPointerOfVariableToStack(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        VirtualMachineStack.emplace(VirtualCodeCommandToExecute.Type, VirtualCodeCommandToExecute.TargetAddress);
    }

private:

    void GetMemoryForFunctionOnStack(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        StackStartAddressESP -= static_cast<UnsignedInt>(VirtualCodeCommandToExecute.TargetAddress);
        VirtualMachineStack.emplace(VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE, StackActualAddressEBP);
        StackActualAddressEBP = StackStartAddressESP;
    }

    void CallFunction(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        VirtualMachineStack.emplace(VirtualCommandDataType::UNSIGNED_LONG_INT_TYPE, RunningVirtualCommandIndex);
        RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress - 1;
    }

    void FreeMemoryForFunctionOnStack(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        StackActualAddressEBP = VirtualMachineStack.top().Data.UnsignedLongIntType;
        VirtualMachineStack.pop();
        StackStartAddressESP += static_cast<UnsignedInt>(VirtualCodeCommandToExecute.Value);
    }

    void ReturnFromFunction(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        if (VirtualMachineStack.empty() == false)
        {
            RunningVirtualCommandIndex = VirtualMachineStack.top().Data.UnsignedLongIntType;
            VirtualMachineStack.pop();
        }
        else
        {
            cout << "END OF PROGRAM" << endl;
            ProgramRunning = false;
        }
    }

    template <class T>
    void JumpConditionalForType(const VirtualCodeCommand& VirtualCodeCommandToExecute, const VirtualCommandDataType VirtualCommandDataTypeParam)
    {
        if (VirtualMachineStack.top().Type == VirtualCommandDataTypeParam)
            if (*static_cast<T*>(reinterpret_cast<void*>(&VirtualMachineStack.top().Data)) == 1)
                RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress - 1;
    }

    void JumpConditional(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        const auto R1 = VirtualMachineStack.top();
        UnsignedInt TypeIndex = 0;
        boost::mpl::for_each<ValidTypes>([&]<typename T>(T TypeArg) { JumpConditionalForType<T>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );

        VirtualMachineStack.pop();
    }

    void Jump(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        RunningVirtualCommandIndex = VirtualCodeCommandToExecute.TargetAddress - 1;
    }

private:

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

    // template <class T>
    // void LoadConstantFunctionParameterFromStackToFunctionParametersListForType(const VirtualCodeCommand& VirtualCodeCommandToExecute, const VirtualCommandDataType VirtualCommandDataTypeParam)
    // {
    //     if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeParam)
    //         VirtualMachineFunctionParametersList.emplace_back(VirtualCodeCommandToExecute.Type, static_cast<T>(*static_cast<T*>(reinterpret_cast<void*>(&VirtualMachineStack.top().Data))));
    // }

    // void LoadConstantFunctionParameterFromStackToFunctionParametersList(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    // {
    //     UnsignedInt TypeIndex = 0;
    //     boost::mpl::for_each<ValidTypes>([&]<typename T>(T TypeArg) { LoadConstantFunctionParameterFromStackToFunctionParametersListForType<T>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
    // }

    //LOAD
    void GeneralLoadVariableToFunctionParametersList(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        if (VirtualCodeCommandToExecute.Kind != CONSTANT_TYPE)
        {
            if (VirtualCodeCommandToExecute.TargetAddress > 0)
            {
                UnsignedInt TypeIndex = 0;
                if (VirtualCodeCommandToExecute.Level == LOCAL_LEVEL)
                    boost::mpl::for_each<ValidTypes>([&]<typename T>(T TypeArg) { LoadLocalVariableToFunctionParametersListForType<T>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
                else
                if (VirtualCodeCommandToExecute.Level == GLOBAL_LEVEL)
                    boost::mpl::for_each<ValidTypes>([&]<typename T>(T TypeArg) { LoadGlobalVariableFromPointerToFunctionParametersListForType<T>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
                else
                if (VirtualCodeCommandToExecute.Level == CLASS_LEVEL)
                {
                }
            }
            else
            {
                VirtualMachineFunctionParametersList.emplace_back(VirtualMachineStack.top());
                //if TargetAddress is 0 to laduj ze stackvvalue

                //UnsignedInt TypeIndex = 0;
                //boost::mpl::for_each<ValidTypes>([&]<typename T>(T TypeArg) { LoadConstantFunctionParameterFromStackToFunctionParametersListForType<T>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );

                //if (VirtualCodeCommandToExecute.Level == LOCAL_LEVEL)
                //    boost::mpl::for_each<ValidTypes>([&]<typename T>(T TypeArg) { LoadConstantFunctionParameterToFunctionParametersListForType<T>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
                // else
                // if (VirtualCodeCommandToExecute.Level == GLOBAL_LEVEL)
                //     boost::mpl::for_each<ValidTypes>([&]<typename T>(T TypeArg) { LoadGlobalVariableFromPointerToFunctionParametersListForType<T>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
                // else
                // if (VirtualCodeCommandToExecute.Level == CLASS_LEVEL)
                // {
                // }

                // UnsignedInt TypeIndex = 0;
                // if (VirtualCodeCommandToExecute.Level == LOCAL_LEVEL)
                //     boost::mpl::for_each<ValidTypes>([&]<typename T>(T TypeArg) { LoadConstantFunctionParameterToFunctionParametersListForType<T>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
                // else
                // if (VirtualCodeCommandToExecute.Level == GLOBAL_LEVEL)
                //     boost::mpl::for_each<ValidTypes>([&]<typename T>(T TypeArg) { LoadGlobalVariableFromPointerToFunctionParametersListForType<T>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
                // else
                // if (VirtualCodeCommandToExecute.Level == CLASS_LEVEL)
                // {
                // }
            }
        }
        else
            LoadConstantFunctionParameterToFunctionParametersList(VirtualCodeCommandToExecute);
    }

    template <class T>
    void LoadConstantFunctionParameterToFunctionParametersListForType(const VirtualCodeCommand& VirtualCodeCommandToExecute, const VirtualCommandDataType VirtualCommandDataTypeParam)
    {
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeParam)
            VirtualMachineFunctionParametersList.emplace_back(VirtualCodeCommandToExecute.Type, static_cast<T>(VirtualCodeCommandToExecute.Value));
    }

    void LoadConstantFunctionParameterToFunctionParametersList(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        UnsignedInt TypeIndex = 0;
        boost::mpl::for_each<ValidTypes>([&]<typename T>(T TypeArg) { LoadConstantFunctionParameterToFunctionParametersListForType<T>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
    }








    template <class T>
    void SaveValueFromParametersListToVariableForType(const VirtualCodeCommand& VirtualCodeCommandToExecute, const VirtualCommandDataType VirtualCommandDataTypeParam)
    {
        if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeParam)
            *reinterpret_cast<T*>(VirtualMachineMemory.data() + StackActualAddressEBP + VirtualCodeCommandToExecute.TargetAddress) = *static_cast<T*>(reinterpret_cast<void*>(&VirtualMachineFunctionParametersList.front().Data));
    }

    // template <class T>
    // void SaveValueFromParametersListToVariableFromPointerForType(const VirtualCodeCommand& VirtualCodeCommandToExecute, const VirtualCommandDataType VirtualCommandDataTypeParam)
    // {
    //     if (VirtualCodeCommandToExecute.Type == VirtualCommandDataTypeParam)
    //         *reinterpret_cast<T*>(VirtualMachineMemory.data() + VirtualCodeCommandToExecute.TargetAddress) = *static_cast<T*>(reinterpret_cast<void*>(&VirtualMachineFunctionParametersList.front().Data));
    // }

    //LDP
    void GeneralSaveValueFromParametersListToVariable(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        UnsignedInt TypeIndex = 0;
        //if (VirtualCodeCommandToExecute.Level == LOCAL_LEVEL)
        boost::mpl::for_each<ValidTypes>([&]<typename T>(T TypeArg) { SaveValueFromParametersListToVariableForType<T>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
        // else
        // if (VirtualCodeCommandToExecute.Level == GLOBAL_LEVEL)
        //     boost::mpl::for_each<ValidTypes>([&]<typename T>(T TypeArg) { SaveValueFromParametersListToVariableFromPointerForType<T>(VirtualCodeCommandToExecute, VirtualCommandDataTypeList[TypeIndex++]); } );
        // else
        // if (VirtualCodeCommandToExecute.Level == CLASS_LEVEL)
        // {
        // }

        VirtualMachineFunctionParametersList.pop_front();
    }

    void ExecuteVirtualCommand(const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        try
        {
            switch (VirtualCodeCommandToExecute.CommandName)
            {
                case VirtualCommandName::LDC: PrintDebugInfo("LDC", VirtualCodeCommandToExecute); LoadConstantToStack(VirtualCodeCommandToExecute); break;

                case VirtualCommandName::LDV: PrintDebugInfo("LDV", VirtualCodeCommandToExecute); GeneralLoadVariableToStack(VirtualCodeCommandToExecute); break;
                case VirtualCommandName::LDVFPTR: PrintDebugInfo("LDVFPTR", VirtualCodeCommandToExecute); GeneralLoadVariableFromOffsetFromStackToStack(VirtualCodeCommandToExecute); break;

                case VirtualCommandName::SVV: PrintDebugInfo("SVV", VirtualCodeCommandToExecute); GeneralSaveValueFromStackToVariable(VirtualCodeCommandToExecute); break;
                case VirtualCommandName::SVVFPTR: PrintDebugInfo("SVVFPTR", VirtualCodeCommandToExecute); GeneralSaveValueFromStackToVariableFromOffsetFromStack(VirtualCodeCommandToExecute); break;

                case VirtualCommandName::LDPTROFV: PrintDebugInfo("LDPTROFV", VirtualCodeCommandToExecute); LoadPointerOfVariableToStack(VirtualCodeCommandToExecute); break;

                case VirtualCommandName::OPR: PrintDebugInfo("OPR", VirtualCodeCommandToExecute); PerformOperation(VirtualCodeCommandToExecute); break;

                case VirtualCommandName::LOAD: PrintDebugInfo("LOAD", VirtualCodeCommandToExecute); GeneralLoadVariableToFunctionParametersList(VirtualCodeCommandToExecute); break;
                case VirtualCommandName::LDP: PrintDebugInfo("LDP", VirtualCodeCommandToExecute); GeneralSaveValueFromParametersListToVariable(VirtualCodeCommandToExecute); break;

                case VirtualCommandName::GET: PrintDebugInfo("GET", VirtualCodeCommandToExecute); GetMemoryForFunctionOnStack(VirtualCodeCommandToExecute); break;
                case VirtualCommandName::CALL: PrintDebugInfo("CALL", VirtualCodeCommandToExecute); CallFunction(VirtualCodeCommandToExecute); break;
                case VirtualCommandName::VIRTRET: PrintDebugInfo("VIRTRET", VirtualCodeCommandToExecute); FreeMemoryForFunctionOnStack(VirtualCodeCommandToExecute); break;

                case VirtualCommandName::JMP: PrintDebugInfo("JMP", VirtualCodeCommandToExecute); Jump(VirtualCodeCommandToExecute); break;
                case VirtualCommandName::JCON: PrintDebugInfo("JCON", VirtualCodeCommandToExecute); JumpConditional(VirtualCodeCommandToExecute); break;

                case VirtualCommandName::END: PrintDebugInfo("END", VirtualCodeCommandToExecute); ProgramRunning = false; break;

                case VirtualCommandName::PRINT: PrintMemory(); break;

                case VirtualCommandName::STACK0: PrintDebugInfo("STACK0", VirtualCodeCommandToExecute); break;

                default: PrintMemory(); throw runtime_error("Unknown instruction");
            }
        }
        CATCH_COUT("instruction error")
    }

public:

    void LoadProgram(const std::vector<VirtualCodeCommand>& ProgramCode)
    {
        ProgramCodeCommandsList = ProgramCode;
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

private:

    void PrintDebugInfo(const char* CommandName, const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        if constexpr(PrintExecutedCommandData == true)
            PrintCommandData(CommandName, VirtualCodeCommandToExecute);

        if constexpr(PrintExecutedStackData == true)
            PrintTopOfStack();

        cout << endl;
    }

    static void PrintCommandData(const char* CommandName, const VirtualCodeCommand& VirtualCodeCommandToExecute)
    {
        cout << VirtualCodeCommandToExecute.LabelAddress << ": " << CommandName << " OPR = " << static_cast<SignedInt>(VirtualCodeCommandToExecute.Operation) << " Type = " << static_cast<SignedInt>(VirtualCodeCommandToExecute.Type) << " Value = " << VirtualCodeCommandToExecute.Value << " TargetAddress = " << VirtualCodeCommandToExecute.TargetAddress << " Kind = " << VirtualCodeCommandToExecute.Kind << " Level = " << VirtualCodeCommandToExecute.Level << " Index = " << VirtualCodeCommandToExecute.Index << " Inside = " << VirtualCodeCommandToExecute.Inside << " ";
    }

    template <class T>
    void PrintTopOfStackProperType(const VirtualCommandDataType VirtualCommandDataTypeParam)
    {
         if (VirtualMachineStack.empty() == false && VirtualMachineStack.top().Type == VirtualCommandDataTypeParam)
             cout << "Stack Size = " << VirtualMachineStack.size() << " Top Stack Type = " << static_cast<SignedInt>(VirtualMachineStack.top().Type) << " Top Stack Value = " << static_cast<T>(*reinterpret_cast<T*>(&VirtualMachineStack.top().Data));
    }

    void PrintTopOfStack()
    {
        UnsignedInt TypeIndex = 0;
        boost::mpl::for_each<ValidTypes>([&]<typename T>(T TypeArg) { PrintTopOfStackProperType<T>(VirtualCommandDataTypeList[TypeIndex++]); });
    }

public:

    void PrintMemory() const
    {
        for (UnsignedInt ByteIndex = 0; ByteIndex < VirtualMachineMemory.size(); ByteIndex++)
            cout << "Memory[" << to_string(ByteIndex) << "] = " << to_string(VirtualMachineMemory[ByteIndex]) << endl;
    }

    void PrintStack()
    {
        auto TemporaryVirtualMachineStack = VirtualMachineStack;
        cout << "STACK START (top to bottom): " << endl;
        while (!TemporaryVirtualMachineStack.empty())
        {
            PrintTopOfStack();
            TemporaryVirtualMachineStack.pop();
        }
        cout << "STACK END" << endl;
    }

    explicit VirtualMachine(const UnsignedInt MemorySizeParam, const UnsignedInt StartVirtualCommandAddress) : MemorySize(MemorySizeParam), VirtualMachineMemory(MemorySize), RunningVirtualCommandIndex(StartVirtualCommandAddress), StackStartAddressESP(MemorySizeParam), StackActualAddressEBP(MemorySizeParam)
    {
    }
};

void ExecuteProgramOnVirtualMachine(const std::vector<VirtualCodeCommand>& ProgramToExecute, const UnsignedInt StartVirtualCommandAddress)
{
    VirtualMachine VirtualMachinObject(136, StartVirtualCommandAddress);

    VirtualMachinObject.LoadProgram(ProgramToExecute);
    VirtualMachinObject.RunProgram();
    VirtualMachinObject.PrintStack();
    VirtualMachinObject.PrintMemory();
}

std::vector<VirtualCodeCommand> CreateSampleProgram()
{
    return
    {
        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 5, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::SVV, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, LOCAL_LEVEL, 0, 0, 0, 15 },

        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 10, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 10, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 20, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::OPR, VirtualCommandOperationType::ADD, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::OPR, VirtualCommandOperationType::MUL, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::SVV, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, LOCAL_LEVEL, 0, 0, 0, 20 },

        { VirtualCommandName::PRINT, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, 0, 0, 0, 0, 0 },

        { VirtualCommandName::LDV, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, LOCAL_LEVEL, 0, 0, 0, 15 },
        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 1, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::OPR, VirtualCommandOperationType::ADD, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::SVV, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, LOCAL_LEVEL, 0, 0, 0, 15 },

        { VirtualCommandName::LDV, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, LOCAL_LEVEL, 0, 0, 0, 15 },
        { VirtualCommandName::LDC, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 10, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::OPR, VirtualCommandOperationType::NEQ, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, 0, 0, 0, 0, 0 },
        { VirtualCommandName::JCON, VirtualCommandOperationType::NOP, VirtualCommandDataType::SIGNED_INT_TYPE, 0, 0, 0, 0, 0, 0, 7 + 1 },
    };
}

int TestVirtualMachine()
{
    VirtualMachine VirtualMachinObject(32, 0);

    const auto Program = CreateSampleProgram();

    VirtualMachinObject.LoadProgram(Program);
    VirtualMachinObject.RunProgram();
    VirtualMachinObject.PrintStack();
    VirtualMachinObject.PrintMemory();

    return 0;
}
