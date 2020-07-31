// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "VoiceChatResult.generated.h"

UENUM(BlueprintType)
enum class EVivoxChatResult : uint8
{
	// The operation succeeded
	Success = 0,

    // Common state errors
    InvalidState,
    NotInitialized,
    NotConnected,
    NotLoggedIn,
    NotPermitted,
    Throttled,

    // Common argument errors
    InvalidArgument,
    CredentialsInvalid,
    CredentialsExpired,

    // Common connection errors
    ClientTimeout,
    ServerTimeout,
    DnsFailure,
    ConnectionFailure,

    // Error does not map to any common categories
    ImplementationError
};

USTRUCT(BlueprintType)
struct VIVOXSUBSYSTEM_API FVivoxChatResult
{
	GENERATED_BODY()

	/** Success, or an error category */
	UPROPERTY(BlueprintReadWrite)
	EVivoxChatResult ResultCode;
	/** If we failed, the code for the error */
	UPROPERTY(BlueprintReadWrite)
	FString ErrorCode;
	/** If we failed, a numeric error from the implementation */
	UPROPERTY(BlueprintReadWrite)
	int ErrorNum = 0;
	/** If we failed, more details about the error condition */
	UPROPERTY(BlueprintReadWrite)
	FString ErrorDesc;

	bool IsSuccess() const { return ResultCode == EVivoxChatResult::Success; }
	
	bool operator==(const FVivoxChatResult& Other) const
	{
		return ResultCode == Other.ResultCode && ErrorCode == Other.ErrorCode;
	}

	bool operator!=(const FVivoxChatResult& Other) const
	{
		return !operator==(Other);
	}

	FVivoxChatResult() : ResultCode() {};
	FVivoxChatResult(FVoiceChatResult VoiceChatResult);
};