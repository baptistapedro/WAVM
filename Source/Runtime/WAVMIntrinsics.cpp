#include "Core/Core.h"
#include "Inline/Floats.h"
#include "Intrinsics.h"
#include "RuntimePrivate.h"

#include <math.h>

namespace Runtime
{
	template<typename Float>
	Float quietNaN(Float value)
	{
		Floats::FloatComponents<Float> components;
		components.value = value;
		components.bits.significand |= typename Floats::FloatComponents<Float>::Bits(1) << (Floats::FloatComponents<Float>::numSignificandBits - 1);
		return components.value;
	}

	template<typename Float>
	Float floatMin(Float left,Float right)
	{
		// If either operand is a NaN, convert it to a quiet NaN and return it.
		if(left != left) { return quietNaN(left); }
		else if(right != right) { return quietNaN(right); }
		// If either operand is less than the other, return it.
		else if(left < right) { return left; }
		else if(right < left) { return right; }
		else
		{
			// Finally, if the operands are apparently equal, compare their integer values to distinguish -0.0 from +0.0
			Floats::FloatComponents<Float> leftComponents;
			leftComponents.value = left;
			Floats::FloatComponents<Float> rightComponents;
			rightComponents.value = right;
			return leftComponents.bitcastInt < rightComponents.bitcastInt ? right : left;
		}
	}
	
	template<typename Float>
	Float floatMax(Float left,Float right)
	{
		// If either operand is a NaN, convert it to a quiet NaN and return it.
		if(left != left) { return quietNaN(left); }
		else if(right != right) { return quietNaN(right); }
		// If either operand is less than the other, return it.
		else if(left > right) { return left; }
		else if(right > left) { return right; }
		else
		{
			// Finally, if the operands are apparently equal, compare their integer values to distinguish -0.0 from +0.0
			Floats::FloatComponents<Float> leftComponents;
			leftComponents.value = left;
			Floats::FloatComponents<Float> rightComponents;
			rightComponents.value = right;
			return leftComponents.bitcastInt > rightComponents.bitcastInt ? right : left;
		}
	}

	template<typename Float>
	Float floatCeil(Float value)
	{
		if(value != value) { return quietNaN(value); }
		else { return ceil(value); }
	}
	
	template<typename Float>
	Float floatFloor(Float value)
	{
		if(value != value) { return quietNaN(value); }
		else { return floor(value); }
	}
	
	template<typename Float>
	Float floatTrunc(Float value)
	{
		if(value != value) { return quietNaN(value); }
		else { return trunc(value); }
	}
	
	template<typename Float>
	Float floatNearest(Float value)
	{
		if(value != value) { return quietNaN(value); }
		else { return nearbyint(value); }
	}

	DEFINE_INTRINSIC_FUNCTION2(wavmIntrinsics,floatMin,floatMin,f32,f32,left,f32,right) { return floatMin(left,right); }
	DEFINE_INTRINSIC_FUNCTION2(wavmIntrinsics,floatMin,floatMin,f64,f64,left,f64,right) { return floatMin(left,right); }
	DEFINE_INTRINSIC_FUNCTION2(wavmIntrinsics,floatMax,floatMax,f32,f32,left,f32,right) { return floatMax(left,right); }
	DEFINE_INTRINSIC_FUNCTION2(wavmIntrinsics,floatMax,floatMax,f64,f64,left,f64,right) { return floatMax(left,right); }

	DEFINE_INTRINSIC_FUNCTION1(wavmIntrinsics,floatCeil,floatCeil,f32,f32,value) { return floatCeil(value); }
	DEFINE_INTRINSIC_FUNCTION1(wavmIntrinsics,floatCeil,floatCeil,f64,f64,value) { return floatCeil(value); }
	DEFINE_INTRINSIC_FUNCTION1(wavmIntrinsics,floatFloor,floatFloor,f32,f32,value) { return floatFloor(value); }
	DEFINE_INTRINSIC_FUNCTION1(wavmIntrinsics,floatFloor,floatFloor,f64,f64,value) { return floatFloor(value); }
	DEFINE_INTRINSIC_FUNCTION1(wavmIntrinsics,floatTrunc,floatTrunc,f32,f32,value) { return floatTrunc(value); }
	DEFINE_INTRINSIC_FUNCTION1(wavmIntrinsics,floatTrunc,floatTrunc,f64,f64,value) { return floatTrunc(value); }
	DEFINE_INTRINSIC_FUNCTION1(wavmIntrinsics,floatNearest,floatNearest,f32,f32,value) { return floatNearest(value); }
	DEFINE_INTRINSIC_FUNCTION1(wavmIntrinsics,floatNearest,floatNearest,f64,f64,value) { return floatNearest(value); }

	template<typename Dest,typename Source,bool isMinInclusive>
	Dest floatToInt(Source sourceValue,Source minValue,Source maxValue)
	{
		if(sourceValue != sourceValue)
		{
			causeException(Exception::Cause::invalidFloatOperation);
		}
		else if(sourceValue >= maxValue || (isMinInclusive ? sourceValue <= minValue : sourceValue < minValue))
		{
			causeException(Exception::Cause::integerDivideByZeroOrIntegerOverflow);
		}
		return (Dest)sourceValue;
	}

	DEFINE_INTRINSIC_FUNCTION1(wavmIntrinsics,floatToSignedInt,floatToSignedInt,i32,f32,source) { return floatToInt<int32,float32,false>(source,(float32)INT32_MIN,-(float32)INT32_MIN); }
	DEFINE_INTRINSIC_FUNCTION1(wavmIntrinsics,floatToSignedInt,floatToSignedInt,i32,f64,source) { return floatToInt<int32,float64,false>(source,(float64)INT32_MIN,-(float64)INT32_MIN); }
	DEFINE_INTRINSIC_FUNCTION1(wavmIntrinsics,floatToSignedInt,floatToSignedInt,i64,f32,source) { return floatToInt<int64,float32,false>(source,(float32)INT64_MIN,-(float32)INT64_MIN); }
	DEFINE_INTRINSIC_FUNCTION1(wavmIntrinsics,floatToSignedInt,floatToSignedInt,i64,f64,source) { return floatToInt<int64,float64,false>(source,(float64)INT64_MIN,-(float64)INT64_MIN); }
	
	DEFINE_INTRINSIC_FUNCTION1(wavmIntrinsics,floatToUnsignedInt,floatToUnsignedInt,i32,f32,source) { return floatToInt<uint32,float32,true>(source,-1.0f,-2.0f * INT32_MIN); }
	DEFINE_INTRINSIC_FUNCTION1(wavmIntrinsics,floatToUnsignedInt,floatToUnsignedInt,i32,f64,source) { return floatToInt<uint32,float64,true>(source,-1.0,-2.0 * INT32_MIN); }
	DEFINE_INTRINSIC_FUNCTION1(wavmIntrinsics,floatToUnsignedInt,floatToUnsignedInt,i64,f32,source) { return floatToInt<uint64,float32,true>(source,-1.0f,-2.0f * INT64_MIN); }
	DEFINE_INTRINSIC_FUNCTION1(wavmIntrinsics,floatToUnsignedInt,floatToUnsignedInt,i64,f64,source) { return floatToInt<uint64,float64,true>(source,-1.0,-2.0 * INT64_MIN); }

	DEFINE_INTRINSIC_FUNCTION0(wavmIntrinsics,divideByZeroTrap,divideByZeroTrap,none)
	{
		causeException(Exception::Cause::integerDivideByZeroOrIntegerOverflow);
	}

	DEFINE_INTRINSIC_FUNCTION0(wavmIntrinsics,unreachableTrap,unreachableTrap,none)
	{
		causeException(Exception::Cause::reachedUnreachable);
	}

	DEFINE_INTRINSIC_FUNCTION3(wavmIntrinsics,indirectCallSignatureMismatch,indirectCallSignatureMismatch,none,i32,index,i64,expectedSignatureBits,i64,tableBits)
	{
		TableInstance* table = reinterpret_cast<TableInstance*>(tableBits);
		void* elementValue = table->baseAddress[index].value;
		const FunctionType* actualSignature = table->baseAddress[index].type;
		const FunctionType* expectedSignature = reinterpret_cast<const FunctionType*>((uintp)expectedSignatureBits);
		std::string ipDescription = "<unknown>";
		LLVMJIT::describeInstructionPointer(reinterpret_cast<uintp>(elementValue),ipDescription);
		Log::printf(Log::Category::debug,"call_indirect signature mismatch: expected %s at index %u but got %s (%s)\n",
			asString(expectedSignature).c_str(),
			index,
			actualSignature ? asString(actualSignature).c_str() : "nullptr",
			ipDescription.c_str()
			);
		causeException(elementValue == nullptr ? Exception::Cause::undefinedTableElement : Exception::Cause::indirectCallSignatureMismatch);
	}

	DEFINE_INTRINSIC_FUNCTION0(wavmIntrinsics,indirectCallIndexOutOfBounds,indirectCallIndexOutOfBounds,none)
	{
		causeException(Exception::Cause::undefinedTableElement);
	}

	DEFINE_INTRINSIC_FUNCTION2(wavmIntrinsics,_growMemory,growMemory,i32,i32,deltaPages,i64,memoryBits)
	{
		MemoryInstance* memory = reinterpret_cast<MemoryInstance*>(memoryBits);
		assert(memory);
		if(memory->numPages + (uintp)deltaPages > 65536) { return -1; }
		else { return (int32)growMemory(memory,(uintp)deltaPages); }
	}
	
	DEFINE_INTRINSIC_FUNCTION1(wavmIntrinsics,_currentMemory,currentMemory,i32,i64,memoryBits)
	{
		MemoryInstance* memory = reinterpret_cast<MemoryInstance*>(memoryBits);
		assert(memory);
		if(memory->numPages > 65536) { return -1; }
		else { return (uint32)getMemoryNumPages(memory); }
	}

	uintp indentLevel = 0;

	DEFINE_INTRINSIC_FUNCTION1(wavmIntrinsics,debugEnterFunction,debugEnterFunction,none,i64,functionInstanceBits)
	{
		FunctionInstance* function = reinterpret_cast<FunctionInstance*>(functionInstanceBits);
		Log::printf(Log::Category::debug,"ENTER: %s\n",function->debugName.c_str());
		++indentLevel;
	}
	
	DEFINE_INTRINSIC_FUNCTION1(wavmIntrinsics,debugExitFunction,debugExitFunction,none,i64,functionInstanceBits)
	{
		FunctionInstance* function = reinterpret_cast<FunctionInstance*>(functionInstanceBits);
		--indentLevel;
		Log::printf(Log::Category::debug,"EXIT:  %s\n",function->debugName.c_str());
	}
	
	DEFINE_INTRINSIC_FUNCTION0(wavmIntrinsics,debugBreak,debugBreak,none)
	{
		Log::printf(Log::Category::debug,"================== wavmIntrinsics.debugBreak\n");
	}

	void initWAVMIntrinsics()
	{
	}
}
