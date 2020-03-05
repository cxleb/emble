#include "ir/module.h"

namespace ir {
	void module::addFunction(function* func) {
		this->functions.push_back(func);
	}

	void module::print() {
		for (function* func : this->functions) {
			func->print();
		}
	}
}