// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThirdPersonShooterGameInstance.h"

#include "CommonSessionSubsystem.h"
#include "CommonUserSubsystem.h"
#include "Components/GameFrameworkComponentManager.h"
#include "HAL/IConsoleManager.h"
#include "ThirdPersonShooterGameplayTags.h"
#include "Player/ThirdPersonShooterPlayerController.h"
#include "Player/ThirdPersonShooterLocalPlayer.h"
#include "GameFramework/PlayerState.h"

#if UE_WITH_DTLS
#include "DTLSCertStore.h"
#include "DTLSHandlerComponent.h"
#include "Misc/FileHelper.h"
#endif // UE_WITH_DTLS

#include UE_INLINE_GENERATED_CPP_BY_NAME(ThirdPersonShooterGameInstance)

namespace ThirdPersonShooter
{
	static bool bTestEncryption = false;
	static FAutoConsoleVariableRef CVarThirdPersonShooterTestEncryption(
		TEXT("ThirdPersonShooter.TestEncryption"),
		bTestEncryption,
		TEXT("If true, clients will send an encryption token with their request to join the server and attempt to encrypt the connection using a debug key. This is NOT SECURE and for demonstration purposes only."),
		ECVF_Default);

#if UE_WITH_DTLS
	static bool bUseDTLSEncryption = false;
	static FAutoConsoleVariableRef CVarThirdPersonShooterUseDTLSEncryption(
		TEXT("ThirdPersonShooter.UseDTLSEncryption"),
		bUseDTLSEncryption,
		TEXT("Set to true if using ThirdPersonShooter.TestEncryption and the DTLS packet handler."),
		ECVF_Default);

	/* Intended for testing with multiple game instances on the same device (desktop builds) */
	static bool bTestDTLSFingerprint = false;
	static FAutoConsoleVariableRef CVarThirdPersonShooterTestDTLSFingerprint(
		TEXT("ThirdPersonShooter.TestDTLSFingerprint"),
		bTestDTLSFingerprint,
		TEXT("If true and using DTLS encryption, generate unique cert per connection and fingerprint will be written to file to simulate passing through an online service."),
		ECVF_Default);

#if !UE_BUILD_SHIPPING
	static FAutoConsoleCommandWithWorldAndArgs CmdGenerateDTLSCertificate(
		TEXT("GenerateDTLSCertificate"),
		TEXT("Generate a DTLS self-signed certificate for testing and export to PEM."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& InArgs, UWorld* InWorld)
			{
				if (InArgs.Num() == 1)
				{
					const FString& CertName = InArgs[0];

					FTimespan CertExpire = FTimespan::FromDays(365);
					TSharedPtr<FDTLSCertificate> Cert = FDTLSCertStore::Get().CreateCert(CertExpire, CertName);
					if (Cert.IsValid())
					{
						const FString CertPath = FPaths::ProjectContentDir() / TEXT("DTLS") / FPaths::MakeValidFileName(FString::Printf(TEXT("%s.pem"), *CertName));

						if (!Cert->ExportCertificate(CertPath))
						{
							UE_LOG(LogTemp, Error, TEXT("GenerateDTLSCertificate: Failed to export certificate."));
						}
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("GenerateDTLSCertificate: Failed to generate certificate."));
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("GenerateDTLSCertificate: Invalid argument(s)."));
				}
			}));
#endif // UE_BUILD_SHIPPING
#endif // UE_WITH_DTLS
};

UThirdPersonShooterGameInstance::UThirdPersonShooterGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UThirdPersonShooterGameInstance::Init()
{
	Super::Init();

	// Register our custom init states
	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);

	if (ensure(ComponentManager))
	{
		ComponentManager->RegisterInitState(ThirdPersonShooterGameplayTags::InitState_Spawned, false, FGameplayTag());
		ComponentManager->RegisterInitState(ThirdPersonShooterGameplayTags::InitState_DataAvailable, false, ThirdPersonShooterGameplayTags::InitState_Spawned);
		ComponentManager->RegisterInitState(ThirdPersonShooterGameplayTags::InitState_DataInitialized, false, ThirdPersonShooterGameplayTags::InitState_DataAvailable);
		ComponentManager->RegisterInitState(ThirdPersonShooterGameplayTags::InitState_GameplayReady, false, ThirdPersonShooterGameplayTags::InitState_DataInitialized);
	}

	// Initialize the debug key with a set value for AES256. This is not secure and for example purposes only.
	DebugTestEncryptionKey.SetNum(32);

	for (int32 i = 0; i < DebugTestEncryptionKey.Num(); ++i)
	{
		DebugTestEncryptionKey[i] = uint8(i);
	}

	if (UCommonSessionSubsystem* SessionSubsystem = GetSubsystem<UCommonSessionSubsystem>())
	{
		// SessionSubsystem->OnPreClientTravelEvent.AddUObject(this, &UThirdPersonShooterGameInstance::OnPreClientTravelToSession);
	}
}

void UThirdPersonShooterGameInstance::Shutdown()
{
	if (UCommonSessionSubsystem* SessionSubsystem = GetSubsystem<UCommonSessionSubsystem>())
	{
		SessionSubsystem->OnPreClientTravelEvent.RemoveAll(this);
	}

	Super::Shutdown();
}

AThirdPersonShooterPlayerController* UThirdPersonShooterGameInstance::GetPrimaryPlayerController() const
{
	return Cast<AThirdPersonShooterPlayerController>(Super::GetPrimaryPlayerController(false));
}

bool UThirdPersonShooterGameInstance::CanJoinRequestedSession() const
{
	// Temporary first pass:  Always return true
	// This will be fleshed out to check the player's state
	if (!Super::CanJoinRequestedSession())
	{
		return false;
	}
	return true;
}

void UThirdPersonShooterGameInstance::HandlerUserInitialized(const UCommonUserInfo* UserInfo, bool bSuccess, FText Error, ECommonUserPrivilege RequestedPrivilege, ECommonUserOnlineContext OnlineContext)
{
	Super::HandlerUserInitialized(UserInfo, bSuccess, Error, RequestedPrivilege, OnlineContext);

	// If login succeeded, tell the local player to load their settings
	if (bSuccess && ensure(UserInfo))
	{
		UThirdPersonShooterLocalPlayer* LocalPlayer = Cast<UThirdPersonShooterLocalPlayer>(GetLocalPlayerByIndex(UserInfo->LocalPlayerIndex));

		// There will not be a local player attached to the dedicated server user
		if (LocalPlayer)
		{
			LocalPlayer->LoadSharedSettingsFromDisk();
		}
	}
}
