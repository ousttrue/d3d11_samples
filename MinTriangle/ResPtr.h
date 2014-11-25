#pragma once


template<class T> 
class ResPtr 
{
    T*	mPtr;

public:
    typedef ResPtr<T>	self_t;

    ResPtr() : mPtr(NULL){ }
    ResPtr(const ResPtr &inPtr)
		: mPtr(NULL)
    {
        *this=inPtr;
    }

	~ResPtr() {
		if (mPtr){
			mPtr->Release();
			mPtr = nullptr;
		}
	}

    operator T*(void) const { return mPtr; }
    T** operator&(void) { return &mPtr; }
    const T** operator&(void)const { return &mPtr; }
    bool operator!(void) const { return !mPtr; }

    self_t& operator=(T* inPtr)  { Set(inPtr); return *this; }
    self_t& operator=(const self_t& inPtr) { Set(inPtr); return *this; }

    void Set(const self_t& inPtr) {
        Set(inPtr.mPtr);
    }

    void Set(T* inPtr) {
        if(inPtr){
            inPtr->AddRef();
        }
        if (mPtr){
            mPtr->Release();
        }
        mPtr = inPtr;
    }

    T* operator->() const { return mPtr; }
    T* Get() const { return mPtr; }
};

