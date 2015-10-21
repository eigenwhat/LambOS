#pragma once

#include <stddef.h>
#include <stdint.h>

class Object
{
public:
	Object() : _referenceCount(1) {}
	virtual ~Object() {}
	void retain() const { ++_referenceCount; }
	void release() const {
		--_referenceCount;

		if(_referenceCount == 0) {
			delete (Object *)this;
		}
	}

	virtual bool isEqualTo(Object *o) { return this == o; }
private:
	mutable int _referenceCount;
};
