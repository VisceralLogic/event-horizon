#pragma once

#include "ehobject.h""

class Type : public EHObject {
public:
	static void registerFromDictionary(const json& dictionary);
	void finalize();
};