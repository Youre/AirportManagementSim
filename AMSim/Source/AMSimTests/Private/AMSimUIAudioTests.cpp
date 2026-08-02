#include "AMSimUISoundSubsystem.h"

#include "AMSimUITheme.h"
#include "Misc/AutomationTest.h"
#include "Sound/SoundGroups.h"
#include "Sound/SoundWave.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimUISoundCatalogTest,
	"AMSim.UI.Audio.CatalogAndCookerVisibleReferences",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimUISoundCatalogTest::RunTest(const FString& Parameters)
{
	const TArray<FName> RequiredIds = UAMSimUISoundSubsystem::RequiredCueIds();
	TSet<FName> UniqueIds;
	for (const FName Id : RequiredIds)
	{
		UniqueIds.Add(Id);
	}
	TestEqual(TEXT("The runtime catalog contains all 28 reviewed cues"), RequiredIds.Num(), 28);
	TestEqual(TEXT("Every runtime cue ID is unique"), UniqueIds.Num(), RequiredIds.Num());

	const UAMSimUISoundSubsystem* Catalog =
		GetDefault<UAMSimUISoundSubsystem>();
	TArray<FName> MissingIds;
	TestTrue(
		TEXT("Every typed cue resolves through a hard asset reference"),
		Catalog->HasCompleteCueSet(&MissingIds));
	TestEqual(TEXT("No required UI sound is missing"), MissingIds.Num(), 0);

	for (int32 Index = 0; Index < RequiredIds.Num(); ++Index)
	{
		USoundWave* Sound = Cast<USoundWave>(Catalog->GetCue(
			static_cast<EAMSimUISound>(Index)));
		if (!TestNotNull(
			*FString::Printf(TEXT("%s resolves to a sound wave"), *RequiredIds[Index].ToString()),
			Sound))
		{
			continue;
		}
		TestTrue(
			TEXT("UI sound package is inside the cooker-visible UI audio folder"),
			Sound->GetOutermost()->GetName().StartsWith(TEXT("/Game/Audio/UI/")));
		TestTrue(TEXT("UI sound is a short one-shot"),
			Sound->GetDuration() > 0.0f && Sound->GetDuration() <= 2.0f);
		TestEqual(TEXT("UI sound is stereo"), Sound->NumChannels, 2);
		TestFalse(TEXT("UI sound does not loop"), Sound->bLooping);
		TestEqual(TEXT("UI sound uses the UI sound group"), Sound->SoundGroup, SOUNDGROUP_UI);
		TestTrue(TEXT("UI sound has a non-zero reviewed level"), Sound->Volume > 0.0f);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimUISoundThemeTest,
	"AMSim.UI.Audio.RiverbendButtonStyleUsesSharedCues",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimUISoundThemeTest::RunTest(const FString& Parameters)
{
	const FButtonStyle Primary =
		AMSim::UITheme::ButtonStyle(AMSim::UITheme::EButton::Primary);
	const FButtonStyle Secondary =
		AMSim::UITheme::ButtonStyle(AMSim::UITheme::EButton::Secondary);
	const FButtonStyle Tool =
		AMSim::UITheme::ButtonStyle(AMSim::UITheme::EButton::Tool);
	TestTrue(
		TEXT("Every themed button shares the soft hover cue"),
		Primary.HoveredSlateSound.GetResourceObject() ==
			UAMSimUISoundSubsystem::GetDefaultCue(EAMSimUISound::HoverSoft));
	TestTrue(
		TEXT("Primary actions use the primary click cue"),
		Primary.PressedSlateSound.GetResourceObject() ==
			UAMSimUISoundSubsystem::GetDefaultCue(EAMSimUISound::ClickPrimary));
	TestTrue(
		TEXT("Secondary actions use the secondary click cue"),
		Secondary.PressedSlateSound.GetResourceObject() ==
			UAMSimUISoundSubsystem::GetDefaultCue(EAMSimUISound::ClickSecondary));
	TestTrue(
		TEXT("Compact tools use the focus tick cue"),
		Tool.PressedSlateSound.GetResourceObject() ==
			UAMSimUISoundSubsystem::GetDefaultCue(EAMSimUISound::FocusTick));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAMSimUISoundCooldownTest,
	"AMSim.UI.Audio.DeterministicCooldown",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAMSimUISoundCooldownTest::RunTest(const FString& Parameters)
{
	FAMSimUISoundCooldown Cooldown;
	TestTrue(TEXT("First cue is accepted"),
		Cooldown.Accept(EAMSimUISound::HoverSoft, 10.0, 0.1));
	TestFalse(TEXT("Repeated cue inside its cooldown is suppressed"),
		Cooldown.Accept(EAMSimUISound::HoverSoft, 10.05, 0.1));
	TestTrue(TEXT("A different cue remains independent"),
		Cooldown.Accept(EAMSimUISound::BuildSnap, 10.05, 0.1));
	TestTrue(TEXT("Cue is accepted after the cooldown"),
		Cooldown.Accept(EAMSimUISound::HoverSoft, 10.101, 0.1));
	Cooldown.Reset();
	TestTrue(TEXT("Reset clears prior playback history"),
		Cooldown.Accept(EAMSimUISound::HoverSoft, 0.0, 0.1));
	return true;
}

#endif
