// Copyright Epic Games, Inc. All Rights Reserved.

#include "VivoxSubsystem.h"
#include "Online.h"
#include "Misc/MessageDialog.h"
#if PLATFORM_PS4
#pragma comment (lib, "SceAudioIn_stub_weak")
#endif // PLATFORM_PS4

DEFINE_LOG_CATEGORY_STATIC(LogVivoxSubsystem, Log, All);

#define LOCTEXT_NAMESPACE "FVivoxSubsystemModule"

void FVivoxSubsystemModule::StartupModule() {}

void FVivoxSubsystemModule::ShutdownModule() {}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FVivoxSubsystemModule, VivoxSubsystem)

bool AreVivoxVoiceChatValuesSet()
{
	bool CheckBool = false;
	GConfig->GetBool(TEXT("VoiceChat.Vivox"), TEXT("bEnabled"), CheckBool, GEngineIni);
	if (!CheckBool) return false;

	FString CheckString;
	GConfig->GetString(TEXT("VoiceChat.Vivox"), TEXT("ServerUrl"), CheckString, GEngineIni);
	if (CheckString.IsEmpty()) return false;
	GConfig->GetString(TEXT("VoiceChat.Vivox"), TEXT("Domain"), CheckString, GEngineIni);
	if (CheckString.IsEmpty()) return false;
	GConfig->GetString(TEXT("VoiceChat.Vivox"), TEXT("Issuer"), CheckString, GEngineIni);
	if (CheckString.IsEmpty()) return false;
	GConfig->GetString(TEXT("VoiceChat.Vivox"), TEXT("InsecureSecret"), CheckString, GEngineIni);
	if (CheckString.IsEmpty()) return false;

	return true;
}

UVivoxSubsystem::UVivoxSubsystem()
{
	VivoxChat = IVoiceChat::Get();
}

UVivoxSubsystem::~UVivoxSubsystem()
{
}

void UVivoxSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	if (!AreVivoxVoiceChatValuesSet())
	{
		FText TitleText = FText::FromString(FString("Warning: Vivox Voice Chat Misconfigured!"));
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(FString("Voice Chat will not work properly until [VoiceChat.Vivox] is enabled in DefaultEngine.ini and your credential values are set correctly. See 'Docs/ShooterGameVoiceIntegrationGuide.pdf' for more info.")), &TitleText);
	}

	if (VivoxChat->Initialize())
	{
		Connect();
	}
}

void UVivoxSubsystem::Deinitialize()
{
	Super::Deinitialize();
	
    VivoxChat->Uninitialize();
}

void UVivoxSubsystem::Connect()
{
    FOnVoiceChatConnectCompleteDelegate OnVoiceChatConnectCompleteCallback;
	OnVoiceChatConnectCompleteCallback.BindLambda([this](const FVoiceChatResult& Result)
	{
		if (Result.IsSuccess())
		{
            UE_LOG(LogVivoxSubsystem, Log, TEXT("Connect success"));
			// Individual Vivox users may not join the same channel from more than one client,
		     // so applications using voice should provide unique Vivox usernames for each player.
		     // These usernames should have a 1:1 mapping to the player and should stay the same
		     // every time that player plays the game. If your game server issues and validates
		     // its own unique accounts, these should be used. For additional security and to
		     // ensure Vivox username restrictions are met, you can provide an md5 hash of your
		     // name instead.
		     // Since ShooterGame does not use its own game server or have accounts, we use the
		     // Nickname of player 0, which returns the player's name/tag from the online
		     // subsystem. On most platforms in most cases this is guaranteed to be unique. For
		     // Switch players on LAN it should be unique as long as each Switch is signed in with
		     // account profiles of different names. This Nickname is checked against Vivox
		     // username length and character restrictions and used if possible, or hashed if
		     // necessary. As long as the Nicknames are unique they will hash uniquely, but
		     // since not every name is hashed (for the convenience of readability in the logs)
		     // there is an infinitesimally small chance that a regular Nickname could match
		     // another's hash. We do not recommend choosing usernames this way in production.
		     //
		     // Note that using GetFirstGamePlayer() will only work correctly when there's one
		     // local player. Console games using networked splitscreen with multiple local
		     // players will need to login each player with their own uuids. A splitscreen voice
			 // chat implementation example is not included in this version of the sample.
#if !(PLATFORM_PS4 || PLATFORM_XBOXONE || PLATFORM_SWITCH)
            Login(GetVivoxSafePlayerName(GetGameInstance()->GetFirstGamePlayer()->GetPreferredUniqueNetId()->ToString()));
#endif
            }
        else
        {
        	UE_LOG(LogVivoxSubsystem, Error, TEXT("Connect failure: %s (%d)"), *Result.ErrorDesc, Result.ErrorNum);
        }
	});
	VivoxChat->Connect(OnVoiceChatConnectCompleteCallback);
}

TArray<FString> UVivoxSubsystem::GetChannels() const
{
	return VivoxChat->GetChannels();
}

EVivoxVoiceChatChannelType UVivoxSubsystem::GetChannelType(const FString& ChannelName) const
{
	return static_cast<EVivoxVoiceChatChannelType>(VivoxChat->GetChannelType(ChannelName));
}

void UVivoxSubsystem::Login(const FString& PlayerName)
{
	FPlatformUserId PlatformId = GetGameInstance()->GetFirstGamePlayer()->GetControllerId();
	
	const auto Identity = Online::GetIdentityInterface();
	if (Identity.IsValid() && GetGameInstance()->GetFirstGamePlayer()->GetPreferredUniqueNetId().IsValid())
	{
        PlatformId = Identity->GetPlatformUserIdFromUniqueNetId(*GetGameInstance()->GetFirstGamePlayer()->GetPreferredUniqueNetId());
	}

	FOnVoiceChatLoginCompleteDelegate OnVoiceChatLoginCompleteCallback;
	OnVoiceChatLoginCompleteCallback.BindLambda([this](const FString& PlayerName, const FVoiceChatResult& Result)
    {
        if (Result.IsSuccess())
        {
            UE_LOG(LogVivoxSubsystem, Log, TEXT("Login success for %s"), *PlayerName);
        }
        else
        {
            UE_LOG(LogVivoxSubsystem, Error, TEXT("Login failure for %s: %s (%d)"), *PlayerName, *Result.ErrorDesc, Result.ErrorNum);
        }
    });

	// IMPORTANT: in production, developers should NOT use the insecure client-side token generation methods.
	// To generate secure access tokens, call VivoxVoiceChatToken module methods from your game server.
	// This is important not only to secure access to Chat features, but tokens issued by the client could
	// appear expired if the client's clock is set incorrectly, resulting in rejection.
	VivoxChat->Login(PlatformId, PlayerName, VivoxChat->InsecureGetLoginToken(PlayerName), OnVoiceChatLoginCompleteCallback);
}

void UVivoxSubsystem::SetPlayerMuted(const FString& PlayerName, const bool bMuted) const
{
	VivoxChat->SetPlayerMuted(PlayerName, bMuted);
}

bool UVivoxSubsystem::IsLoggedIn() const
{
	return VivoxChat->IsLoggedIn();
}

bool UVivoxSubsystem::IsLoggingIn() const
{
	return VivoxChat->IsLoggingIn();
}

bool UVivoxSubsystem::IsPlayerMuted(const FString& PlayerName) const
{
	return VivoxChat->IsPlayerMuted(PlayerName);
}

void UVivoxSubsystem::SetAudioInputDeviceMuted(const bool bIsMuted) const
{
	VivoxChat->SetAudioInputDeviceMuted(bIsMuted);
}

void UVivoxSubsystem::SetAudioOutputDeviceMuted(const bool bIsMuted) const
{
	VivoxChat->SetAudioOutputDeviceMuted(bIsMuted);
}

void UVivoxSubsystem::Update3DPosition(APawn* Pawn)
{
	// Return if Pawn is invalid
	if (!Pawn)
			return;

	// Return if we're not in a positional channel.
	if (ConnectedPositionalChannel.IsEmpty())
		return;

	// Update cached 3D position and orientation.
	CachedPosition.SetValue(Pawn->GetActorLocation());
	CachedForwardVector.SetValue(Pawn->GetActorForwardVector());
	CachedUpVector.SetValue(Pawn->GetActorUpVector());

	// Return if there's no change from cached values.
	if (!Get3DValuesAreDirty())
		return;
	
	// Set new position and orientation in connected position channel.
	VivoxChat->Set3DPosition(ConnectedPositionalChannel, CachedPosition.GetValue(), CachedPosition.GetValue(), CachedForwardVector.GetValue(), CachedUpVector.GetValue());

	Clear3DValuesAreDirty();
}

FString UVivoxSubsystem::GetVivoxSafePlayerName(FString BaseName)
{bool bDoHash = false;

	// check length is <= 60 minus length of VivoxIssuer; default assumes max issuer length
	int32 VivoxSafePlayerLength = 35;
	FString VivoxIssuer;
	GConfig->GetString(TEXT("VoiceChat.Vivox"), TEXT("Issuer"), VivoxIssuer, GEngineIni);
	if (!VivoxIssuer.IsEmpty())
		VivoxSafePlayerLength = 60 - VivoxIssuer.Len();

	// a known issue limits this by one further character so this is >= instead of > for now.
	if (BaseName.Len() >= VivoxSafePlayerLength)
	{
		bDoHash = true;
	}
	else // also check character restrictions
		{
		const FString ValidCharacters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890=+-_.!~()";
		int32 Loc;
		auto ConstItr = BaseName.CreateConstIterator();
		while (BaseName.IsValidIndex(ConstItr.GetIndex()))
		{
			if (!ValidCharacters.FindChar(BaseName[ConstItr++.GetIndex()], Loc))
			{
				bDoHash = true;
				break;
			}
		}
		}

	if (bDoHash)
		return FMD5::HashAnsiString(*BaseName);
	else
		return BaseName;
}

bool UVivoxSubsystem::Get3DValuesAreDirty() const
{
	return (CachedPosition.IsDirty() ||
            CachedForwardVector.IsDirty() ||
            CachedUpVector.IsDirty());
}

void UVivoxSubsystem::Clear3DValuesAreDirty()
{
	CachedPosition.SetDirty(false);
	CachedForwardVector.SetDirty(false);
	CachedUpVector.SetDirty(false);
}