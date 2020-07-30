// Copyright Epic Games, Inc. All Rights Reserved.

#include "VivoxSubsystem.h"
#include "Misc/MessageDialog.h"

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

	VivoxChat->Initialize();
}

void UVivoxSubsystem::Deinitialize()
{
	Super::Deinitialize();
	
    VivoxChat->Uninitialize();
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