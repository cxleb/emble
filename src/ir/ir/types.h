#pragma once
#include <string>

namespace ir {
	enum Types{
		Invalid,

		UInt8,
		UInt16,
		UInt32,
		UInt64,

		Int8,
		Int16,
		Int32,
		Int64,

		VPointer,
		Void,
		Function,
		Instruction
	};
}