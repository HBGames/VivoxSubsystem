// Copyright Epic Games, Inc. All Rights Reserved.


#include "VoiceChatResult.h"
#include "VivoxSubsystem/Public/VoiceChatResult.h"

FVivoxChatResult::FVivoxChatResult(FVoiceChatResult VoiceChatResult)
	: ResultCode(static_cast<EVivoxChatResult>(VoiceChatResult.ResultCode))
	, ErrorCode(VoiceChatResult.ErrorCode)
	, ErrorNum(VoiceChatResult.ErrorNum)
	, ErrorDesc(VoiceChatResult.ErrorDesc)
{}
