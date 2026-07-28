#include "AMSimProgressionView.h"

#include "AMSimAirportSimulationSubsystem.h"
#include "AMSimUITheme.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/ButtonSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/Texture2D.h"
#include "Engine/UserInterfaceSettings.h"
#include "UObject/ConstructorHelpers.h"

namespace AMSimProgressionViewPrivate
{
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
		Text->SetLineHeightPercentage(0.94f);
		AMSim::UITheme::StyleText(Text, Size, Color, bBold, true);
		return Text;
	}

	UBorder* MakeSurface(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const AMSim::UITheme::ESurface Surface,
		const FMargin& Padding,
		const float Radius = 15.0f,
		const float Stroke = 1.4f)
	{
		UBorder* Border = Tree->ConstructWidget<UBorder>(
			UBorder::StaticClass(),
			Name);
		AMSim::UITheme::StyleSurface(
			Border,
			Surface,
			Padding,
			Radius,
			Stroke);
		return Border;
	}

	void AddAnchored(
		UCanvasPanel* Canvas,
		UWidget* Widget,
		const FAnchors& Anchors,
		const int32 ZOrder = 0)
	{
		UCanvasPanelSlot* Slot = Canvas->AddChildToCanvas(Widget);
		Slot->SetAnchors(Anchors);
		Slot->SetOffsets(FMargin(0.0f));
		Slot->SetZOrder(ZOrder);
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
}

using namespace AMSimProgressionViewPrivate;

UAMSimProgressionView::UAMSimProgressionView(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> GeneralAviation(
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_GeneralAviation.T_GeneralAviation"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> FlightSchool(
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_FlightSchool.T_FlightSchool"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> Charter(
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_Charter.T_Charter"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> Cargo(
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_Baggage.T_Baggage"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> Passenger(
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_Passengers.T_Passengers"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> MixedAirport(
		TEXT("/Game/Phase45/Presentation/Textures/UI/T_MixedAirport.T_MixedAirport"));
	PathThumbnails = {
		GeneralAviation.Object,
		FlightSchool.Object,
		Charter.Object,
		Cargo.Object,
		Passenger.Object,
		MixedAirport.Object};
}

int32 UAMSimProgressionView::GetLoadedThumbnailCount() const
{
	int32 Count = 0;
	for (const UTexture2D* Thumbnail : PathThumbnails)
	{
		Count += Thumbnail ? 1 : 0;
	}
	return Count;
}

bool UAMSimProgressionView::HasRequiredThumbnailKit() const
{
	return PathThumbnails.Num() == 6 &&
		GetLoadedThumbnailCount() == 6;
}

TSharedRef<SWidget> UAMSimProgressionView::RebuildWidget()
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
	const int32 BodySize = bCompactLayout ? 11 : 14;
	const int32 SmallSize = bCompactLayout ? 9 : 11;

	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(
		UCanvasPanel::StaticClass(),
		TEXT("ProgressionRoot"));
	WidgetTree->RootWidget = Root;
	UBorder* Backdrop = MakeSurface(
		WidgetTree,
		TEXT("ProgressionBackdrop"),
		AMSim::UITheme::ESurface::Chrome,
		FMargin(10.0f),
		18.0f,
		1.8f);
	AddAnchored(Root, Backdrop, FAnchors(0.008f, 0.008f, 0.992f, 0.992f));

	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(
		UCanvasPanel::StaticClass(),
		TEXT("ProgressionCanvas"));
	Backdrop->SetContent(Canvas);
	UTextBlock* Heading = MakeText(
		WidgetTree,
		TEXT("ProgressionHeading"),
		TEXT("AIRPORT CAPABILITIES\nYOUR AIRPORT, YOUR PATH"),
		TitleSize,
		AMSim::UITheme::White(),
		true);
	Heading->SetJustification(ETextJustify::Center);
	AddAnchored(
		Canvas,
		Heading,
		FAnchors(0.20f, 0.012f, 0.80f, 0.105f));
	FundsText = MakeText(
		WidgetTree,
		TEXT("ProgressionFunds"),
		TEXT("0 CR  |  0 RATING  |  0 AP"),
		BodySize,
		AMSim::UITheme::Amber(),
		true);
	FundsText->SetJustification(ETextJustify::Right);
	AddAnchored(
		Canvas,
		FundsText,
		FAnchors(0.67f, 0.02f, 0.985f, 0.085f));

	UBorder* ObjectiveRail = MakeSurface(
		WidgetTree,
		TEXT("ProgressionObjectiveRail"),
		AMSim::UITheme::ESurface::Panel,
		FMargin(bCompactLayout ? 10.0f : 14.0f));
	UVerticalBox* Objectives = WidgetTree->ConstructWidget<UVerticalBox>(
		UVerticalBox::StaticClass(),
		TEXT("ProgressionObjectives"));
	ObjectiveRail->SetContent(Objectives);
	AddVertical(
		Objectives,
		MakeText(
			WidgetTree,
			TEXT("ProgressionObjectiveTitle"),
			TEXT("SUGGESTED OBJECTIVES"),
			BodySize,
			AMSim::UITheme::Amber(),
			true),
		10.0f);
	for (int32 Index = 0; Index < 3; ++Index)
	{
		UBorder* Card = MakeSurface(
			WidgetTree,
			*FString::Printf(TEXT("ProgressionObjective%d"), Index),
			AMSim::UITheme::ESurface::Card,
			FMargin(bCompactLayout ? 8.0f : 11.0f),
			12.0f);
		UVerticalBox* Column = WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			*FString::Printf(TEXT("ProgressionObjectiveColumn%d"), Index));
		Card->SetContent(Column);
		UTextBlock* Text = MakeText(
			WidgetTree,
			*FString::Printf(TEXT("ProgressionObjectiveText%d"), Index),
			TEXT("OBJECTIVE"),
			SmallSize,
			AMSim::UITheme::White(),
			true);
		UProgressBar* Progress =
			WidgetTree->ConstructWidget<UProgressBar>(
				UProgressBar::StaticClass(),
				*FString::Printf(
					TEXT("ProgressionObjectiveProgress%d"),
					Index));
		Progress->SetFillColorAndOpacity(AMSim::UITheme::Amber());
		AddVertical(Column, Text, 8.0f);
		AddVertical(Column, Progress, 0.0f);
		ObjectiveTexts.Add(Text);
		ObjectiveProgress.Add(Progress);
		AddVertical(Objectives, Card, 10.0f);
	}
	AddAnchored(
		Canvas,
		ObjectiveRail,
		FAnchors(0.012f, 0.12f, 0.19f, 0.89f));

	UBorder* Map = MakeSurface(
		WidgetTree,
		TEXT("ProgressionMap"),
		AMSim::UITheme::ESurface::Panel,
		FMargin(10.0f),
		17.0f);
	UCanvasPanel* MapCanvas =
		WidgetTree->ConstructWidget<UCanvasPanel>(
			UCanvasPanel::StaticClass(),
			TEXT("ProgressionMapCanvas"));
	Map->SetContent(MapCanvas);
	const auto AddConnector =
		[this, MapCanvas](const TCHAR* Name, const FAnchors& Anchors)
		{
			UBorder* Connector =
				WidgetTree->ConstructWidget<UBorder>(
					UBorder::StaticClass(),
					Name);
			Connector->SetBrushColor(FLinearColor(
				AMSim::UITheme::Green().R,
				AMSim::UITheme::Green().G,
				AMSim::UITheme::Green().B,
				0.48f));
			AddAnchored(MapCanvas, Connector, Anchors, 0);
		};
	AddConnector(
		TEXT("ProgressionTopRail"),
		FAnchors(0.17f, 0.382f, 0.83f, 0.389f));
	AddConnector(
		TEXT("ProgressionBottomRail"),
		FAnchors(0.17f, 0.611f, 0.83f, 0.618f));
	AddConnector(
		TEXT("ProgressionTopCenter"),
		FAnchors(0.495f, 0.382f, 0.505f, 0.415f));
	AddConnector(
		TEXT("ProgressionBottomCenter"),
		FAnchors(0.495f, 0.585f, 0.505f, 0.618f));
	for (int32 Column = 0; Column < 3; ++Column)
	{
		const float Center = 0.17f + Column * 0.33f;
		AddConnector(
			*FString::Printf(TEXT("ProgressionTopBranch%d"), Column),
			FAnchors(
				Center - 0.005f,
				0.35f,
				Center + 0.005f,
				0.389f));
		AddConnector(
			*FString::Printf(
				TEXT("ProgressionBottomBranch%d"),
				Column),
			FAnchors(
				Center - 0.005f,
				0.611f,
				Center + 0.005f,
				0.65f));
	}
	const FAnchors CardAnchors[] = {
		FAnchors(0.018f, 0.025f, 0.322f, 0.35f),
		FAnchors(0.348f, 0.025f, 0.652f, 0.35f),
		FAnchors(0.678f, 0.025f, 0.982f, 0.35f),
		FAnchors(0.018f, 0.65f, 0.322f, 0.975f),
		FAnchors(0.348f, 0.65f, 0.652f, 0.975f),
		FAnchors(0.678f, 0.65f, 0.982f, 0.975f)};
	for (int32 Index = 0; Index < 6; ++Index)
	{
		UBorder* Card = MakeSurface(
			WidgetTree,
			*FString::Printf(TEXT("ProgressionPath%d"), Index),
			AMSim::UITheme::ESurface::Card,
			FMargin(bCompactLayout ? 7.0f : 10.0f),
			14.0f);
		UTextBlock* Text = MakeText(
			WidgetTree,
			*FString::Printf(TEXT("ProgressionPathText%d"), Index),
			TEXT("PATH\nCAPABILITY BANDS"),
			SmallSize,
			AMSim::UITheme::White(),
			true);
		Text->SetJustification(ETextJustify::Center);
		UVerticalBox* CardColumn =
			WidgetTree->ConstructWidget<UVerticalBox>(
				UVerticalBox::StaticClass(),
				*FString::Printf(
					TEXT("ProgressionPathColumn%d"),
					Index));
		if (PathThumbnails.IsValidIndex(Index) && PathThumbnails[Index])
		{
			USizeBox* ThumbnailSize =
				WidgetTree->ConstructWidget<USizeBox>(
					USizeBox::StaticClass(),
					*FString::Printf(
						TEXT("ProgressionPathThumbnailSize%d"),
						Index));
			const float ThumbnailExtent =
				bCompactLayout ? 70.0f : 92.0f;
			ThumbnailSize->SetWidthOverride(ThumbnailExtent);
			ThumbnailSize->SetHeightOverride(ThumbnailExtent);
			UImage* Thumbnail = WidgetTree->ConstructWidget<UImage>(
				UImage::StaticClass(),
				*FString::Printf(
					TEXT("ProgressionPathThumbnail%d"),
					Index));
			Thumbnail->SetBrushFromTexture(PathThumbnails[Index], true);
			ThumbnailSize->SetContent(Thumbnail);
			UVerticalBoxSlot* ThumbnailSlot =
				CardColumn->AddChildToVerticalBox(ThumbnailSize);
			ThumbnailSlot->SetHorizontalAlignment(HAlign_Center);
			ThumbnailSlot->SetPadding(
				FMargin(0.0f, 0.0f, 0.0f, 5.0f));
		}
		UVerticalBoxSlot* TextSlot =
			CardColumn->AddChildToVerticalBox(Text);
		TextSlot->SetHorizontalAlignment(HAlign_Fill);
		TextSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		Card->SetContent(CardColumn);
		UButton* Button = WidgetTree->ConstructWidget<UButton>(
			UButton::StaticClass(),
			*FString::Printf(TEXT("ProgressionPathButton%d"), Index));
		Button->SetStyle(AMSim::UITheme::ButtonStyle(
			AMSim::UITheme::EButton::Quiet,
			14.0f));
		switch (Index)
		{
		case 0:
			Button->OnClicked.AddDynamic(
				this,
				&UAMSimProgressionView::SelectPath0);
			break;
		case 1:
			Button->OnClicked.AddDynamic(
				this,
				&UAMSimProgressionView::SelectPath1);
			break;
		case 2:
			Button->OnClicked.AddDynamic(
				this,
				&UAMSimProgressionView::SelectPath2);
			break;
		case 3:
			Button->OnClicked.AddDynamic(
				this,
				&UAMSimProgressionView::SelectPath3);
			break;
		case 4:
			Button->OnClicked.AddDynamic(
				this,
				&UAMSimProgressionView::SelectPath4);
			break;
		case 5:
			Button->OnClicked.AddDynamic(
				this,
				&UAMSimProgressionView::SelectPath5);
			break;
		default:
			break;
		}
		Button->SetContent(Card);
		if (UButtonSlot* ButtonSlot = Cast<UButtonSlot>(Card->Slot))
		{
			ButtonSlot->SetHorizontalAlignment(HAlign_Fill);
			ButtonSlot->SetVerticalAlignment(VAlign_Fill);
		}
		PathCards.Add(Card);
		PathTexts.Add(Text);
		AddAnchored(MapCanvas, Button, CardAnchors[Index], 2);
	}
	UBorder* AirportNode = MakeSurface(
		WidgetTree,
		TEXT("ProgressionAirportNode"),
		AMSim::UITheme::ESurface::Positive,
		FMargin(8.0f),
		14.0f,
		2.0f);
	UTextBlock* AirportText = MakeText(
		WidgetTree,
		TEXT("ProgressionAirportText"),
		TEXT("YOUR AIRPORT\nRIVERBEND FIELD"),
		BodySize,
		AMSim::UITheme::White(),
		true);
	AirportText->SetJustification(ETextJustify::Center);
	AirportNode->SetContent(AirportText);
	AddAnchored(
		MapCanvas,
		AirportNode,
		FAnchors(0.34f, 0.415f, 0.66f, 0.585f),
		4);
	AddAnchored(
		Canvas,
		Map,
		FAnchors(0.20f, 0.12f, 0.79f, 0.89f));

	UBorder* Inspector = MakeSurface(
		WidgetTree,
		TEXT("ProgressionInspector"),
		AMSim::UITheme::ESurface::RaisedCard,
		FMargin(bCompactLayout ? 10.0f : 15.0f));
	UVerticalBox* InspectorColumn =
		WidgetTree->ConstructWidget<UVerticalBox>(
			UVerticalBox::StaticClass(),
			TEXT("ProgressionInspectorColumn"));
	Inspector->SetContent(InspectorColumn);
	InspectorTitle = MakeText(
		WidgetTree,
		TEXT("ProgressionInspectorTitle"),
		TEXT("PASSENGER"),
		TitleSize,
		AMSim::UITheme::Cyan(),
		true);
	InspectorText = MakeText(
		WidgetTree,
		TEXT("ProgressionInspectorText"),
		TEXT("CURRENT EVIDENCE"),
		SmallSize,
		AMSim::UITheme::White());
	AddVertical(InspectorColumn, InspectorTitle, 10.0f);
	AddVertical(InspectorColumn, InspectorText, 0.0f, true);
	AddAnchored(
		Canvas,
		Inspector,
		FAnchors(0.80f, 0.12f, 0.988f, 0.89f));

	UBorder* Combined = MakeSurface(
		WidgetTree,
		TEXT("ProgressionCombined"),
		AMSim::UITheme::ESurface::Positive,
		FMargin(10.0f, 7.0f),
		13.0f);
	CombinedText = MakeText(
		WidgetTree,
		TEXT("ProgressionCombinedText"),
		TEXT("PATHS CAN BE COMBINED"),
		SmallSize,
		AMSim::UITheme::White(),
		true);
	CombinedText->SetJustification(ETextJustify::Center);
	Combined->SetContent(CombinedText);
	AddAnchored(
		Canvas,
		Combined,
		FAnchors(0.20f, 0.91f, 0.80f, 0.985f));

	RefreshFromSimulation();
	return Super::RebuildWidget();
}

void UAMSimProgressionView::RefreshFromSimulation()
{
	UAMSimAirportSimulationSubsystem* Subsystem =
		GetWorld()
			? GetWorld()->GetSubsystem<UAMSimAirportSimulationSubsystem>()
			: nullptr;
	if (!Subsystem)
	{
		return;
	}
	const AMSim::FProgressionViewState Next =
		AMSim::MakeProgressionViewState(
			Subsystem->GetPhase1Query(),
			Subsystem->GetPhase2Query(),
			Subsystem->GetSimulation().GetPhase2State(),
			Subsystem->GetPhase3Query(),
			Subsystem->GetPhase4Query());
	if (Next.Revision == ViewState.Revision)
	{
		return;
	}
	ViewState = Next;
	FundsText->SetText(FText::FromString(ViewState.Funds));
	for (int32 Index = 0; Index < ObjectiveTexts.Num(); ++Index)
	{
		if (ViewState.Objectives.IsValidIndex(Index))
		{
			ObjectiveTexts[Index]->SetText(
				FText::FromString(ViewState.Objectives[Index]));
			ObjectiveProgress[Index]->SetPercent(
				ViewState.ObjectiveProgress[Index]);
		}
	}
	for (int32 Index = 0; Index < PathTexts.Num(); ++Index)
	{
		if (!ViewState.Paths.IsValidIndex(Index))
		{
			continue;
		}
		const AMSim::FProgressionPathViewState& Path =
			ViewState.Paths[Index];
		PathTexts[Index]->SetText(FText::FromString(FString::Printf(
			TEXT("%s\n%s"),
			*Path.Name,
			*Path.Bands)));
		if (SelectedPathIndex == INDEX_NONE && Path.bSelected)
		{
			SelectedPathIndex = Index;
		}
	}
	ApplySelection();
	CombinedText->SetText(
		FText::FromString(ViewState.CombinedPathExplanation));
}

void UAMSimProgressionView::SelectPath0()
{
	SelectPath(0);
}

void UAMSimProgressionView::SelectPath1()
{
	SelectPath(1);
}

void UAMSimProgressionView::SelectPath2()
{
	SelectPath(2);
}

void UAMSimProgressionView::SelectPath3()
{
	SelectPath(3);
}

void UAMSimProgressionView::SelectPath4()
{
	SelectPath(4);
}

void UAMSimProgressionView::SelectPath5()
{
	SelectPath(5);
}

void UAMSimProgressionView::SelectPath(const int32 Index)
{
	if (!ViewState.Paths.IsValidIndex(Index))
	{
		return;
	}
	SelectedPathIndex = Index;
	ApplySelection();
}

void UAMSimProgressionView::ApplySelection()
{
	if (!ViewState.Paths.IsValidIndex(SelectedPathIndex))
	{
		return;
	}
	for (int32 Index = 0; Index < PathCards.Num(); ++Index)
	{
		const AMSim::FProgressionPathViewState& Path =
			ViewState.Paths[Index];
		const bool bSelected = Index == SelectedPathIndex;
		AMSim::UITheme::StyleSurface(
			PathCards[Index],
			Path.bFutureLocked
				? bSelected
					? AMSim::UITheme::ESurface::Warning
					: AMSim::UITheme::ESurface::Chip
				: bSelected
					? AMSim::UITheme::ESurface::Positive
					: AMSim::UITheme::ESurface::Card,
			FMargin(bCompactLayout ? 7.0f : 10.0f),
			14.0f,
			bSelected ? 2.4f : 1.4f);
		PathTexts[Index]->SetColorAndOpacity(FSlateColor(
			Path.bFutureLocked && !bSelected
				? AMSim::UITheme::Muted()
				: bSelected
					? AMSim::UITheme::Cyan()
					: AMSim::UITheme::White()));
	}
	const AMSim::FProgressionPathViewState& Selected =
		ViewState.Paths[SelectedPathIndex];
	InspectorTitle->SetText(FText::FromString(Selected.Name));
	if (Selected.bFutureLocked)
	{
		InspectorText->SetText(FText::FromString(FString::Printf(
			TEXT(
				"FUTURE DESTINATION\n"
				"%s\n\n"
				"REQUIREMENTS\n"
				"Owned by a later approved gameplay phase.\n\n"
				"CURRENT STATUS\n"
				"No playable cargo systems are implemented.\n\n"
				"NEXT CAPABILITY\n"
				"Remains visibly locked until its simulation exists.\n\n"
				"REWARD / OUTCOME\n"
				"No gameplay reward is advertised early."),
			*Selected.Summary)));
	}
	else if (Selected.bSelected)
	{
		InspectorText->SetText(FText::FromString(ViewState.Inspector));
	}
	else
	{
		InspectorText->SetText(FText::FromString(FString::Printf(
			TEXT(
				"PATH SUMMARY\n"
				"%s\n\n"
				"REQUIREMENTS\n"
				"Use implemented facilities and operating systems "
				"for this path.\n\n"
				"CAPABILITY BANDS\n"
				"%s\n\n"
				"CURRENT EVIDENCE\n"
				"%s\n\n"
				"SPATIAL FOOTPRINT\n"
				"%s\n\n"
				"BUSINESS EFFECT\n"
				"Uses implemented facilities and operating systems without "
				"excluding another path.\n\n"
				"NEXT CAPABILITY\n"
				"Advanced and Major remain future locked.\n\n"
				"REWARD / OUTCOME\n"
				"This path combines with other unlocked operations."),
			*Selected.Summary,
			*Selected.Bands,
			*Selected.Status,
			*Selected.Summary)));
	}
}
