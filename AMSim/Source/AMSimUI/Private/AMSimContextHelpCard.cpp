#include "AMSimContextHelpCard.h"

#include "AMSimAirportSimulationSubsystem.h"
#include "AMSimContextHelp.h"
#include "AMSimUITheme.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/UserInterfaceSettings.h"

namespace
{
	UTextBlock* MakeHelpText(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const FString& Copy,
		const int32 Size,
		const FLinearColor& Color,
		const bool bBold = false)
	{
		UTextBlock* Text = Tree->ConstructWidget<UTextBlock>(
			UTextBlock::StaticClass(),
			Name);
		Text->SetText(FText::FromString(Copy));
		Text->SetAutoWrapText(true);
		Text->SetLineHeightPercentage(1.0f);
		AMSim::UITheme::StyleText(
			Text,
			Size,
			Color,
			bBold,
			true);
		return Text;
	}

	void AddHelpRow(
		UVerticalBox* Box,
		UWidget* Widget,
		const float Bottom)
	{
		UVerticalBoxSlot* Slot =
			Box->AddChildToVerticalBox(Widget);
		Slot->SetPadding(
			FMargin(0.0f, 0.0f, 0.0f, Bottom));
	}
}

TSharedRef<SWidget> UAMSimContextHelpCard::RebuildWidget()
{
	using namespace AMSim::UITheme;
	if (!WidgetTree)
	{
		return Super::RebuildWidget();
	}
	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(
		UCanvasPanel::StaticClass(),
		TEXT("ContextHelpRoot"));
	WidgetTree->RootWidget = Root;

	CardSurface = WidgetTree->ConstructWidget<UBorder>(
		UBorder::StaticClass(),
		TEXT("ContextHelpSurface"));
	StyleSurface(
		CardSurface,
		ESurface::RaisedCard,
		FMargin(18.0f, 14.0f),
		18.0f,
		1.8f);
	UVerticalBox* Column =
		WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			TEXT("ContextHelpColumn"));
	CardSurface->SetContent(Column);

	EyebrowText = MakeHelpText(
		WidgetTree,
		TEXT("ContextHelpEyebrow"),
		TEXT("FIRST AIRPORT"),
		10,
		Cyan(),
		true);
	AddHelpRow(Column, EyebrowText, 3.0f);
	TitleText = MakeHelpText(
		WidgetTree,
		TEXT("ContextHelpTitle"),
		TEXT("A short helpful title"),
		18,
		White(),
		true);
	AddHelpRow(Column, TitleText, 5.0f);
	BodyText = MakeHelpText(
		WidgetTree,
		TEXT("ContextHelpBody"),
		TEXT("Context help"),
		14,
		White());
	AddHelpRow(Column, BodyText, 7.0f);
	ActionText = MakeHelpText(
		WidgetTree,
		TEXT("ContextHelpAction"),
		TEXT("NEXT  Choose an available action."),
		12,
		Amber(),
		true);
	AddHelpRow(Column, ActionText, 9.0f);

	AcknowledgeButton = WidgetTree->ConstructWidget<UButton>(
		UButton::StaticClass(),
		TEXT("ContextHelpAcknowledge"));
	AcknowledgeButton->SetStyle(
		ButtonStyle(EButton::Primary, 12.0f));
	UTextBlock* ButtonLabel = MakeHelpText(
		WidgetTree,
		TEXT("ContextHelpAcknowledgeLabel"),
		TEXT("GOT IT"),
		12,
		White(),
		true);
	ButtonLabel->SetJustification(ETextJustify::Center);
	AcknowledgeButton->SetContent(ButtonLabel);
	AcknowledgeButton->OnClicked.AddDynamic(
		this,
		&UAMSimContextHelpCard::AcknowledgeCurrentHelp);
	AddHelpRow(Column, AcknowledgeButton, 0.0f);

	const UUserInterfaceSettings* Settings =
		GetDefault<UUserInterfaceSettings>();
	const float Scale = Settings
		? Settings->GetDPIScaleBasedOnSize(FIntPoint(1920, 1080))
		: 1.0f;
	const bool bCompact = Scale >= 1.75f;
	UCanvasPanelSlot* CardSlot =
		Root->AddChildToCanvas(CardSurface);
	CardSlot->SetAnchors(
		bCompact
			? FAnchors(0.08f, 0.16f, 0.92f, 0.58f)
			: FAnchors(0.30f, 0.16f, 0.70f, 0.42f));
	CardSlot->SetOffsets(FMargin(0.0f));
	CardSlot->SetZOrder(30);
	CardSurface->SetVisibility(ESlateVisibility::Collapsed);
	RefreshFromSimulation();
	return Super::RebuildWidget();
}

void UAMSimContextHelpCard::NativeTick(
	const FGeometry& MyGeometry,
	const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	RefreshFromSimulation();
}

void UAMSimContextHelpCard::RefreshFromSimulation()
{
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()
			? GetWorld()->GetSubsystem<
				UAMSimAirportSimulationSubsystem>()
			: nullptr;
	if (!Subsystem || !CardSurface)
	{
		return;
	}
	const AMSim::FPhase1QuerySnapshot Query =
		Subsystem->GetPhase1Query();
	if (LastRevision == Query.Revision)
	{
		return;
	}
	LastRevision = Query.Revision;
	const AMSim::FContextHelpDefinition* Definition =
		AMSim::SelectPhase1ContextHelp(
			Subsystem->GetSimulation().GetPhase1State());
	if (!Definition)
	{
		CurrentHelpId = {};
		CardSurface->SetVisibility(
			ESlateVisibility::Collapsed);
		return;
	}
	CurrentHelpId = Definition->Id;
	EyebrowText->SetText(
		FText::FromString(Definition->Eyebrow));
	TitleText->SetText(
		FText::FromString(Definition->Title));
	BodyText->SetText(
		FText::FromString(Definition->Body));
	ActionText->SetText(FText::FromString(
		TEXT("NEXT  ") + Definition->NextAction));
	CardSurface->SetVisibility(
		ESlateVisibility::SelfHitTestInvisible);
}

void UAMSimContextHelpCard::AcknowledgeCurrentHelp()
{
	if (CurrentHelpId.IsNone() || !GetWorld())
	{
		return;
	}
	if (UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()->GetSubsystem<
			UAMSimAirportSimulationSubsystem>())
	{
		AMSim::FPhase1Command Command;
		Command.Type =
			AMSim::EPhase1CommandType::AcknowledgeContextHelp;
		Command.ContextHelpId = CurrentHelpId;
		Subsystem->SubmitPhase1Command(Command);
		LastRevision = MAX_uint64;
		RefreshFromSimulation();
	}
}
