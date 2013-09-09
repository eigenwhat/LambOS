#pragma once

class Object
{
public:
	Object() : _referenceCount(0) {}
	virtual ~Object() {}
	void retain() const { ++_referenceCount; }
	void release() const {
		--_referenceCount;

		if(_referenceCount == 0) {
			delete (Object *)this;
		}
	}
private:
	mutable int _referenceCount;
};
