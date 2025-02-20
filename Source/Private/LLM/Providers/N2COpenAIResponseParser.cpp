// Copyright ProtoSpatial. All Rights Reserved.

#include "LLM/Providers/N2COpenAIResponseParser.h"
#include "Utils/N2CLogger.h"
#include "Serialization/JsonSerializer.h"

bool UN2COpenAIResponseParser::ParseLLMResponse(
    const FString& InJson,
    FN2CTranslationResponse& OutResponse)
{
    // Parse JSON string
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(InJson);

    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        FN2CLogger::Get().LogError(
            FString::Printf(TEXT("Failed to parse OpenAI response JSON: %s"), *InJson),
            TEXT("OpenAIResponseParser")
        );
        return false;
    }

    // Check for OpenAI error response
    FString ErrorMessage;
    if (JsonObject->HasField(TEXT("error")))
    {
        if (HandleOpenAIError(JsonObject, ErrorMessage))
        {
            FN2CLogger::Get().LogError(ErrorMessage, TEXT("OpenAIResponseParser"));
        }
        return false;
    }

    // Extract message content from OpenAI format
    FString MessageContent;
    if (!ExtractMessageContent(JsonObject, MessageContent))
    {
        FN2CLogger::Get().LogError(TEXT("Failed to extract message content from OpenAI response"), TEXT("OpenAIResponseParser"));
        return false;
    }

    // Extract usage information if available
    const TSharedPtr<FJsonObject> UsageObject = JsonObject->GetObjectField(TEXT("usage"));
    if (UsageObject.IsValid())
    {
        int32 PromptTokens = 0;
        int32 CompletionTokens = 0;
        UsageObject->TryGetNumberField(TEXT("prompt_tokens"), PromptTokens);
        UsageObject->TryGetNumberField(TEXT("completion_tokens"), CompletionTokens);
        
        OutResponse.Usage.InputTokens = PromptTokens;
        OutResponse.Usage.OutputTokens = CompletionTokens;

        FN2CLogger::Get().Log(FString::Printf(TEXT("LLM Token Usage - Input: %d Output: %d"), PromptTokens, CompletionTokens), EN2CLogSeverity::Info);
    }

    FN2CLogger::Get().Log(FString::Printf(TEXT("LLM Response Message Content: %s"), *MessageContent), EN2CLogSeverity::Debug);

    // Parse the extracted content as our expected JSON format
    return Super::ParseLLMResponse(MessageContent, OutResponse);
}

bool UN2COpenAIResponseParser::ExtractMessageContent(
    const TSharedPtr<FJsonObject>& JsonObject,
    FString& OutContent)
{
    // Get choices array
    const TArray<TSharedPtr<FJsonValue>>* ChoicesArray;
    if (!JsonObject->TryGetArrayField(TEXT("choices"), ChoicesArray) || ChoicesArray->Num() == 0)
    {
        return false;
    }

    // Get first choice
    const TSharedPtr<FJsonObject> ChoiceObject = (*ChoicesArray)[0]->AsObject();
    if (!ChoiceObject.IsValid())
    {
        return false;
    }

    // Get message object
    const TSharedPtr<FJsonObject> MessageObject = ChoiceObject->GetObjectField(TEXT("message"));
    if (!MessageObject.IsValid())
    {
        return false;
    }

    // Get content string
    FString RawContent;
    if (!MessageObject->TryGetStringField(TEXT("content"), RawContent))
    {
        return false;
    }

    // Check if content is wrapped in ```json markers and remove them
    if (RawContent.StartsWith(TEXT("```json")) && RawContent.EndsWith(TEXT("```")))
    {
        // Remove the ```json prefix and ``` suffix
        OutContent = RawContent.RightChop(7); // Skip past "```json"
        OutContent = OutContent.LeftChop(3);  // Remove trailing "```"
        OutContent = OutContent.TrimStartAndEnd(); // Remove any extra whitespace
        
        FN2CLogger::Get().Log(TEXT("Stripped JSON markers from OpenAI response"), EN2CLogSeverity::Debug);
    }
    else
    {
        OutContent = RawContent;
    }

    return true;
}

bool UN2COpenAIResponseParser::HandleOpenAIError(
    const TSharedPtr<FJsonObject>& JsonObject,
    FString& OutErrorMessage)
{
    const TSharedPtr<FJsonObject> ErrorObject = JsonObject->GetObjectField(TEXT("error"));
    if (!ErrorObject.IsValid())
    {
        OutErrorMessage = TEXT("Unknown OpenAI error");
        return true;
    }

    FString ErrorType, ErrorMessage;
    ErrorObject->TryGetStringField(TEXT("type"), ErrorType);
    ErrorObject->TryGetStringField(TEXT("message"), ErrorMessage);

    if (ErrorType.Contains(TEXT("rate_limit")))
    {
        OutErrorMessage = TEXT("OpenAI API rate limit exceeded");
    }
    else if (ErrorType.Contains(TEXT("invalid_request_error")))
    {
        OutErrorMessage = FString::Printf(TEXT("Invalid request: %s"), *ErrorMessage);
    }
    else if (ErrorType.Contains(TEXT("authentication")))
    {
        OutErrorMessage = TEXT("OpenAI API authentication failed");
    }
    else
    {
        OutErrorMessage = FString::Printf(TEXT("OpenAI API error: %s - %s"), *ErrorType, *ErrorMessage);
    }

    return true;
}
