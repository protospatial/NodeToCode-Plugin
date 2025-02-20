// Copyright ProtoSpatial. All Rights Reserved.

#pragma once

#include "N2CSyntaxDefinition.h"

class FN2CCPPSyntaxDefinition : public FN2CSyntaxDefinition
{
public:
    FN2CCPPSyntaxDefinition();

    virtual const TArray<FString>& GetKeywords() const override { return Keywords; }
    virtual const TArray<FString>& GetOperators() const override { return Operators; }
    virtual void GetCommentDelimiters(FString& OutLineComment, FString& OutBlockCommentStart, FString& OutBlockCommentEnd) const override;
    virtual const TArray<TCHAR>& GetStringDelimiters() const override { return StringDelimiters; }
    virtual const TArray<FString>& GetParentheses() const override { return Parentheses; }
    virtual const TArray<FString>& GetCurlyBraces() const override { return CurlyBraces; }
    virtual const TArray<FString>& GetSquareBrackets() const override { return SquareBrackets; }
    virtual EN2CCodeLanguage GetLanguage() const override { return EN2CCodeLanguage::Cpp; }
};
