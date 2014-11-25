#pragma once


class ITextContext
{
public:
    virtual ~ITextContext(){}
    virtual std::wstring GetText(const std::wstring &key)=0;
};

