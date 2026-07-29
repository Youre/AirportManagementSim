#include "AMSimRegionalOperationsView.h"

#include "Algo/Count.h"
#include "AMSimAirportSimulationSubsystem.h"
#include "AMSimGameInstanceSubsystem.h"
#include "AMSimOverviewView.h"
#include "AMSimPhase5View.h"
#include "AMSimProgressionView.h"
#include "AMSimRadioSubsystem.h"
#include "AMSimSaveStore.h"
#include "AMSimTimetableGeometry.h"
#include "AMSimUITheme.h"
#include "AMSimWorldPresenter.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/ScrollBox.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/UserInterfaceSettings.h"
#include "Engine/Texture2D.h"
#include "EngineUtils.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "UObject/ConstructorHelpers.h"

namespace AMSimRegionalOperationsViewPrivate
{
	using namespace AMSim::UITheme;

	UTextBlock* MakeText(
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
		Text->SetLineHeightPercentage(0.92f);
		StyleText(Text, Size, Color, bBold, true);
		return Text;
	}

	UBorder* MakeSurface(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const ESurface Surface,
		const FMargin& Padding = FMargin(12.0f),
		const float Radius = 14.0f,
		const float Stroke = 1.4f)
	{
		UBorder* Border = Tree->ConstructWidget<UBorder>(
			UBorder::StaticClass(),
			Name);
		StyleSurface(Border, Surface, Padding, Radius, Stroke);
		return Border;
	}

	UButton* MakeButton(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const FString& Label,
		const EButton Kind,
		const int32 Size)
	{
		UButton* Button = Tree->ConstructWidget<UButton>(
			UButton::StaticClass(),
			Name);
		Button->SetStyle(ButtonStyle(Kind, 11.0f));
		UTextBlock* LabelText = MakeText(
			Tree,
			*FString::Printf(TEXT("%sLabel"), Name),
			Label,
			Size,
			White(),
			true);
		LabelText->SetJustification(ETextJustify::Center);
		Button->SetContent(LabelText);
		return Button;
	}

	UCanvasPanelSlot* AddAnchored(
		UCanvasPanel* Canvas,
		UWidget* Widget,
		const FAnchors& Anchors,
		const FMargin& Offsets = FMargin(0.0f),
		const int32 ZOrder = 0)
	{
		UCanvasPanelSlot* Slot = Canvas->AddChildToCanvas(Widget);
		Slot->SetAnchors(Anchors);
		Slot->SetOffsets(Offsets);
		Slot->SetZOrder(ZOrder);
		return Slot;
	}

	void AddVertical(
		UVerticalBox* Box,
		UWidget* Widget,
		const float Bottom = 8.0f,
		const bool bFill = false)
	{
		UVerticalBoxSlot* Slot = Box->AddChildToVerticalBox(Widget);
		Slot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, Bottom));
		Slot->SetSize(FSlateChildSize(
			bFill ? ESlateSizeRule::Fill : ESlateSizeRule::Automatic));
	}

	void AddHorizontal(
		UHorizontalBox* Box,
		UWidget* Widget,
		const float Right = 8.0f,
		const bool bFill = false)
	{
		UHorizontalBoxSlot* Slot = Box->AddChildToHorizontalBox(Widget);
		Slot->SetPadding(FMargin(0.0f, 0.0f, Right, 0.0f));
		Slot->SetSize(FSlateChildSize(
			bFill ? ESlateSizeRule::Fill : ESlateSizeRule::Automatic));
		Slot->SetVerticalAlignment(VAlign_Center);
	}

	UBorder* MakeMetricCard(
		UWidgetTree* Tree,
		const TCHAR* Name,
		TObjectPtr<UTextBlock>& OutText,
		const FString& Initial,
		const int32 Size,
		const ESurface Surface = ESurface::Card)
	{
		UBorder* Card = MakeSurface(
			Tree,
			Name,
			Surface,
			FMargin(11.0f),
			12.0f);
		OutText = MakeText(
			Tree,
			*FString::Printf(TEXT("%sText"), Name),
			Initial,
			Size,
			White());
		Card->SetContent(OutText);
		return Card;
	}

	UCanvasPanel* MakeStripedWindow(
		UWidgetTree* Tree,
		const TCHAR* Name,
		TObjectPtr<UTextBlock>& OutText)
	{
		UCanvasPanel* Window = Tree->ConstructWidget<UCanvasPanel>(
			UCanvasPanel::StaticClass(),
			Name);
		UBorder* Base = MakeSurface(
			Tree,
			*FString::Printf(TEXT("%sBase"), Name),
			ESurface::Warning,
			FMargin(0.0f),
			9.0f,
			2.0f);
		Base->SetBrushColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.17f));
		AddAnchored(Window, Base, FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
		for (int32 Index = 0; Index < 8; ++Index)
		{
			UBorder* Stripe = Tree->ConstructWidget<UBorder>(
				UBorder::StaticClass(),
				*FString::Printf(TEXT("%sStripe%d"), Name, Index));
			Stripe->SetBrushColor(FLinearColor(1.0f, 0.86f, 0.42f, 0.05f));
			const float Left = 0.03f + 0.12f * Index;
			AddAnchored(
				Window,
				Stripe,
				FAnchors(Left, 0.0f, Left + 0.028f, 1.0f),
				FMargin(),
				1);
		}
		OutText = MakeText(
			Tree,
			*FString::Printf(TEXT("%sText"), Name),
			TEXT("WEATHER WINDOW"),
			9,
			Amber(),
			true);
		OutText->SetJustification(ETextJustify::Center);
		AddAnchored(
			Window,
			OutText,
			FAnchors(0.02f, 0.02f, 0.98f, 0.30f),
			FMargin(),
			2);
		return Window;
	}

	void SetActionVisible(UButton* Button, const bool bVisible)
	{
		if (Button)
		{
			Button->SetIsEnabled(bVisible);
			Button->SetVisibility(
				bVisible
					? ESlateVisibility::Visible
					: ESlateVisibility::Collapsed);
		}
	}

	FString MinuteDisplay(const int32 Minute)
	{
		return FString::Printf(
			TEXT("%02d:%02d"),
			(Minute / 60) % 24,
			Minute % 60);
	}

	FString AircraftShortLabel(const FString& DisplayName)
	{
		if (DisplayName.Contains(TEXT("Trailwing")))
		{
			return TEXT("TW42");
		}
		if (DisplayName.Contains(TEXT("Skylark")))
		{
			return TEXT("SL72");
		}
		if (DisplayName.Contains(TEXT("Harborliner")))
		{
			return TEXT("HL126");
		}
		return DisplayName.Left(6).ToUpper();
	}

	FString OperatorShortLabel(const FString& DisplayName)
	{
		if (DisplayName.Contains(TEXT("Riverbend")))
		{
			return TEXT("RIVERBEND");
		}
		if (DisplayName.Contains(TEXT("Northstar")))
		{
			return TEXT("NORTHSTAR");
		}
		if (DisplayName.Contains(TEXT("Coastal")))
		{
			return TEXT("COASTAL");
		}
		return DisplayName.Left(9).ToUpper();
	}
}

using namespace AMSimRegionalOperationsViewPrivate;

UAMSimRegionalOperationsView::UAMSimRegionalOperationsView(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static const TCHAR* IdentityPaths[] = {
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_Flights.T_Flights"),
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_MixedAirport.T_MixedAirport"),
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_Passengers.T_Passengers")};
	for (const TCHAR* Path : IdentityPaths)
	{
		ConstructorHelpers::FObjectFinderOptional<UTexture2D> Finder(Path);
		ContractIdentityTextures.Add(Finder.Get());
	}
	static ConstructorHelpers::FObjectFinderOptional<UTexture2D> AircraftFinder(
		TEXT("/Game/Phase1/Presentation/Textures/Aircraft/T_Cessna152_Heading_00.T_Cessna152_Heading_00"));
	ContractAircraftTexture = AircraftFinder.Get();
	static ConstructorHelpers::FObjectFinderOptional<UTexture2D> WeatherFinder(
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_Weather.T_Weather"));
	WeatherIconTexture = WeatherFinder.Get();
}

TSharedRef<SWidget> UAMSimRegionalOperationsView::RebuildWidget()
{
	if (!WidgetTree || WidgetTree->RootWidget)
	{
		return Super::RebuildWidget();
	}

	const UUserInterfaceSettings* Settings =
		GetDefault<UUserInterfaceSettings>();
	const float Scale = Settings
		? Settings->GetDPIScaleBasedOnSize(FIntPoint(1920, 1080))
		: 1.0f;
	bCompactLayout = Scale >= 1.75f;
	const int32 TitleSize = bCompactLayout ? 15 : 20;
	const int32 BodySize = bCompactLayout ? 12 : 14;
	const int32 SmallSize = bCompactLayout ? 10 : 12;

	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(
		UCanvasPanel::StaticClass(),
		TEXT("RegionalRoot"));
	WidgetTree->RootWidget = Root;
	RootSurface = Root;

	UBorder* TopBar = MakeSurface(
		WidgetTree,
		TEXT("RegionalTopBar"),
		ESurface::Chrome,
		bCompactLayout
			? FMargin(12.0f, 7.0f)
			: FMargin(18.0f, 9.0f),
		17.0f,
		1.8f);
	UHorizontalBox* TopRow = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("RegionalTopRow"));
	TopBar->SetContent(TopRow);
	UTextBlock* Brand = MakeText(
		WidgetTree,
		TEXT("RegionalBrand"),
		bCompactLayout
			? TEXT("RIVERBEND")
			: TEXT("RIVERBEND AIRPORT · REGIONAL OPERATIONS"),
		TitleSize,
		White(),
		true);
	AddHorizontal(TopRow, Brand, 12.0f, true);
	FundsText = MakeText(
		WidgetTree,
		TEXT("RegionalFunds"),
		TEXT("0 CR"),
		BodySize,
		Amber(),
		true);
	AddHorizontal(TopRow, FundsText, 16.0f);
	WeatherText = MakeText(
		WidgetTree,
		TEXT("RegionalWeatherTop"),
		TEXT("FORECAST READY"),
		SmallSize,
		Muted(),
		true);
	AddHorizontal(TopRow, WeatherText, 16.0f);
	ClockText = MakeText(
		WidgetTree,
		TEXT("RegionalClock"),
		TEXT("DAY 1 · 08:00"),
		BodySize,
		CyanSoft(),
		true);
	AddHorizontal(TopRow, ClockText, 12.0f);
	UButton* ReturnButton = MakeButton(
		WidgetTree,
		TEXT("RegionalReturn"),
		TEXT("BACK"),
		EButton::Secondary,
		SmallSize);
	ReturnButton->OnClicked.AddDynamic(
		this,
		&UAMSimRegionalOperationsView::ReturnToAirport);
	AddHorizontal(TopRow, ReturnButton, 7.0f);
	UButton* OverviewButton = MakeButton(
		WidgetTree,
		TEXT("RegionalOverview"),
		TEXT("OVERVIEW"),
		EButton::Secondary,
		SmallSize);
	OverviewButton->OnClicked.AddDynamic(
		this,
		&UAMSimRegionalOperationsView::ToggleOverview);
	AddHorizontal(TopRow, OverviewButton, 7.0f);
	UButton* ProgressionButton = MakeButton(
		WidgetTree,
		TEXT("RegionalCapabilities"),
		bCompactLayout ? TEXT("PATHS") : TEXT("CAPABILITIES"),
		EButton::Secondary,
		SmallSize);
	ProgressionButton->OnClicked.AddDynamic(
		this,
		&UAMSimRegionalOperationsView::ToggleProgression);
	AddHorizontal(TopRow, ProgressionButton, 7.0f);
	AdvancedOperationsButton = MakeButton(
		WidgetTree,
		TEXT("RegionalAdvancedOperations"),
		bCompactLayout ? TEXT("ADV") : TEXT("ADVANCED"),
		EButton::Secondary,
		SmallSize);
	AdvancedOperationsButton->OnClicked.AddDynamic(
		this,
		&UAMSimRegionalOperationsView::ToggleAdvancedOperations);
	AdvancedOperationsButton->SetVisibility(
		ESlateVisibility::Collapsed);
	AddHorizontal(TopRow, AdvancedOperationsButton, 7.0f);
	MajorOperationsButton = MakeButton(
		WidgetTree,
		TEXT("RegionalMajorOperations"),
		TEXT("MAJOR"),
		EButton::Secondary,
		SmallSize);
	MajorOperationsButton->OnClicked.AddDynamic(
		this,
		&UAMSimRegionalOperationsView::ToggleMajorOperations);
	MajorOperationsButton->SetVisibility(
		ESlateVisibility::Collapsed);
	AddHorizontal(TopRow, MajorOperationsButton, 7.0f);
	UButton* PauseButton = MakeButton(
		WidgetTree,
		TEXT("RegionalPause"),
		TEXT("PAUSE"),
		EButton::Quiet,
		SmallSize);
	PauseButton->OnClicked.AddDynamic(
		this,
		&UAMSimRegionalOperationsView::PauseSimulation);
	AddHorizontal(TopRow, PauseButton, 6.0f);
	UButton* SpeedOneButton = MakeButton(
		WidgetTree,
		TEXT("RegionalSpeed1"),
		TEXT("1x"),
		EButton::Quiet,
		SmallSize);
	SpeedOneButton->OnClicked.AddDynamic(
		this,
		&UAMSimRegionalOperationsView::SetSpeedOne);
	AddHorizontal(TopRow, SpeedOneButton, 6.0f);
	UButton* SpeedTwoButton = MakeButton(
		WidgetTree,
		TEXT("RegionalSpeed2"),
		TEXT("2x"),
		EButton::Quiet,
		SmallSize);
	SpeedTwoButton->OnClicked.AddDynamic(
		this,
		&UAMSimRegionalOperationsView::SetSpeedTwo);
	AddHorizontal(TopRow, SpeedTwoButton, 6.0f);
	UButton* SpeedFourButton = MakeButton(
		WidgetTree,
		TEXT("RegionalSpeed4"),
		TEXT("4x"),
		EButton::Quiet,
		SmallSize);
	SpeedFourButton->OnClicked.AddDynamic(
		this,
		&UAMSimRegionalOperationsView::SetSpeedFour);
	AddHorizontal(TopRow, SpeedFourButton, 6.0f);
	UButton* SpeedEightButton = MakeButton(
		WidgetTree,
		TEXT("RegionalSpeed8"),
		TEXT("8x"),
		EButton::Quiet,
		SmallSize);
	SpeedEightButton->OnClicked.AddDynamic(
		this,
		&UAMSimRegionalOperationsView::SetSpeedEight);
	AddHorizontal(TopRow, SpeedEightButton, 6.0f);
	AddAnchored(
		Root,
		TopBar,
		FAnchors(0.008f, 0.008f, 0.992f, 0.083f),
		FMargin(0.0f),
		20);

	TimetableSurface = WidgetTree->ConstructWidget<UCanvasPanel>(
		UCanvasPanel::StaticClass(),
		TEXT("RegionalTimetableSurface"));
	AddAnchored(
		Root,
		TimetableSurface,
		FAnchors(0.0f, 0.085f, 1.0f, 1.0f));

	UBorder* ContractRail = MakeSurface(
		WidgetTree,
		TEXT("RegionalContractRail"),
		ESurface::Panel,
		FMargin(bCompactLayout ? 11.0f : 15.0f),
		17.0f);
	UScrollBox* ContractScroll =
		WidgetTree->ConstructWidget<UScrollBox>(
			UScrollBox::StaticClass(),
			TEXT("RegionalContractScroll"));
	UVerticalBox* ContractColumn =
		WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			TEXT("RegionalContractColumn"));
	ContractScroll->AddChild(ContractColumn);
	ContractRail->SetContent(ContractScroll);
	AddVertical(
		ContractColumn,
		MakeText(
			WidgetTree,
			TEXT("RegionalContractTitle"),
			TEXT("RECURRING OFFERS"),
			TitleSize,
			White(),
			true),
		5.0f);
	AddVertical(
		ContractColumn,
		MakeText(
			WidgetTree,
			TEXT("RegionalContractCopy"),
			TEXT("Three fictional operators · seven daily services each"),
			SmallSize,
			Muted()),
		10.0f);
	for (int32 Index = 0; Index < 3; ++Index)
	{
		UBorder* Card = MakeSurface(
			WidgetTree,
			*FString::Printf(TEXT("RegionalContractCard%d"), Index),
			ESurface::RaisedCard,
			FMargin(9.0f),
			12.0f);
		UHorizontalBox* CardRow =
			WidgetTree->ConstructWidget<UHorizontalBox>(
				UHorizontalBox::StaticClass(),
				*FString::Printf(TEXT("RegionalContractRow%d"), Index));
		Card->SetContent(CardRow);

		USizeBox* OperatorBox = WidgetTree->ConstructWidget<USizeBox>(
			USizeBox::StaticClass(),
			*FString::Printf(TEXT("RegionalContractOperatorSize%d"), Index));
		OperatorBox->SetWidthOverride(bCompactLayout ? 28.0f : 36.0f);
		OperatorBox->SetHeightOverride(bCompactLayout ? 28.0f : 36.0f);
		UImage* OperatorImage = WidgetTree->ConstructWidget<UImage>(
			UImage::StaticClass(),
			*FString::Printf(TEXT("RegionalContractOperator%d"), Index));
		if (ContractIdentityTextures.IsValidIndex(Index))
		{
			OperatorImage->SetBrushFromTexture(
				ContractIdentityTextures[Index],
				true);
		}
		static const FLinearColor OperatorTints[] = {
			FLinearColor(0.33f, 0.86f, 0.96f, 1.0f),
			FLinearColor(1.0f, 0.72f, 0.24f, 1.0f),
			FLinearColor(0.44f, 0.80f, 0.48f, 1.0f)};
		OperatorImage->SetColorAndOpacity(OperatorTints[Index]);
		OperatorBox->SetContent(OperatorImage);
		AddHorizontal(CardRow, OperatorBox, 8.0f);

		TObjectPtr<UTextBlock> Text = MakeText(
			WidgetTree,
			*FString::Printf(TEXT("RegionalContractText%d"), Index),
			TEXT("SELECTED OPERATOR OFFER"),
			SmallSize,
			White(),
			true);
		ContractCardTexts.Add(Text);
		AddHorizontal(CardRow, Text, 6.0f, true);

		USizeBox* AircraftBox = WidgetTree->ConstructWidget<USizeBox>(
			USizeBox::StaticClass(),
			*FString::Printf(TEXT("RegionalContractAircraftSize%d"), Index));
		AircraftBox->SetWidthOverride(bCompactLayout ? 34.0f : 46.0f);
		AircraftBox->SetHeightOverride(bCompactLayout ? 24.0f : 32.0f);
		UImage* AircraftImage = WidgetTree->ConstructWidget<UImage>(
			UImage::StaticClass(),
			*FString::Printf(TEXT("RegionalContractAircraft%d"), Index));
		AircraftImage->SetBrushFromTexture(ContractAircraftTexture, true);
		AircraftImage->SetColorAndOpacity(OperatorTints[Index]);
		AircraftBox->SetContent(AircraftImage);
		AddHorizontal(CardRow, AircraftBox, 0.0f);

		ContractCards.Add(Card);
		AddVertical(ContractColumn, Card, 9.0f);
	}
	InitializeButton = MakeButton(
		WidgetTree,
		TEXT("RegionalInitialize"),
		TEXT("OPEN REGIONAL PLANNING"),
		EButton::Primary,
		SmallSize);
	InitializeButton->OnClicked.AddDynamic(
		this,
		&UAMSimRegionalOperationsView::InitializeRegionalAirport);
	AddVertical(ContractColumn, InitializeButton, 8.0f);
	AcceptContractButton = MakeButton(
		WidgetTree,
		TEXT("RegionalAcceptContract"),
		TEXT("ACCEPT NEXT CONTRACT"),
		EButton::Positive,
		SmallSize);
	AcceptContractButton->OnClicked.AddDynamic(
		this,
		&UAMSimRegionalOperationsView::AcceptNextContract);
	AddVertical(ContractColumn, AcceptContractButton, 8.0f);
	PublishButton = MakeButton(
		WidgetTree,
		TEXT("RegionalPublish"),
		TEXT("PUBLISH SEVEN-DAY TIMETABLE"),
		EButton::Primary,
		SmallSize);
	PublishButton->OnClicked.AddDynamic(
		this,
		&UAMSimRegionalOperationsView::PublishTimetable);
	AddVertical(ContractColumn, PublishButton, 8.0f);
	AddAnchored(
		TimetableSurface,
		ContractRail,
		bCompactLayout
			? FAnchors(0.012f, 0.018f, 0.252f, 0.90f)
			: FAnchors(0.012f, 0.018f, 0.202f, 0.90f));

	WeekGrid = MakeSurface(
		WidgetTree,
		TEXT("RegionalWeekGrid"),
		ESurface::Chrome,
		FMargin(bCompactLayout ? 10.0f : 14.0f),
		19.0f,
		1.8f);
	UCanvasPanel* GridCanvas =
		WidgetTree->ConstructWidget<UCanvasPanel>(
			UCanvasPanel::StaticClass(),
			TEXT("RegionalGridCanvas"));
	WeekGrid->SetContent(GridCanvas);
	StatusText = MakeText(
		WidgetTree,
		TEXT("RegionalStatus"),
		TEXT("REGIONAL PLANNING READY"),
		TitleSize,
		White(),
		true);
	StatusText->SetJustification(ETextJustify::Center);
	AddAnchored(
		GridCanvas,
		StatusText,
		FAnchors(0.02f, 0.015f, 0.98f, 0.075f));
	TimetableText = MakeText(
		WidgetTree,
		TEXT("RegionalTimetableSummary"),
		TEXT("MON–SUN · A1 / A2 / R1"),
		SmallSize,
		CyanSoft(),
		true);
	TimetableText->SetJustification(ETextJustify::Center);
	AddAnchored(
		GridCanvas,
		TimetableText,
		FAnchors(0.02f, 0.078f, 0.98f, 0.12f));

	static const TCHAR* Days[] = {
		TEXT("MON"), TEXT("TUE"), TEXT("WED"), TEXT("THU"),
		TEXT("FRI"), TEXT("SAT"), TEXT("SUN")};
	for (int32 Day = 0; Day < 7; ++Day)
	{
		const float Left =
			AMSim::FTimetableGeometry::DayColumnLeft(Day + 1);
		const float Right =
			AMSim::FTimetableGeometry::DayColumnRight(Day + 1);
		UBorder* DayChip = MakeSurface(
			WidgetTree,
			*FString::Printf(TEXT("RegionalDay%d"), Day),
			Day == 0 ? ESurface::RaisedCard : ESurface::Chip,
			FMargin(5.0f),
			8.0f);
		UTextBlock* DayText = MakeText(
			WidgetTree,
			*FString::Printf(TEXT("RegionalDayText%d"), Day),
			Days[Day],
			SmallSize,
			Day == 0 ? Cyan() : Muted(),
			true);
		DayText->SetJustification(ETextJustify::Center);
		DayChip->SetContent(DayText);
		DayChips.Add(DayChip);
		DayChipTexts.Add(DayText);
		AddAnchored(
			GridCanvas,
			DayChip,
			FAnchors(Left, 0.13f, Right, 0.19f));
	}

	static const int32 TimeMarks[] = {
		6 * 60, 9 * 60, 12 * 60, 15 * 60, 18 * 60, 21 * 60};
	for (const int32 Minute : TimeMarks)
	{
		const float LineY =
			AMSim::FTimetableGeometry::MinuteToAnchor(Minute);
		UTextBlock* TimeLabel = MakeText(
			WidgetTree,
			*FString::Printf(TEXT("RegionalTimeLabel%d"), Minute),
			MinuteDisplay(Minute),
			SmallSize,
			Muted(),
			true);
		TimeLabel->SetJustification(ETextJustify::Right);
		AddAnchored(
			GridCanvas,
			TimeLabel,
			FAnchors(0.004f, LineY - 0.014f, 0.050f, LineY + 0.018f));
		UBorder* GuideLine =
			WidgetTree->ConstructWidget<UBorder>(
				UBorder::StaticClass(),
				*FString::Printf(TEXT("RegionalTimeGuide%d"), Minute));
		GuideLine->SetBrushColor(FLinearColor(
			CyanSoft().R,
			CyanSoft().G,
			CyanSoft().B,
			Minute == 12 * 60 ? 0.24f : 0.11f));
		AddAnchored(
			GridCanvas,
			GuideLine,
			FAnchors(0.055f, LineY, 0.988f, LineY + 0.0015f));
	}

	for (int32 Index = 0; Index < 21; ++Index)
	{
		const int32 Day = Index / 3;
		const int32 Row = Index % 3;
		UBorder* Card = MakeSurface(
			WidgetTree,
			*FString::Printf(TEXT("RegionalFlightCard%d"), Index),
			ESurface::Card,
			FMargin(bCompactLayout ? 7.0f : 9.0f),
			10.0f);
		UTextBlock* CardText = MakeText(
			WidgetTree,
			*FString::Printf(TEXT("RegionalFlightText%d"), Index),
			TEXT("FLIGHT · SLOT · GATE"),
			bCompactLayout ? 9 : 10,
			White(),
			true);
		Card->SetContent(CardText);
		FlightCards.Add(Card);
		FlightCardTexts.Add(CardText);
		const int32 PlaceholderMinute = 7 * 60 + Row * 5 * 60;
		const AMSim::FTimetableCardGeometry Geometry =
			AMSim::FTimetableGeometry::MakeCard(
				Day + 1,
				PlaceholderMinute,
				PlaceholderMinute + 90,
				bCompactLayout);
		FlightCardSlots.Add(AddAnchored(
			GridCanvas,
			Card,
			FAnchors(
				Geometry.Left,
				Geometry.Top,
				Geometry.Right,
				Geometry.Bottom),
			FMargin(0.0f),
			2 + Day));
	}
	WeatherWindowOverlay = MakeStripedWindow(
		WidgetTree,
		TEXT("RegionalWeatherWindow"),
		WeatherWindowText);
	WeatherWindowSlot = AddAnchored(
		GridCanvas,
		WeatherWindowOverlay,
		FAnchors(0.34f, 0.40f, 0.45f, 0.58f),
		FMargin(),
		10);
	WeatherWindowOverlay->SetVisibility(ESlateVisibility::Collapsed);
	AddAnchored(
		TimetableSurface,
		WeekGrid,
		bCompactLayout
			? FAnchors(0.263f, 0.018f, 0.742f, 0.90f)
			: FAnchors(0.212f, 0.018f, 0.782f, 0.90f));

	UBorder* EvidenceRail = MakeSurface(
		WidgetTree,
		TEXT("RegionalEvidenceRail"),
		ESurface::Panel,
		FMargin(bCompactLayout ? 11.0f : 14.0f),
		17.0f);
	UScrollBox* EvidenceScroll =
		WidgetTree->ConstructWidget<UScrollBox>(
			UScrollBox::StaticClass(),
			TEXT("RegionalEvidenceScroll"));
	UVerticalBox* EvidenceColumn =
		WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			TEXT("RegionalEvidenceColumn"));
	EvidenceScroll->AddChild(EvidenceColumn);
	EvidenceRail->SetContent(EvidenceScroll);
	AddVertical(
		EvidenceColumn,
		MakeText(
			WidgetTree,
			TEXT("RegionalSelectedTitle"),
			TEXT("SELECTED FLIGHT"),
			TitleSize,
			White(),
			true),
		7.0f);
	AddVertical(
		EvidenceColumn,
		MakeMetricCard(
			WidgetTree,
			TEXT("RegionalSelectedFlightCard"),
			SelectedFlightText,
			TEXT("SELECT A FLIGHT"),
			BodySize,
			ESurface::RaisedCard),
		7.0f);
	AddVertical(
		EvidenceColumn,
		MakeMetricCard(
			WidgetTree,
			TEXT("RegionalSelectedDetailCard"),
			SelectedFlightDetailText,
			TEXT("Exact slot and gate"),
			SmallSize),
		9.0f);
	AddVertical(
		EvidenceColumn,
		MakeText(
			WidgetTree,
			TEXT("RegionalFeasibilityTitle"),
			TEXT("FEASIBILITY"),
			BodySize,
			Cyan(),
			true),
		5.0f);
	AddVertical(
		EvidenceColumn,
		MakeMetricCard(
			WidgetTree,
			TEXT("RegionalFeasibilityCard"),
			FeasibilityText,
			TEXT("Runway · stand · services"),
			SmallSize,
			ESurface::Positive),
		7.0f);
	AddVertical(
		EvidenceColumn,
		MakeMetricCard(
			WidgetTree,
			TEXT("RegionalConnectionCard"),
			ConnectionText,
			TEXT("CONNECTIONS · READY"),
			SmallSize),
		6.0f);
	AddVertical(
		EvidenceColumn,
		MakeMetricCard(
			WidgetTree,
			TEXT("RegionalBorderCard"),
			BorderText,
			TEXT("BORDER · READY"),
			SmallSize),
		6.0f);
	AddVertical(
		EvidenceColumn,
		MakeMetricCard(
			WidgetTree,
			TEXT("RegionalTransportCard"),
			TransportText,
			TEXT("RENTAL · RAIL"),
			SmallSize),
		6.0f);
	AddVertical(
		EvidenceColumn,
		MakeMetricCard(
			WidgetTree,
			TEXT("RegionalRenewalCard"),
			RenewalText,
			TEXT("TENANTS · MONITORING"),
			SmallSize),
		8.0f);
	ReviewGateButton = MakeButton(
		WidgetTree,
		TEXT("RegionalReviewGate"),
		TEXT("REVIEW GATE CHANGE"),
		EButton::Secondary,
		SmallSize);
	ReviewGateButton->OnClicked.AddDynamic(
		this,
		&UAMSimRegionalOperationsView::ReviewGateChange);
	AddVertical(EvidenceColumn, ReviewGateButton, 7.0f);
	ConfirmGateButton = MakeButton(
		WidgetTree,
		TEXT("RegionalConfirmGate"),
		TEXT("CONFIRM HIGH-RISK CHANGE"),
		EButton::Destructive,
		SmallSize);
	ConfirmGateButton->OnClicked.AddDynamic(
		this,
		&UAMSimRegionalOperationsView::ConfirmGateChange);
	AddVertical(EvidenceColumn, ConfirmGateButton, 7.0f);
	RenewalButton = MakeButton(
		WidgetTree,
		TEXT("RegionalRenewal"),
		TEXT("ACCEPT TENANT RENEWAL"),
		EButton::Positive,
		SmallSize);
	RenewalButton->OnClicked.AddDynamic(
		this,
		&UAMSimRegionalOperationsView::AcceptRenewal);
	AddVertical(EvidenceColumn, RenewalButton, 7.0f);
	AddAnchored(
		TimetableSurface,
		EvidenceRail,
		bCompactLayout
			? FAnchors(0.752f, 0.018f, 0.988f, 0.90f)
			: FAnchors(0.792f, 0.018f, 0.988f, 0.90f));

	UBorder* TimetableFooter = MakeSurface(
		WidgetTree,
		TEXT("RegionalTimetableFooter"),
		ESurface::Chrome,
		FMargin(12.0f, 7.0f),
		13.0f);
	UHorizontalBox* FooterRow =
		WidgetTree->ConstructWidget<UHorizontalBox>(
			UHorizontalBox::StaticClass(),
			TEXT("RegionalFooterRow"));
	TimetableFooter->SetContent(FooterRow);
	InteractionText = MakeText(
		WidgetTree,
		TEXT("RegionalInteraction"),
		TEXT("Exact five-minute slots · locked horizon 30 minutes"),
		SmallSize,
		Muted());
	AddHorizontal(FooterRow, InteractionText, 10.0f, true);
	UButton* SaveButton = MakeButton(
		WidgetTree,
		TEXT("RegionalSave"),
		TEXT("SAVE"),
		EButton::Quiet,
		SmallSize);
	SaveButton->OnClicked.AddDynamic(
		this,
		&UAMSimRegionalOperationsView::SaveGame);
	AddHorizontal(FooterRow, SaveButton, 7.0f);
	UButton* LoadButton = MakeButton(
		WidgetTree,
		TEXT("RegionalLoad"),
		TEXT("LOAD"),
		EButton::Quiet,
		SmallSize);
	LoadButton->OnClicked.AddDynamic(
		this,
		&UAMSimRegionalOperationsView::LoadGame);
	AddHorizontal(FooterRow, LoadButton, 0.0f);
	AddAnchored(
		TimetableSurface,
		TimetableFooter,
		bCompactLayout
			? FAnchors(0.012f, 0.895f, 0.988f, 0.985f)
			: FAnchors(0.012f, 0.915f, 0.988f, 0.985f));

	IncidentSurface = WidgetTree->ConstructWidget<UCanvasPanel>(
		UCanvasPanel::StaticClass(),
		TEXT("RegionalIncidentSurface"));
	AddAnchored(
		Root,
		IncidentSurface,
		FAnchors(0.0f, 0.085f, 1.0f, 1.0f));

	UBorder* WeatherRail = MakeSurface(
		WidgetTree,
		TEXT("RegionalWeatherRail"),
		ESurface::Panel,
		FMargin(bCompactLayout ? 11.0f : 15.0f),
		17.0f);
	UVerticalBox* WeatherColumn =
		WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			TEXT("RegionalWeatherColumn"));
	WeatherRail->SetContent(WeatherColumn);
	UHorizontalBox* WeatherTitleRow =
		WidgetTree->ConstructWidget<UHorizontalBox>(
			UHorizontalBox::StaticClass(),
			TEXT("RegionalWeatherTitleRow"));
	USizeBox* WeatherIconBox = WidgetTree->ConstructWidget<USizeBox>(
		USizeBox::StaticClass(),
		TEXT("RegionalWeatherIconSize"));
	WeatherIconBox->SetWidthOverride(34.0f);
	WeatherIconBox->SetHeightOverride(34.0f);
	UImage* WeatherIcon = WidgetTree->ConstructWidget<UImage>(
		UImage::StaticClass(),
		TEXT("RegionalWeatherIcon"));
	WeatherIcon->SetBrushFromTexture(WeatherIconTexture, true);
	WeatherIcon->SetColorAndOpacity(Cyan());
	WeatherIconBox->SetContent(WeatherIcon);
	AddHorizontal(WeatherTitleRow, WeatherIconBox, 8.0f);
	AddHorizontal(
		WeatherTitleRow,
		MakeText(
			WidgetTree,
			TEXT("RegionalWeatherTitle"),
			TEXT("WEATHER & OPERATIONS"),
			TitleSize,
			White(),
			true),
		0.0f,
		true);
	AddVertical(WeatherColumn, WeatherTitleRow, 8.0f);
	AddVertical(
		WeatherColumn,
		MakeMetricCard(
			WidgetTree,
			TEXT("RegionalWeatherCard"),
			WeatherText,
			TEXT("RAIN · RUNWAY WET"),
			BodySize,
			ESurface::RaisedCard),
		9.0f);
	AddVertical(
		WeatherColumn,
		MakeText(
			WidgetTree,
			TEXT("RegionalForecastTitle"),
			TEXT("OPERATIONAL FORECAST"),
			BodySize,
			Cyan(),
			true),
		5.0f);
	AddVertical(
		WeatherColumn,
		MakeMetricCard(
			WidgetTree,
			TEXT("RegionalForecastCard"),
			ForecastText,
			TEXT("Confidence and runway recommendation"),
			SmallSize),
		9.0f);
	ForecastConfidenceBar = WidgetTree->ConstructWidget<UProgressBar>(
		UProgressBar::StaticClass(),
		TEXT("RegionalForecastConfidence"));
	ForecastConfidenceBar->SetFillColorAndOpacity(AMSim::UITheme::Green());
	ForecastConfidenceBar->SetPercent(0.0f);
	AddVertical(WeatherColumn, ForecastConfidenceBar, 9.0f);
	AddVertical(
		WeatherColumn,
		MakeText(
			WidgetTree,
			TEXT("RegionalWeatherEvidence"),
			TEXT("TEMPERATURE / NOT MODELED\nLIMITS / AIRCRAFT-SPECIFIC\nSOURCE / DETERMINISTIC FORECAST"),
			SmallSize,
			Muted()),
		0.0f);
	AddAnchored(
		IncidentSurface,
		WeatherRail,
		bCompactLayout
			? FAnchors(0.012f, 0.018f, 0.252f, 0.70f)
			: FAnchors(0.012f, 0.018f, 0.202f, 0.70f),
		FMargin(0.0f),
		10);

	UBorder* CaptionCard = MakeSurface(
		WidgetTree,
		TEXT("RegionalCaptionCard"),
		ESurface::RaisedCard,
		FMargin(13.0f, 8.0f),
		14.0f);
	CaptionText = MakeText(
		WidgetTree,
		TEXT("RegionalCaption"),
		TEXT("AIRPORT 1: Rescue vehicles entering Runway 27."),
		BodySize,
		White(),
		true);
	CaptionText->SetJustification(ETextJustify::Center);
	CaptionCard->SetContent(CaptionText);
	AddAnchored(
		IncidentSurface,
		CaptionCard,
		bCompactLayout
			? FAnchors(0.27f, 0.02f, 0.73f, 0.10f)
			: FAnchors(0.32f, 0.02f, 0.68f, 0.10f),
		FMargin(0.0f),
		11);

	UBorder* IncidentRail = MakeSurface(
		WidgetTree,
		TEXT("RegionalIncidentRail"),
		ESurface::Panel,
		FMargin(bCompactLayout ? 11.0f : 15.0f),
		17.0f);
	UVerticalBox* IncidentColumn =
		WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			TEXT("RegionalIncidentColumn"));
	IncidentRail->SetContent(IncidentColumn);
	AddVertical(
		IncidentColumn,
		MakeText(
			WidgetTree,
			TEXT("RegionalIncidentTitle"),
			TEXT("INCIDENT RESPONSE"),
			TitleSize,
			White(),
			true),
		7.0f);
	AddVertical(
		IncidentColumn,
		MakeMetricCard(
			WidgetTree,
			TEXT("RegionalIncidentHeadlineCard"),
			IncidentHeadlineText,
			TEXT("RUNWAY 27 · ALERTED"),
			BodySize,
			ESurface::Danger),
		8.0f);
	AddVertical(
		IncidentColumn,
		MakeMetricCard(
			WidgetTree,
			TEXT("RegionalIncidentLifecycleCard"),
			IncidentLifecycleText,
			TEXT("ALERTED ✓ · DISPATCHED ○"),
			SmallSize,
			ESurface::RaisedCard),
		8.0f);
	AddVertical(
		IncidentColumn,
		MakeMetricCard(
			WidgetTree,
			TEXT("RegionalIncidentConsequenceCard"),
			IncidentConsequenceText,
			TEXT("Affected runway closed. Other operations continue."),
			SmallSize,
			ESurface::Danger),
		9.0f);
	HoldButton = MakeButton(
		WidgetTree,
		TEXT("RegionalHold"),
		TEXT("HOLD DEPARTURES"),
		EButton::Secondary,
		SmallSize);
	HoldButton->OnClicked.AddDynamic(
		this,
		&UAMSimRegionalOperationsView::HoldDepartures);
	AddVertical(IncidentColumn, HoldButton, 6.0f);
	DivertButton = MakeButton(
		WidgetTree,
		TEXT("RegionalDivert"),
		TEXT("DIVERT ARRIVALS"),
		EButton::Secondary,
		SmallSize);
	DivertButton->OnClicked.AddDynamic(
		this,
		&UAMSimRegionalOperationsView::DivertArrivals);
	AddVertical(IncidentColumn, DivertButton, 6.0f);
	TowButton = MakeButton(
		WidgetTree,
		TEXT("RegionalTow"),
		TEXT("DEPLOY TOW TEAM"),
		EButton::Primary,
		SmallSize);
	TowButton->OnClicked.AddDynamic(
		this,
		&UAMSimRegionalOperationsView::DispatchTow);
	AddVertical(IncidentColumn, TowButton, 6.0f);
	ProtectButton = MakeButton(
		WidgetTree,
		TEXT("RegionalProtect"),
		TEXT("PROTECT AREA"),
		EButton::Positive,
		SmallSize);
	ProtectButton->OnClicked.AddDynamic(
		this,
		&UAMSimRegionalOperationsView::ProtectArea);
	AddVertical(IncidentColumn, ProtectButton, 6.0f);
	ReviewIncidentButton = MakeButton(
		WidgetTree,
		TEXT("RegionalReviewIncident"),
		TEXT("REVIEW CAUSE"),
		EButton::Primary,
		SmallSize);
	ReviewIncidentButton->OnClicked.AddDynamic(
		this,
		&UAMSimRegionalOperationsView::ReviewIncident);
	AddVertical(IncidentColumn, ReviewIncidentButton, 6.0f);
	RecoveryButton = MakeButton(
		WidgetTree,
		TEXT("RegionalRecovery"),
		TEXT("APPLY RECOVERY PLAN"),
		EButton::Positive,
		SmallSize);
	RecoveryButton->OnClicked.AddDynamic(
		this,
		&UAMSimRegionalOperationsView::ApplyRecovery);
	AddVertical(IncidentColumn, RecoveryButton, 0.0f);
	AddAnchored(
		IncidentSurface,
		IncidentRail,
		bCompactLayout
			? FAnchors(0.752f, 0.018f, 0.988f, 0.70f)
			: FAnchors(0.792f, 0.018f, 0.988f, 0.70f),
		FMargin(0.0f),
		10);

	UBorder* ResponseTray = MakeSurface(
		WidgetTree,
		TEXT("RegionalResponseTray"),
		ESurface::Chrome,
		FMargin(bCompactLayout ? 12.0f : 16.0f),
		17.0f,
		1.8f);
	UHorizontalBox* ResponseRow =
		WidgetTree->ConstructWidget<UHorizontalBox>(
			UHorizontalBox::StaticClass(),
			TEXT("RegionalResponseRow"));
	ResponseTray->SetContent(ResponseRow);
	UVerticalBox* CauseColumn =
		WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			TEXT("RegionalCauseColumn"));
	IncidentCauseText = MakeText(
		WidgetTree,
		TEXT("RegionalIncidentCause"),
		TEXT("CAUSE · warned operational risk"),
		SmallSize,
		White(),
		true);
	AddVertical(CauseColumn, IncidentCauseText, 5.0f);
	IncidentRemedyText = MakeText(
		WidgetTree,
		TEXT("RegionalIncidentRemedy"),
		TEXT("REMEDY · keep the response route clear"),
		SmallSize,
		CyanSoft());
	AddVertical(CauseColumn, IncidentRemedyText, 0.0f);
	AddHorizontal(ResponseRow, CauseColumn, 12.0f, true);
	AddAnchored(
		IncidentSurface,
		ResponseTray,
		bCompactLayout
			? FAnchors(0.16f, 0.73f, 0.84f, 0.90f)
			: FAnchors(0.20f, 0.73f, 0.80f, 0.90f),
		FMargin(0.0f),
		12);

	OverviewView =
		WidgetTree->ConstructWidget<UAMSimOverviewView>(
			UAMSimOverviewView::StaticClass(),
			TEXT("RegionalOverviewView"));
	OverviewView->OnOpenTimetable.BindUObject(
		this,
		&UAMSimRegionalOperationsView::ToggleOverview);
	OverviewView->OnOpenCapabilities.BindUObject(
		this,
		&UAMSimRegionalOperationsView::ToggleProgression);
	OverviewView->OnSelectAircraft.BindUObject(
		this,
		&UAMSimRegionalOperationsView::SelectOverviewAircraft);
	OverviewView->OnSelectFacility.BindUObject(
		this,
		&UAMSimRegionalOperationsView::SelectOverviewFacility);
	AddAnchored(
		Root,
		OverviewView,
		FAnchors(0.0f, 0.085f, 1.0f, 1.0f),
		FMargin(0.0f),
		35);
	ProgressionView =
		WidgetTree->ConstructWidget<UAMSimProgressionView>(
			UAMSimProgressionView::StaticClass(),
			TEXT("RegionalProgressionView"));
	AddAnchored(
		Root,
		ProgressionView,
		FAnchors(0.0f, 0.085f, 1.0f, 1.0f),
		FMargin(0.0f),
		40);
	Phase5View = WidgetTree->ConstructWidget<UAMSimPhase5View>(
		UAMSimPhase5View::StaticClass(),
		TEXT("Phase5OperationsView"));
	Phase5View->OnReturnRequested.BindUObject(
		this,
		&UAMSimRegionalOperationsView::CloseAdvancedOperations);
	AddAnchored(
		Root,
		Phase5View,
		FAnchors(0.0f, 0.0f, 1.0f, 1.0f),
		FMargin(0.0f),
		100);
	InitializePhase6View();

	SetVisibility(ESlateVisibility::Collapsed);
	bProgressionOpen = FParse::Param(
		FCommandLine::Get(),
		TEXT("AMSimPhase45ProgressionProof"));
	bOverviewOpen =
		!bProgressionOpen &&
		FParse::Param(FCommandLine::Get(), TEXT("AMSimPhase45OverviewProof"));
	TimetableSurface->SetVisibility(
		bProgressionOpen || bOverviewOpen
			? ESlateVisibility::Collapsed
			: ESlateVisibility::Visible);
	IncidentSurface->SetVisibility(ESlateVisibility::Collapsed);
	OverviewView->SetVisibility(
		bOverviewOpen
			? ESlateVisibility::SelfHitTestInvisible
			: ESlateVisibility::Collapsed);
	ProgressionView->SetVisibility(
		bProgressionOpen
			? ESlateVisibility::SelfHitTestInvisible
			: ESlateVisibility::Collapsed);
	RefreshFromSimulation();
	return Super::RebuildWidget();
}

bool UAMSimRegionalOperationsView::Submit(
	AMSim::FPhase4Command Command,
	const FString& SuccessMessage)
{
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()
			? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
			: nullptr;
	if (!Subsystem)
	{
		return false;
	}
	const AMSim::EPhase4CommandResult Result =
		Subsystem->SubmitPhase4Command(Command);
	const bool bAccepted =
		Result == AMSim::EPhase4CommandResult::Accepted;
	if (InteractionText)
	{
		InteractionText->SetText(FText::FromString(
			bAccepted
				? SuccessMessage
				: TEXT("That regional action is not ready yet.")));
		InteractionText->SetColorAndOpacity(FSlateColor(
			bAccepted ? AMSim::UITheme::Cyan() : AMSim::UITheme::Coral()));
	}
	if (bAccepted)
	{
		RefreshFromSimulation();
	}
	return bAccepted;
}

void UAMSimRegionalOperationsView::InitializeRegionalAirport()
{
	AMSim::FPhase4Command Command;
	Command.Type =
		AMSim::EPhase4CommandType::InitializeRegionalAirport;
	Submit(Command, TEXT("Regional planning opened with three recurring offers."));
}

void UAMSimRegionalOperationsView::AcceptNextContract()
{
	AMSim::FPhase4Command Command;
	Command.Type =
		AMSim::EPhase4CommandType::AcceptNextRecurringContract;
	Submit(Command, TEXT("Recurring operator contract accepted."));
}

void UAMSimRegionalOperationsView::PublishTimetable()
{
	AMSim::FPhase4Command Command;
	Command.Type =
		AMSim::EPhase4CommandType::PublishSevenDayTimetable;
	Submit(Command, TEXT("Seven-day timetable published in exact slots."));
}

void UAMSimRegionalOperationsView::ToggleProgression()
{
	if (ViewState.bIncidentMode || !ProgressionView)
	{
		return;
	}
	bProgressionOpen = !bProgressionOpen;
	bOverviewOpen = false;
	ProgressionView->RefreshFromSimulation();
	ProgressionView->SetVisibility(
		bProgressionOpen
			? ESlateVisibility::SelfHitTestInvisible
			: ESlateVisibility::Collapsed);
	TimetableSurface->SetVisibility(
		bProgressionOpen
			? ESlateVisibility::Collapsed
			: ESlateVisibility::SelfHitTestInvisible);
	OverviewView->SetVisibility(ESlateVisibility::Collapsed);
	for (TActorIterator<AAMSimWorldPresenter> It(GetWorld()); It; ++It)
	{
		It->SetMatureOverviewMode(false);
		break;
	}
}

void UAMSimRegionalOperationsView::ToggleOverview()
{
	if (ViewState.bIncidentMode || !OverviewView)
	{
		return;
	}
	bOverviewOpen = !bOverviewOpen;
	bProgressionOpen = false;
	OverviewView->RefreshFromSimulation();
	OverviewView->SetVisibility(
		bOverviewOpen
			? ESlateVisibility::SelfHitTestInvisible
			: ESlateVisibility::Collapsed);
	TimetableSurface->SetVisibility(
		bOverviewOpen
			? ESlateVisibility::Collapsed
			: ESlateVisibility::SelfHitTestInvisible);
	ProgressionView->SetVisibility(ESlateVisibility::Collapsed);
	for (TActorIterator<AAMSimWorldPresenter> It(GetWorld()); It; ++It)
	{
		It->SetMatureOverviewMode(bOverviewOpen);
		break;
	}
}

void UAMSimRegionalOperationsView::SelectOverviewAircraft()
{
	for (TActorIterator<AAMSimWorldPresenter> It(GetWorld()); It; ++It)
	{
		It->SetMatureSelectionFacility(false);
		break;
	}
}

void UAMSimRegionalOperationsView::SelectOverviewFacility()
{
	for (TActorIterator<AAMSimWorldPresenter> It(GetWorld()); It; ++It)
	{
		It->SetMatureSelectionFacility(true);
		break;
	}
}

const AMSim::FPhase4FlightRecord*
UAMSimRegionalOperationsView::GetDisruptedFlight() const
{
	const UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()
			? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
			: nullptr;
	if (!Subsystem)
	{
		return nullptr;
	}
	return Subsystem->GetSimulation().GetPhase4State().Flights.FindByPredicate(
		[](const AMSim::FPhase4FlightRecord& Flight)
			{
				return Flight.DayIndex == 3 &&
					Flight.bWeatherRestricted;
			});
}

void UAMSimRegionalOperationsView::ReviewGateChange()
{
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()
			? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
			: nullptr;
	const AMSim::FPhase4FlightRecord* Flight = GetDisruptedFlight();
	if (!Subsystem || !Flight)
	{
		return;
	}
	AMSim::FPhase4Command Command;
	Command.Type = AMSim::EPhase4CommandType::ReassignDisruptedFlight;
	Command.FlightId = Flight->Id;
	Command.RequestedGateId = TEXT("R1");
	const AMSim::EPhase4CommandResult Result =
		Subsystem->SubmitPhase4Command(Command);
	bGateWarningReviewed =
		Result == AMSim::EPhase4CommandResult::AcceptedWithWarning;
	if (InteractionText)
	{
		InteractionText->SetText(FText::FromString(
			bGateWarningReviewed
				? TEXT("HIGH RISK · inside the locked horizon. Confirm remote Stand R1 with bus boarding.")
				: TEXT("The gate change is not available.")));
		InteractionText->SetColorAndOpacity(FSlateColor(
			bGateWarningReviewed
				? AMSim::UITheme::Amber()
				: AMSim::UITheme::Coral()));
	}
	SetActionVisible(ReviewGateButton, !bGateWarningReviewed);
	SetActionVisible(ConfirmGateButton, bGateWarningReviewed);
}

void UAMSimRegionalOperationsView::ConfirmGateChange()
{
	const AMSim::FPhase4FlightRecord* Flight = GetDisruptedFlight();
	if (!Flight)
	{
		return;
	}
	AMSim::FPhase4Command Command;
	Command.Type =
		AMSim::EPhase4CommandType::ConfirmHighRiskGateChange;
	Command.FlightId = Flight->Id;
	Command.RequestedGateId = TEXT("R1");
	Command.bConfirmWarning = true;
	if (Submit(
		Command,
		TEXT("Gate changed to R1; passenger, bag, service, and bus routes updated.")))
	{
		bGateWarningReviewed = false;
	}
}

void UAMSimRegionalOperationsView::HoldDepartures()
{
	AMSim::FPhase4Command Command;
	Command.Type =
		AMSim::EPhase4CommandType::HoldAffectedDepartures;
	Submit(Command, TEXT("Affected departures held."));
}

void UAMSimRegionalOperationsView::DivertArrivals()
{
	AMSim::FPhase4Command Command;
	Command.Type =
		AMSim::EPhase4CommandType::DivertAffectedArrivals;
	Submit(Command, TEXT("Arrivals diverted from Runway 27."));
}

void UAMSimRegionalOperationsView::DispatchTow()
{
	AMSim::FPhase4Command Command;
	Command.Type = AMSim::EPhase4CommandType::DeployTowTeam;
	Submit(Command, TEXT("Tow and airport operations teams dispatched."));
}

void UAMSimRegionalOperationsView::ProtectArea()
{
	AMSim::FPhase4Command Command;
	Command.Type = AMSim::EPhase4CommandType::ProtectIncidentArea;
	Submit(Command, TEXT("Area protected · no injuries · other operations continue."));
}

void UAMSimRegionalOperationsView::ReviewIncident()
{
	AMSim::FPhase4Command Command;
	Command.Type = AMSim::EPhase4CommandType::ReviewIncidentCause;
	Submit(Command, TEXT("Cause report reviewed with prevention action."));
}

void UAMSimRegionalOperationsView::ApplyRecovery()
{
	AMSim::FPhase4Command Command;
	Command.Type = AMSim::EPhase4CommandType::ApplyRecoveryPlan;
	Submit(Command, TEXT("Runway reopened after tow, inspection, and recovery support."));
}

void UAMSimRegionalOperationsView::AcceptRenewal()
{
	AMSim::FPhase4Command Command;
	Command.Type =
		AMSim::EPhase4CommandType::AcceptTenantRenewal;
	Submit(Command, TEXT("Riverbend Connect renewal accepted."));
}

void UAMSimRegionalOperationsView::SubmitSpeed(
	const int32 Multiplier,
	const bool bPaused)
{
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()
			? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
			: nullptr;
	if (!Subsystem)
	{
		return;
	}
	AMSim::FPhase1Command Command;
	Command.Type = bPaused
		? AMSim::EPhase1CommandType::SetPaused
		: AMSim::EPhase1CommandType::SetSpeed;
	Command.bPaused = bPaused;
	Command.SpeedMultiplier = Multiplier;
	Subsystem->SubmitPhase1Command(Command);
}

void UAMSimRegionalOperationsView::SaveGame()
{
	UAMSimAirportSimulationSubsystem* Simulation =
		GetWorld()
			? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
			: nullptr;
	UAMSimGameInstanceSubsystem* GameInstance =
		GetGameInstance()
			? GetGameInstance()->GetSubsystem<UAMSimGameInstanceSubsystem>()
			: nullptr;
	if (!Simulation || !GameInstance)
	{
		return;
	}
	AMSim::FSaveMetadata Metadata;
	Metadata.PlayerLabel = TEXT("Regional week");
	Metadata.AirportName = TEXT("Riverbend Airport");
	const AMSim::FSaveResult Result =
		GameInstance->SaveSnapshotAsync(
			TEXT("RegionalAutosave"),
			Simulation->CreateSnapshot(),
			MoveTemp(Metadata)).Get();
	if (InteractionText)
	{
		InteractionText->SetText(FText::FromString(
			Result.bSucceeded
				? TEXT("Regional week saved.")
				: TEXT("The regional save could not be written.")));
	}
}

void UAMSimRegionalOperationsView::LoadGame()
{
	UAMSimAirportSimulationSubsystem* Simulation =
		GetWorld()
			? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
			: nullptr;
	UAMSimGameInstanceSubsystem* GameInstance =
		GetGameInstance()
			? GetGameInstance()->GetSubsystem<UAMSimGameInstanceSubsystem>()
			: nullptr;
	if (!Simulation || !GameInstance)
	{
		return;
	}
	AMSim::FSnapshot Snapshot;
	bool bUsedBackup = false;
	const bool bLoaded =
		GameInstance->LoadSnapshot(
			TEXT("RegionalAutosave"),
			Snapshot,
			bUsedBackup) &&
		Simulation->RestoreSnapshot(Snapshot);
	if (InteractionText)
	{
		InteractionText->SetText(FText::FromString(
			bLoaded
				? bUsedBackup
					? TEXT("Backup regional save restored.")
					: TEXT("Regional save restored.")
				: TEXT("No valid regional save was found.")));
	}
}

void UAMSimRegionalOperationsView::RefreshFromSimulation()
{
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()
			? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
			: nullptr;
	if (!Subsystem)
	{
		return;
	}
	const AMSim::FPhase4QuerySnapshot Query =
		Subsystem->GetPhase4Query();
	const AMSim::FPhase4State& State =
		Subsystem->GetSimulation().GetPhase4State();
	const AMSim::FPhase5QuerySnapshot Phase5Query =
		Subsystem->GetPhase5Query();
	const AMSim::FPhase6QuerySnapshot Phase6Query =
		Subsystem->GetPhase6Query();
	if (Phase5View)
	{
		Phase5View->RefreshFromSimulation();
	}
	if (RefreshPhase6View(Phase6Query))
	{
		return;
	}
	const bool bAdvancedAvailable =
		Phase5Query.bUnlocked || Phase5Query.bInitialized;
	if (!bAdvancedAvailable)
	{
		bAdvancedOperationsOpen = false;
	}
	if (AdvancedOperationsButton)
	{
		AdvancedOperationsButton->SetVisibility(
			bAdvancedAvailable
				? ESlateVisibility::Visible
				: ESlateVisibility::Collapsed);
	}
	if (bAdvancedAvailable && bAdvancedOperationsOpen)
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		if (Phase5View)
		{
			Phase5View->SetVisibility(
				ESlateVisibility::SelfHitTestInvisible);
		}
		return;
	}
	if (!Query.bUnlocked && !Query.bInitialized)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if (Phase5View)
	{
		Phase5View->SetVisibility(ESlateVisibility::Collapsed);
	}
	UpdateWorldPresentation(Query, State);
	if (ProgressionView)
	{
		ProgressionView->RefreshFromSimulation();
	}
	if (OverviewView)
	{
		OverviewView->RefreshFromSimulation();
	}
	if (ViewState.Revision == Query.Revision)
	{
		return;
	}
	ViewState = AMSim::MakePhase4ViewState(Query, State);
	const AMSim::FPhase4FlightRecord* CompactSelected =
		State.Flights.FindByPredicate(
			[&State](const AMSim::FPhase4FlightRecord& Flight)
				{
					return State.Incident.Lifecycle >=
							AMSim::EPhase4IncidentLifecycle::Alerted
						? Flight.Id == State.Incident.FlightId
						: Flight.DayIndex == 3 &&
							Flight.bWeatherRestricted;
				});
	if (!CompactSelected && !State.Flights.IsEmpty())
	{
		CompactSelected = &State.Flights[0];
	}
	StatusText->SetText(FText::FromString(ViewState.Status));
	TimetableText->SetText(FText::FromString(ViewState.Timetable));
	SelectedFlightText->SetText(
		FText::FromString(
			bCompactLayout
				? ViewState.SelectedFlight
					.Replace(TEXT("NORTHSTAR REGIONAL"), TEXT("NORTHSTAR"))
					.Replace(TEXT("RIVERBEND CONNECT"), TEXT("RIVERBEND"))
					.Replace(TEXT("COASTAL AIRWAYS"), TEXT("COASTAL"))
				: ViewState.SelectedFlight));
	SelectedFlightDetailText->SetText(
		FText::FromString(
			bCompactLayout && CompactSelected
				? FString::Printf(
					TEXT("D%d · %s–%s · %s"),
					CompactSelected->DayIndex,
					*MinuteDisplay(
						CompactSelected->PlannedArrivalMinute),
					*MinuteDisplay(
						CompactSelected->PlannedDepartureMinute),
					*CompactSelected->AssignedGateId.ToString())
				: ViewState.SelectedFlightDetail));
	FeasibilityText->SetText(FText::FromString(
		bCompactLayout && CompactSelected
			? FString::Printf(
				TEXT("RWY✓ %s✓ SVC✓ %s"),
				*CompactSelected->AssignedGateId.ToString(),
				CompactSelected->Feasibility ==
						AMSim::EPhase4Feasibility::HighRisk
					? TEXT("RISK !")
					: TEXT("READY ✓"))
			: ViewState.Feasibility));
	ConnectionText->SetText(FText::FromString(
		bCompactLayout
			? FString::Printf(
				TEXT("%d CONN · %d MISS · %d/%d BAG"),
				Query.ConnectedPassengerCount,
				Query.MissedConnectionPassengerCount,
				Query.CompletedTransferBagCount,
				Query.TransferBagCount)
			: ViewState.Connections));
	BorderText->SetText(FText::FromString(
		bCompactLayout
			? FString::Printf(
				TEXT("INTL · %d/%d BORDER"),
				Query.BorderProcessedPassengerCount,
				Query.InternationalPassengerCount)
			: ViewState.Border));
	TransportText->SetText(FText::FromString(
		bCompactLayout
			? FString::Printf(
				TEXT("RENTAL %d · RAIL %d"),
				Query.RentalCarPassengerCount,
				Query.RailPassengerCount)
			: ViewState.Transport));
	const AMSim::FPhase4ForecastRecord* CompactForecast =
		State.Forecast.FindByPredicate(
			[&Query](const AMSim::FPhase4ForecastRecord& Candidate)
				{
					return Candidate.DayIndex ==
						FMath::Max(1, Query.CurrentOperatingDay);
				});
	WeatherText->SetText(FText::FromString(
		bCompactLayout && CompactForecast
			? FString::Printf(
				TEXT("%s · %.1f KM · %s"),
				CompactForecast->Category ==
						AMSim::EPhase4WeatherCategory::Rain
					? TEXT("RAIN")
					: TEXT("WEATHER"),
				static_cast<double>(
					CompactForecast->VisibilityMeters) / 1000.0,
				*CompactForecast->RunwaySurface.ToString().ToUpper())
			: ViewState.Weather));
	ForecastText->SetText(FText::FromString(
		bCompactLayout && CompactForecast
			? FString::Printf(
				TEXT("6H · %d%% · RWY %s"),
				CompactForecast->ConfidencePercent,
				*CompactForecast->RecommendedRunway.ToString())
			: ViewState.Forecast));
	if (CompactForecast)
	{
		const TCHAR* Category =
			CompactForecast->Category ==
					AMSim::EPhase4WeatherCategory::LowVisibility
				? TEXT("LOW VISIBILITY")
				: CompactForecast->Category ==
							AMSim::EPhase4WeatherCategory::Rain
					? TEXT("RAIN")
					: CompactForecast->Category ==
								AMSim::EPhase4WeatherCategory::StrongWind
						? TEXT("STRONG WIND")
						: TEXT("CLEAR");
		const TCHAR* Implication =
			CompactForecast->Category ==
					AMSim::EPhase4WeatherCategory::LowVisibility ||
				CompactForecast->Category ==
					AMSim::EPhase4WeatherCategory::Rain
				? TEXT("APPROACH LIMITS ACTIVE")
				: CompactForecast->Category ==
							AMSim::EPhase4WeatherCategory::StrongWind
					? TEXT("CROSSWIND REVIEW")
					: TEXT("NORMAL OPERATIONS");
		WeatherText->SetText(FText::FromString(FString::Printf(
			TEXT("CURRENT / %s\nWIND / %03d DEG / %d KT\nVIS / %.1f KM\nSURFACE / %s"),
			Category,
			CompactForecast->WindDirectionDegrees,
			CompactForecast->WindSpeedKnots,
			static_cast<double>(CompactForecast->VisibilityMeters) / 1000.0,
			*CompactForecast->RunwaySurface.ToString().ToUpper())));
		ForecastText->SetText(FText::FromString(FString::Printf(
			TEXT("D%d +%02dH / %s\n%03d DEG / %d KT / %.1f KM\nCONFIDENCE / %d%%\nRWY %s / %s\n%s"),
			CompactForecast->DayIndex,
			CompactForecast->HourOffset,
			Category,
			CompactForecast->WindDirectionDegrees,
			CompactForecast->WindSpeedKnots,
			static_cast<double>(CompactForecast->VisibilityMeters) / 1000.0,
			CompactForecast->ConfidencePercent,
			*CompactForecast->RecommendedRunway.ToString(),
			*CompactForecast->RunwaySurface.ToString().ToUpper(),
			Implication)));
		if (ForecastConfidenceBar)
		{
			ForecastConfidenceBar->SetPercent(
				FMath::Clamp(
					CompactForecast->ConfidencePercent / 100.0f,
					0.0f,
					1.0f));
		}
	}
	IncidentHeadlineText->SetText(
		FText::FromString(
			bCompactLayout
				? State.Incident.bTowDispatched
					? TEXT("RWY 27 · TOW SENT")
					: TEXT("RWY 27 · ALERT")
				: ViewState.IncidentHeadline));
	IncidentCauseText->SetText(FText::FromString(FString::Printf(
		TEXT("CAUSE · %s"),
		*ViewState.IncidentCause)));
	IncidentConsequenceText->SetText(
		FText::FromString(
			bCompactLayout
				? State.Incident.bRunwayClosed
					? TEXT("CLOSED · OTHER OPS OPEN")
					: TEXT("RWY 27 OPEN")
				: ViewState.IncidentConsequence));
	IncidentRemedyText->SetText(FText::FromString(FString::Printf(
		TEXT("REMEDY · %s"),
		*ViewState.IncidentRemedy)));
	IncidentLifecycleText->SetText(
		FText::FromString(
			bCompactLayout
				? FString::Printf(
					TEXT("ALERT ✓ · TOW %s · SAFE %s"),
					State.Incident.bTowDispatched
						? TEXT("✓")
						: TEXT("○"),
					State.Incident.bAreaProtected
						? TEXT("✓")
						: TEXT("○"))
				: ViewState.IncidentLifecycle));
	CaptionText->SetText(FText::FromString(ViewState.Caption));
	if (UAMSimRadioSubsystem* Radio =
		GetGameInstance()
			? GetGameInstance()->GetSubsystem<
				UAMSimRadioSubsystem>()
			: nullptr)
	{
		Radio->PresentCaption(
			GetWorld(),
			TEXT("Phase4.Operations"),
			ViewState.Caption);
	}
	RenewalText->SetText(FText::FromString(
		bCompactLayout
			? Query.RenewalAcceptedCount > 0
				? TEXT("TENANT · RENEWED")
				: TEXT("TENANTS · MONITOR")
			: ViewState.Renewal));

	const AMSim::FPhase1State& Phase1 =
		Subsystem->GetSimulation().GetPhase1State();
	FundsText->SetText(FText::FromString(FString::Printf(
		TEXT("%lld CR · %d AP"),
		Phase1.Credits,
		Phase1.AirportPoints)));
	const int64 TotalMinutes = Query.GameTimeMilliseconds / 60000;
	ClockText->SetText(FText::FromString(FString::Printf(
		TEXT("DAY %d/7 · %02lld:%02lld"),
		FMath::Max(1, Query.CurrentOperatingDay),
		(TotalMinutes / 60) % 24,
		TotalMinutes % 60)));

	if (ViewState.bIncidentMode)
	{
		bProgressionOpen = false;
		bOverviewOpen = false;
		for (TActorIterator<AAMSimWorldPresenter> It(GetWorld()); It; ++It)
		{
			It->SetMatureOverviewMode(true);
			break;
		}
	}
	const bool bProgressionProof = !ViewState.bIncidentMode &&
		Query.bFixtureCompleted &&
		FParse::Param(FCommandLine::Get(),
			TEXT("AMSimPhase45ProgressionProof"));
	if (bProgressionProof)
	{
		bProgressionOpen = true;
		bOverviewOpen = false;
	}
	const bool bOverviewProof =
		!ViewState.bIncidentMode &&
		Query.bFixtureCompleted &&
		FParse::Param(
			FCommandLine::Get(),
			TEXT("AMSimPhase45OverviewProof"));
	if (bOverviewProof)
	{
		bProgressionOpen = false;
		bOverviewOpen = true;
		const bool bFacilityProof = FParse::Param(
			FCommandLine::Get(),
			TEXT("AMSimPhase45FacilityProof"));
		OverviewView->SetFacilitySelected(bFacilityProof);
		for (TActorIterator<AAMSimWorldPresenter> It(GetWorld()); It; ++It)
		{
			It->SetMatureOverviewMode(true);
			It->SetMatureSelectionFacility(bFacilityProof);
			break;
		}
	}
	TimetableSurface->SetVisibility(
		ViewState.bIncidentMode || bProgressionOpen || bOverviewOpen
			? ESlateVisibility::Collapsed
			: ESlateVisibility::SelfHitTestInvisible);
	IncidentSurface->SetVisibility(
		ViewState.bIncidentMode
			? ESlateVisibility::SelfHitTestInvisible
			: ESlateVisibility::Collapsed);
	ProgressionView->SetVisibility(
		bProgressionOpen
			? ESlateVisibility::SelfHitTestInvisible
			: ESlateVisibility::Collapsed);
	OverviewView->SetVisibility(
		bOverviewOpen
			? ESlateVisibility::SelfHitTestInvisible
			: ESlateVisibility::Collapsed);
	RefreshContractCards(State);
	RefreshFlightCards(State);
	SetActionVisible(InitializeButton, ViewState.bCanInitialize);
	SetActionVisible(
		AcceptContractButton,
		ViewState.bCanAcceptContract);
	SetActionVisible(PublishButton, ViewState.bCanPublish);
	SetActionVisible(
		ReviewGateButton,
		ViewState.bCanReviewGateChange && !bGateWarningReviewed);
	SetActionVisible(
		ConfirmGateButton,
		ViewState.bCanReviewGateChange && bGateWarningReviewed);
	SetActionVisible(HoldButton, ViewState.bCanHold);
	SetActionVisible(DivertButton, ViewState.bCanDivert);
	SetActionVisible(TowButton, ViewState.bCanDispatchTow);
	SetActionVisible(ProtectButton, ViewState.bCanProtect);
	SetActionVisible(
		ReviewIncidentButton,
		ViewState.bCanReviewIncident);
	SetActionVisible(RecoveryButton, ViewState.bCanRecover);
	SetActionVisible(RenewalButton, ViewState.bCanRenew);
	ForceLayoutPrepass();
}
