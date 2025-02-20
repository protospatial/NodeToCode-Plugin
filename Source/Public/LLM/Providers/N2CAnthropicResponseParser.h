// Copyright ProtoSpatial. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LLM/N2CResponseParserBase.h"
#include "N2CAnthropicResponseParser.generated.h"

/**
 * @class UN2CAnthropicResponseParser
 * @brief Parser for Anthropic's Claude API responses
 */
UCLASS()
class NODETOCODE_API UN2CAnthropicResponseParser : public UN2CResponseParserBase
{
    GENERATED_BODY()

public:
    /** Parse Anthropic-specific JSON response */
    virtual bool ParseLLMResponse(
        const FString& InJson,
        FN2CTranslationResponse& OutResponse) override;

protected:
    /** Extract message content from Anthropic response format */
    bool ExtractMessageContent(
        const TSharedPtr<FJsonObject>& JsonObject,
        FString& OutContent);

    /** Handle Anthropic-specific error responses */
    bool HandleAnthropicError(
        const TSharedPtr<FJsonObject>& JsonObject,
        FString& OutErrorMessage);
};
