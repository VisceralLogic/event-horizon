#pragma once

#include <memory>

#include "ehobject.h""

class AI;

class Type : public EHObject {
public:
	static void registerFromDictionary(const json& dictionary);
	void finalize();
	shared_ptr<AI> newInstance();
};