#include "AMSimConstructionProposalView.h"
#include "AMSimAirportSimulationSubsystem.h"
#include "AMSimPhase1Fixture.h"
#include "AMSimUITheme.h"
#include "AMSimUISoundSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "InputCoreTypes.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
namespace AMSimConstructionProposalPrivate
{
	constexpr float ParcelLeft = 0.205f;
	constexpr float ParcelTop = 0.135f;
	constexpr float ParcelRight = 0.785f;
	constexpr float ParcelBottom = 0.825f;
	constexpr double ParcelCentimeters = 100000.0;
	constexpr int64 HandleHitCentimeters = 3500;

	enum EHandleIndex : int32
	{
		RunwayStartHandle = 0,
		RunwayEndHandle = 1,
		TaxiHandleStart = 2,
		TaxiHandleCount = UAMSimConstructionProposalView::MaximumTaxiwaySegments * 2,
		GateAHandle = TaxiHandleStart + TaxiHandleCount,
		GateBHandle,
		RoadStartHandle,
		RoadEndHandle,
		HandleCount
	};
	UTextBlock* Text(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const TCHAR* Value,
		const int32 Size,
		const FLinearColor& Color,
		const bool bBold = false)
	{
		UTextBlock* Result = Tree->ConstructWidget<UTextBlock>(
			UTextBlock::StaticClass(),
			FName(Name));
		Result->SetText(FText::FromString(Value));
		Result->SetAutoWrapText(true);
		AMSim::UITheme::StyleText(Result, Size, Color, bBold, true);
		return Result;
	}
	UBorder* Surface(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const AMSim::UITheme::ESurface Kind,
		const FMargin Padding = FMargin(12.0f))
	{
		UBorder* Result = Tree->ConstructWidget<UBorder>(
			UBorder::StaticClass(),
			FName(Name));
		AMSim::UITheme::StyleSurface(Result, Kind, Padding, 14.0f, 1.4f);
		return Result;
	}
	UBorder* ColorSurface(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const FLinearColor& Fill,
		const FLinearColor& Stroke,
		const float StrokeWidth = 2.0f,
		const float Radius = 7.0f)
	{
		UBorder* Result = Tree->ConstructWidget<UBorder>(
			UBorder::StaticClass(),
			FName(Name));
		Result->SetBrush(AMSim::UITheme::RoundedBrush(
			Fill,
			Radius,
			Stroke,
			StrokeWidth));
		Result->SetPadding(FMargin(0.0f));
		return Result;
	}
	void AddVertical(
		UVerticalBox* Box,
		UWidget* Widget,
		const float Padding = 4.0f)
	{
		UVerticalBoxSlot* Slot = Box->AddChildToVerticalBox(Widget);
		Slot->SetPadding(FMargin(0.0f, Padding));
		Slot->SetHorizontalAlignment(HAlign_Fill);
	}
	void SetFill(UHorizontalBoxSlot* Slot, const float Value)
	{
		FSlateChildSize Size(ESlateSizeRule::Fill);
		Size.Value = Value;
		Slot->SetSize(Size);
	}
	UCanvasPanelSlot* Place(
		UCanvasPanel* Canvas,
		UWidget* Widget,
		const FAnchors& Anchors,
		const FMargin& Offsets = FMargin(),
		const int32 ZOrder = 0)
	{
		UCanvasPanelSlot* Slot = Canvas->AddChildToCanvas(Widget);
		Slot->SetAnchors(Anchors);
		Slot->SetOffsets(Offsets);
		Slot->SetZOrder(ZOrder);
		return Slot;
	}
	UButton* Button(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const TCHAR* Label,
		const AMSim::UITheme::EButton Kind,
		const int32 TextSize = 12)
	{
		UButton* Result = Tree->ConstructWidget<UButton>(
			UButton::StaticClass(),
			FName(Name));
		Result->SetStyle(AMSim::UITheme::ButtonStyle(Kind));
		UTextBlock* LabelText = Text(
			Tree,
			*(FString(Name) + TEXT("Label")),
			Label,
			TextSize,
			AMSim::UITheme::White(),
			true);
		LabelText->SetJustification(ETextJustify::Center);
		Result->SetContent(LabelText);
		return Result;
	}
	UCanvasPanel* PreviewRegion(
		UWidgetTree* Tree,
		const TCHAR* Name,
		const FLinearColor& Fill,
		const FLinearColor& Stroke,
		const TCHAR* Label,
		const int32 StripeCount,
		TObjectPtr<UBorder>& OutSurface,
		UTextBlock*& OutLabel)
	{
		UCanvasPanel* Region = Tree->ConstructWidget<UCanvasPanel>(
			UCanvasPanel::StaticClass(),
			FName(Name));
		Region->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		OutSurface = ColorSurface(
			Tree,
			*(FString(Name) + TEXT("Base")),
			Fill,
			Stroke,
			2.0f,
			8.0f);
		Place(Region, OutSurface, FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
		for (int32 Index = 0; Index < StripeCount; ++Index)
		{
			const float Left = 0.04f + Index * (0.90f / FMath::Max(StripeCount, 1));
			Place(
				Region,
				ColorSurface(
					Tree,
					*FString::Printf(TEXT("%sStripe%d"), Name, Index),
					FLinearColor(1.0f, 1.0f, 1.0f, 0.14f),
					FLinearColor::Transparent,
					0.0f,
					0.0f),
				FAnchors(Left, 0.08f, Left + 0.026f, 0.92f),
				FMargin(),
				1);
		}
		OutLabel = Text(
			Tree,
			*(FString(Name) + TEXT("Label")),
			Label,
			10,
			AMSim::UITheme::White(),
			true);
		OutLabel->SetJustification(ETextJustify::Center);
		Place(
			Region,
			OutLabel,
			FAnchors(0.0f, 0.0f, 1.0f, 1.0f),
			FMargin(),
			2);
		return Region;
	}
	float MapX(const int64 X)
	{
		return ParcelLeft +
			static_cast<float>(static_cast<double>(X) / ParcelCentimeters) *
			(ParcelRight - ParcelLeft);
	}
	float MapY(const int64 Y)
	{
		return ParcelTop +
			static_cast<float>(static_cast<double>(Y) / ParcelCentimeters) *
			(ParcelBottom - ParcelTop);
	}
	int64 DistanceSquared(
		const AMSim::FPhase1Point Left,
		const AMSim::FPhase1Point Right)
	{
		const int64 DeltaX = Left.X - Right.X;
		const int64 DeltaY = Left.Y - Right.Y;
		return DeltaX * DeltaX + DeltaY * DeltaY;
	}

	bool ProposalEquals(
		const AMSim::FStarterPlanProposal& Left,
		const AMSim::FStarterPlanProposal& Right)
	{
		return Left.RunwayStart == Right.RunwayStart &&
			Left.RunwayEnd == Right.RunwayEnd &&
			Left.RunwayWidthCentimeters == Right.RunwayWidthCentimeters &&
			Left.TaxiStart == Right.TaxiStart &&
			Left.TaxiEnd == Right.TaxiEnd &&
			Left.TaxiwaySegments == Right.TaxiwaySegments &&
			Left.StandCenter == Right.StandCenter &&
			Left.AccessStart == Right.AccessStart &&
			Left.AccessEnd == Right.AccessEnd &&
			Left.OperationsHutCenter == Right.OperationsHutCenter;
	}

	void SetSegmentGeometry(
		UCanvasPanelSlot* Slot,
		UWidget* Widget,
		const AMSim::FPhase1Point Start,
		const AMSim::FPhase1Point End,
		const float HalfThickness)
	{
		if (!Slot || !Widget)
		{
			return;
		}
		const float StartX = MapX(Start.X);
		const float StartY = MapY(Start.Y);
		const float EndX = MapX(End.X);
		const float EndY = MapY(End.Y);
		const float DeltaX = EndX - StartX;
		const float DeltaY = EndY - StartY;
		constexpr float CanvasHeightToWidth = 1080.0f / 1920.0f;
		const float ScreenDeltaY = DeltaY * CanvasHeightToWidth;
		const float Length = FMath::Max(
			FMath::Sqrt(DeltaX * DeltaX + ScreenDeltaY * ScreenDeltaY),
			0.006f);
		const float CenterX = (StartX + EndX) * 0.5f;
		const float CenterY = (StartY + EndY) * 0.5f;
		Slot->SetAnchors(FAnchors(
			CenterX - Length * 0.5f,
			CenterY - HalfThickness,
			CenterX + Length * 0.5f,
			CenterY + HalfThickness));
		Slot->SetOffsets(FMargin());
		Widget->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
		Widget->SetRenderTransformAngle(
			FMath::RadiansToDegrees(FMath::Atan2(ScreenDeltaY, DeltaX)));
	}

	void SetBoxGeometry(
		UCanvasPanelSlot* Slot,
		const AMSim::FPhase1Point Center,
		const float HalfWidth,
		const float HalfHeight)
	{
		if (!Slot)
		{
			return;
		}
		Slot->SetAnchors(FAnchors(
			MapX(Center.X) - HalfWidth,
			MapY(Center.Y) - HalfHeight,
			MapX(Center.X) + HalfWidth,
			MapY(Center.Y) + HalfHeight));
		Slot->SetOffsets(FMargin());
	}

	void SetPointGeometry(
		UCanvasPanelSlot* Slot,
		const AMSim::FPhase1Point Point,
		const FVector2D Size = FVector2D(28.0f, 28.0f),
		const FVector2D PixelOffset = FVector2D::ZeroVector)
	{
		if (!Slot)
		{
			return;
		}
		Slot->SetAnchors(FAnchors(
			FMath::Clamp(MapX(Point.X), ParcelLeft, ParcelRight),
			FMath::Clamp(MapY(Point.Y), ParcelTop, ParcelBottom)));
		Slot->SetAlignment(FVector2D(0.5f, 0.5f));
		Slot->SetOffsets(FMargin(PixelOffset.X, PixelOffset.Y, Size.X, Size.Y));
	}

	void StyleProposalSurface(
		UBorder* SurfaceWidget,
		const bool bSelected,
		const FLinearColor& Identity)
	{
		if (!SurfaceWidget)
		{
			return;
		}
		const FLinearColor Fill(
			Identity.R,
			Identity.G,
			Identity.B,
			bSelected ? 0.58f : 0.34f);
		SurfaceWidget->SetBrush(AMSim::UITheme::RoundedBrush(
			Fill,
			8.0f,
			bSelected ? AMSim::UITheme::White() : AMSim::UITheme::Cyan(),
			bSelected ? 3.0f : 1.6f));
	}

	const TCHAR* ToolName(const UAMSimConstructionProposalView::EPlacementTool Tool)
	{
		switch (Tool)
		{
		case UAMSimConstructionProposalView::EPlacementTool::Runway:
			return TEXT("RUNWAY");
		case UAMSimConstructionProposalView::EPlacementTool::Taxiway:
			return TEXT("TAXIWAY");
		case UAMSimConstructionProposalView::EPlacementTool::RoadAccess:
			return TEXT("SERVICE ROAD");
		}
		return TEXT("BUILD TOOL");
	}
}

FString UAMSimConstructionProposalView::DescribeRunwayGeometry(
	const AMSim::FStarterPlanProposal& Proposal)
{
	const double DeltaX = static_cast<double>(
		Proposal.RunwayEnd.X - Proposal.RunwayStart.X);
	const double DeltaY = static_cast<double>(
		Proposal.RunwayEnd.Y - Proposal.RunwayStart.Y);
	const int64 LengthMeters = FMath::RoundToInt64(
		FMath::Sqrt(DeltaX * DeltaX + DeltaY * DeltaY) / 100.0);
	const AMSim::FRunwayDesignation Designation =
		AMSim::CalculateRunwayDesignation(
			Proposal.RunwayStart,
			Proposal.RunwayEnd);
	return FString::Printf(
		TEXT("RUNWAY %02d/%02d  ·  %lld M x %d M"),
		Designation.PrimaryNumber,
		Designation.ReciprocalNumber,
		LengthMeters,
		Proposal.RunwayWidthCentimeters / 100);
}

TSharedRef<SWidget> UAMSimConstructionProposalView::RebuildWidget()
{
	using namespace AMSimConstructionProposalPrivate;
	SetIsFocusable(true);
	const bool bConflictProof = FParse::Param(
		FCommandLine::Get(),
		TEXT("AMSimPhase45ConstructionProof"));
	CurrentProposal = bConflictProof
		? MakePresentationProposal(true)
		: MakeEmptyProposal();
	TaxiGeometryWidgets.Empty();
	TaxiGeometrySurfaces.Empty();
	TaxiGeometrySlots.Empty();
	HandleSurfaces.Empty();
	HandleTexts.Empty();
	HandleSlots.Empty();
	DiagnosticSlots.Empty();
	DiagnosticSurfaces.Empty();
	DiagnosticTexts.Empty();
	PlacementMask = bConflictProof ? AllPlacementParts : 0;
	CurrentValidation = bConflictProof
		? AMSim::ValidateStarterPlan(CurrentProposal)
		: AMSim::FPhase1Validation();
	WidgetTree = NewObject<UWidgetTree>(this, TEXT("ConstructionProposalTree"));
	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(
		UCanvasPanel::StaticClass(),
		TEXT("ConstructionProposalRoot"));
	WidgetTree->RootWidget = Root;

	Place(
		Root,
		ColorSurface(
			WidgetTree,
			TEXT("BuildModeLeftBackdrop"),
			AMSim::UITheme::Navy900(),
			FLinearColor::Transparent,
			0.0f,
			0.0f),
		FAnchors(0.0f, 0.0f, ParcelLeft, 1.0f));
	Place(
		Root,
		ColorSurface(
			WidgetTree,
			TEXT("BuildModeRightBackdrop"),
			AMSim::UITheme::Navy900(),
			FLinearColor::Transparent,
			0.0f,
			0.0f),
		FAnchors(ParcelRight, 0.0f, 1.0f, 1.0f));

	UBorder* ParcelTint = ColorSurface(
		WidgetTree,
		TEXT("OwnedParcelSurface"),
		FLinearColor(0.03f, 0.24f, 0.22f, 0.16f),
		AMSim::UITheme::Cyan(),
		3.0f,
		10.0f);
	Place(
		Root,
		ParcelTint,
		FAnchors(ParcelLeft, ParcelTop, ParcelRight, ParcelBottom),
		FMargin(),
		1);
	for (int32 Index = 1; Index < 10; ++Index)
	{
		const float Alpha = static_cast<float>(Index) / 10.0f;
		const float X = FMath::Lerp(ParcelLeft, ParcelRight, Alpha);
		const float Y = FMath::Lerp(ParcelTop, ParcelBottom, Alpha);
		const FLinearColor MajorGrid = Index == 5
			? FLinearColor(0.32f, 0.83f, 0.91f, 0.34f)
			: FLinearColor(0.72f, 0.90f, 0.90f, 0.19f);
		Place(
			Root,
			ColorSurface(
				WidgetTree,
				*FString::Printf(TEXT("ParcelGridV%d"), Index),
				MajorGrid,
				FLinearColor::Transparent,
				0.0f,
				0.0f),
			FAnchors(X - 0.0006f, ParcelTop, X + 0.0006f, ParcelBottom),
			FMargin(),
			2);
		Place(
			Root,
			ColorSurface(
				WidgetTree,
				*FString::Printf(TEXT("ParcelGridH%d"), Index),
				MajorGrid,
				FLinearColor::Transparent,
				0.0f,
				0.0f),
			FAnchors(ParcelLeft, Y - 0.0008f, ParcelRight, Y + 0.0008f),
			FMargin(),
			2);
	}
	UBorder* ParcelLabel = Surface(
		WidgetTree,
		TEXT("OwnedParcelLabel"),
		AMSim::UITheme::ESurface::Chip,
		FMargin(10.0f, 5.0f));
	ParcelLabel->SetContent(Text(
		WidgetTree,
		TEXT("OwnedParcelLabelText"),
		TEXT("OWNED PARCEL  ·  1,000 x 1,000 M  ·  10 M SNAP"),
		10,
		AMSim::UITheme::Cyan(),
		true));
	Place(
		Root,
		ParcelLabel,
		FAnchors(ParcelLeft + 0.006f, ParcelTop + 0.008f, 0.445f, ParcelTop + 0.047f),
		FMargin(),
		8);

	UBorder* Header = Surface(
		WidgetTree,
		TEXT("BuildModeHeader"),
		AMSim::UITheme::ESurface::Chrome,
		FMargin(14.0f, 8.0f));
	UHorizontalBox* HeaderRow = WidgetTree->ConstructWidget<UHorizontalBox>();
	Header->SetContent(HeaderRow);
	UTextBlock* HeaderTitle = Text(
		WidgetTree,
		TEXT("BuildModeTitle"),
		TEXT("BUILD MODE  ·  STARTER AIRFIELD"),
		14,
		AMSim::UITheme::White(),
		true);
	UHorizontalBoxSlot* HeaderTitleSlot =
		HeaderRow->AddChildToHorizontalBox(HeaderTitle);
	SetFill(HeaderTitleSlot, 0.66f);
	HeaderStateText = Text(
		WidgetTree,
		TEXT("BuildModeState"),
		TEXT("10 M GRID  ·  RUNWAY"),
		11,
		AMSim::UITheme::Cyan(),
		true);
	HeaderStateText->SetJustification(ETextJustify::Right);
	UHorizontalBoxSlot* HeaderStateSlot =
		HeaderRow->AddChildToHorizontalBox(HeaderStateText);
	SetFill(HeaderStateSlot, 0.34f);
	Place(Root, Header, FAnchors(0.25f, 0.012f, 0.75f, 0.083f), FMargin(), 10);

	UBorder* Palette = Surface(
		WidgetTree,
		TEXT("BuildToolPalette"),
		AMSim::UITheme::ESurface::Panel,
		FMargin(11.0f, 9.0f));
	UVerticalBox* PaletteColumn = WidgetTree->ConstructWidget<UVerticalBox>();
	Palette->SetContent(PaletteColumn);
	AddVertical(
		PaletteColumn,
		Text(
			WidgetTree,
			TEXT("BuildToolsTitle"),
			TEXT("BUILD TOOLS"),
			13,
			AMSim::UITheme::White(),
			true),
		0.0f);
	AddVertical(
		PaletteColumn,
		Text(
			WidgetTree,
			TEXT("BuildToolsHint"),
			TEXT("PLACE ON THE MAP  ·  DRAG HANDLES TO ADJUST"),
			9,
			AMSim::UITheme::Muted(),
			true),
		1.0f);
	AddVertical(
		PaletteColumn,
		Text(WidgetTree, TEXT("MovementTools"), TEXT("MOVEMENT"), 9, AMSim::UITheme::Amber(), true),
		4.0f);
	RunwayToolButton = Button(
		WidgetTree,
		TEXT("RunwayPlacementTool"),
		TEXT("RUNWAY"),
		AMSim::UITheme::EButton::Primary,
		11);
	RunwayToolButton->OnClicked.AddDynamic(
		this,
		&UAMSimConstructionProposalView::SelectRunwayTool);
	AddVertical(PaletteColumn, RunwayToolButton, 2.0f);
	TaxiwayToolButton = Button(
		WidgetTree,
		TEXT("TaxiwayPlacementTool"),
		TEXT("TAXIWAY"),
		AMSim::UITheme::EButton::Tool,
		11);
	TaxiwayToolButton->OnClicked.AddDynamic(
		this,
		&UAMSimConstructionProposalView::SelectTaxiwayTool);
	AddVertical(PaletteColumn, TaxiwayToolButton, 2.0f);
	AddVertical(
		PaletteColumn,
		Text(WidgetTree, TEXT("OptionalTools"), TEXT("OPTIONAL ACCESS"), 9, AMSim::UITheme::Amber(), true),
		4.0f);
	RoadAccessToolButton = Button(
		WidgetTree,
		TEXT("RoadAccessPlacementTool"),
		TEXT("SERVICE ROAD"),
		AMSim::UITheme::EButton::Tool,
		10);
	RoadAccessToolButton->OnClicked.AddDynamic(
		this,
		&UAMSimConstructionProposalView::SelectRoadAccessTool);
	AddVertical(PaletteColumn, RoadAccessToolButton, 2.0f);
	AddVertical(
		PaletteColumn,
		Text(WidgetTree, TEXT("PlanTools"), TEXT("PLAN"), 9, AMSim::UITheme::Amber(), true),
		4.0f);
	UndoButton = Button(
		WidgetTree,
		TEXT("UndoPlacement"),
		TEXT("UNDO LAST EDIT"),
		AMSim::UITheme::EButton::Quiet,
		10);
	UndoButton->OnClicked.AddDynamic(
		this,
		&UAMSimConstructionProposalView::UndoPlacement);
	AddVertical(PaletteColumn, UndoButton, 2.0f);
	UButton* ResetButton = Button(
		WidgetTree,
		TEXT("ResetPlacement"),
		TEXT("RESET STARTER PLAN"),
		AMSim::UITheme::EButton::Quiet,
		10);
	ResetButton->OnClicked.AddDynamic(
		this,
		&UAMSimConstructionProposalView::ResetPlacement);
	AddVertical(PaletteColumn, ResetButton, 2.0f);
	Place(Root, Palette, FAnchors(0.01f, 0.018f, 0.19f, 0.53f), FMargin(), 12);

	ValidationSurface = Surface(
		WidgetTree,
		TEXT("BuildValidation"),
		AMSim::UITheme::ESurface::Panel,
		FMargin(13.0f, 10.0f));
	UVerticalBox* ValidationColumn = WidgetTree->ConstructWidget<UVerticalBox>();
	ValidationSurface->SetContent(ValidationColumn);
	AddVertical(
		ValidationColumn,
		Text(WidgetTree, TEXT("BuildProposalEyebrow"), TEXT("STARTER PROPOSAL"), 10, AMSim::UITheme::Muted(), true),
		0.0f);
	ValidationLabelText = Text(
		WidgetTree,
		TEXT("BuildValidationTitle"),
		TEXT("READY TO BUILD"),
		14,
		AMSim::UITheme::Green(),
		true);
	AddVertical(ValidationColumn, ValidationLabelText, 1.0f);
	ValidationBodyText = Text(
		WidgetTree,
		TEXT("BuildValidationBody"),
		TEXT("Runway and taxi network reach a terminal gate."),
		11,
		AMSim::UITheme::White(),
		true);
	AddVertical(ValidationColumn, ValidationBodyText, 3.0f);
	GeometryText = Text(
		WidgetTree,
		TEXT("BuildGeometry"),
		TEXT("RUNWAY 09/27  ·  600 M x 20 M"),
		10,
		AMSim::UITheme::Cyan(),
		true);
	AddVertical(ValidationColumn, GeometryText, 6.0f);
	ConnectionText = Text(
		WidgetTree,
		TEXT("BuildConnections"),
		TEXT("CONNECTED  -  RUNWAY > TAXI NETWORK > GATE"),
		10,
		AMSim::UITheme::White(),
		true);
	AddVertical(ValidationColumn, ConnectionText, 4.0f);
	AddVertical(
		ValidationColumn,
		Text(
			WidgetTree,
			TEXT("BuildResources"),
			TEXT("3,400 CR  -  GRASS SURFACE  -  1 CREW\nOPTIONAL SERVICE ROADS SPEED CONSTRUCTION BY 20%"),
			10,
			AMSim::UITheme::Amber(),
			true),
		6.0f);
	Place(
		Root,
		ValidationSurface,
		FAnchors(0.80f, 0.018f, 0.99f, 0.33f),
		FMargin(),
		12);

	UBorder* Instruction = Surface(
		WidgetTree,
		TEXT("BuildInstruction"),
		AMSim::UITheme::ESurface::Chip,
		FMargin(10.0f, 6.0f));
	PlacementText = Text(
		WidgetTree,
		TEXT("BuildInstructionText"),
		TEXT("RUNWAY  ·  CLICK START, THEN END  ·  DRAG TO MOVE OR ADJUST HANDLES"),
		10,
		AMSim::UITheme::White(),
		true);
	PlacementText->SetJustification(ETextJustify::Center);
	Instruction->SetContent(PlacementText);
	Place(Root, Instruction, FAnchors(0.27f, 0.088f, 0.72f, 0.132f), FMargin(), 11);

	UTextBlock* RunwayLabel = nullptr;
	UCanvasPanel* RunwayPreview = PreviewRegion(
		WidgetTree,
		TEXT("RunwayPreview"),
		FLinearColor(0.08f, 0.45f, 0.62f, 0.46f),
		AMSim::UITheme::Cyan(),
		TEXT("RUNWAY 09/27  ·  600 M"),
		12,
		RunwayGeometrySurface,
		RunwayLabel);
	RunwayGeometryLabel = RunwayLabel;
	if (UCanvasPanelSlot* LabelSlot = Cast<UCanvasPanelSlot>(RunwayGeometryLabel->Slot))
	{
		LabelSlot->SetAnchors(FAnchors(0.52f, 0.0f, 0.90f, 1.0f));
		LabelSlot->SetOffsets(FMargin());
	}
	RunwayGeometryWidget = RunwayPreview;
	RunwayGeometrySlot = Place(Root, RunwayPreview, FAnchors(0.25f, 0.38f, 0.65f, 0.46f), FMargin(), 5);

	for (int32 Index = 0; Index < MaximumTaxiwaySegments; ++Index)
	{
		UTextBlock* TaxiLabel = nullptr;
		TObjectPtr<UBorder> TaxiSurface;
		UCanvasPanel* TaxiPreview = PreviewRegion(
			WidgetTree,
			*FString::Printf(TEXT("TaxiwayPreview%d"), Index),
			FLinearColor(0.08f, 0.45f, 0.62f, 0.42f),
			AMSim::UITheme::Cyan(),
			TEXT(""),
			4,
			TaxiSurface,
			TaxiLabel);
		TaxiGeometryWidgets.Add(TaxiPreview);
		TaxiGeometrySurfaces.Add(TaxiSurface);
		TaxiGeometrySlots.Add(Place(
			Root,
			TaxiPreview,
			FAnchors(0.44f, 0.40f, 0.46f, 0.60f),
			FMargin(),
			6));
	}

	UTextBlock* TerminalLabel = nullptr;
	UCanvasPanel* TerminalPreview = PreviewRegion(
		WidgetTree,
		TEXT("StarterTerminalPreview"),
		FLinearColor(0.49f, 0.36f, 0.13f, 0.62f),
		AMSim::UITheme::Amber(),
		TEXT("BASIC TERMINAL"),
		5,
		TerminalGeometrySurface,
		TerminalLabel);
	TerminalGeometryWidget = TerminalPreview;
	TerminalGeometrySlot = Place(
		Root,
		TerminalPreview,
		FAnchors(0.44f, 0.69f, 0.55f, 0.78f),
		FMargin(),
		7);

	UTextBlock* GateALabel = nullptr;
	UCanvasPanel* GateAPreview = PreviewRegion(
		WidgetTree,
		TEXT("StarterGateAPreview"),
		FLinearColor(0.08f, 0.45f, 0.62f, 0.34f),
		AMSim::UITheme::Cyan(),
		TEXT(""),
		3,
		GateAGeometrySurface,
		GateALabel);
	GateAGeometryWidget = GateAPreview;
	GateAGeometrySlot = Place(
		Root,
		GateAPreview,
		FAnchors(0.43f, 0.63f, 0.48f, 0.69f),
		FMargin(),
		7);

	UTextBlock* GateBLabel = nullptr;
	UCanvasPanel* GateBPreview = PreviewRegion(
		WidgetTree,
		TEXT("StarterGateBPreview"),
		FLinearColor(0.08f, 0.45f, 0.62f, 0.34f),
		AMSim::UITheme::Cyan(),
		TEXT(""),
		3,
		GateBGeometrySurface,
		GateBLabel);
	GateBGeometryWidget = GateBPreview;
	GateBGeometrySlot = Place(
		Root,
		GateBPreview,
		FAnchors(0.49f, 0.63f, 0.54f, 0.69f),
		FMargin(),
		7);

	UTextBlock* AccessLabel = nullptr;
	UCanvasPanel* AccessPreview = PreviewRegion(
		WidgetTree,
		TEXT("AccessPreview"),
		FLinearColor(0.12f, 0.35f, 0.44f, 0.50f),
		AMSim::UITheme::Cyan(),
		TEXT(""),
		8,
		AccessGeometrySurface,
		AccessLabel);
	AccessGeometryWidget = AccessPreview;
	AccessGeometrySlot = Place(Root, AccessPreview, FAnchors(0.44f, 0.60f, 0.46f, 0.82f), FMargin(), 6);

	HoverGhostSurface = ColorSurface(
		WidgetTree,
		TEXT("PlacementCenterline"),
		FLinearColor(0.09f, 0.61f, 0.74f, 0.38f),
		AMSim::UITheme::Cyan(),
		2.0f,
		3.0f);
	HoverGhostWidget = HoverGhostSurface;
	HoverGhostSlot = Place(
		Root,
		HoverGhostSurface,
		FAnchors(0.4f, 0.4f, 0.5f, 0.405f),
		FMargin(),
		8);
	HoverGhostSurface->SetVisibility(ESlateVisibility::Collapsed);

	PointerEndpointSurface = ColorSurface(
		WidgetTree,
		TEXT("PlacementPointerEndpoint"),
		AMSim::UITheme::Navy900(),
		AMSim::UITheme::Cyan(),
		3.0f,
		28.0f);
	PointerEndpointText = Text(
		WidgetTree,
		TEXT("PlacementPointerEndpointLabel"),
		TEXT(""),
		9,
		AMSim::UITheme::White(),
		true);
	PointerEndpointText->SetJustification(ETextJustify::Center);
	PointerEndpointSurface->SetContent(PointerEndpointText);
	PointerEndpointSlot = Place(
		Root,
		PointerEndpointSurface,
		FAnchors(0.5f),
		FMargin(0.0f, 0.0f, 26.0f, 26.0f),
		10);
	PointerEndpointSurface->SetVisibility(ESlateVisibility::Collapsed);

	CrossingSurface = Surface(
		WidgetTree,
		TEXT("RunwayCrossingMarker"),
		AMSim::UITheme::ESurface::Chip,
		FMargin(8.0f, 4.0f));
	CrossingText = Text(
		WidgetTree,
		TEXT("RunwayCrossingLabel"),
		TEXT("RUNWAY CROSSING"),
		8,
		AMSim::UITheme::White(),
		true);
	CrossingText->SetJustification(ETextJustify::Center);
	CrossingSurface->SetContent(CrossingText);
	CrossingSlot = Place(
		Root,
		CrossingSurface,
		FAnchors(0.5f),
		FMargin(0.0f, 0.0f, 132.0f, 30.0f),
		11);
	CrossingSurface->SetVisibility(ESlateVisibility::Collapsed);

	for (int32 Index = 0; Index < HandleCount; ++Index)
	{
		UBorder* Handle = ColorSurface(
			WidgetTree,
			*FString::Printf(TEXT("BuildHandle%d"), Index),
			AMSim::UITheme::Navy900(),
			AMSim::UITheme::Cyan(),
			2.0f,
			14.0f);
		UTextBlock* HandleLabel = Text(
			WidgetTree,
			*FString::Printf(TEXT("BuildHandleLabel%d"), Index),
			Index == GateAHandle ? TEXT("A") :
				Index == GateBHandle ? TEXT("B") : TEXT(""),
			8,
			AMSim::UITheme::White(),
			true);
		HandleLabel->SetJustification(ETextJustify::Center);
		Handle->SetContent(HandleLabel);
		HandleSurfaces.Add(Handle);
		HandleTexts.Add(HandleLabel);
		HandleSlots.Add(Place(
			Root,
			Handle,
			FAnchors(0.5f),
			FMargin(0.0f, 0.0f,
				Index == GateAHandle || Index == GateBHandle ? 34.0f : 26.0f,
				Index == GateAHandle || Index == GateBHandle ? 34.0f : 26.0f),
			9));
	}

	for (int32 Index = 0; Index < 4; ++Index)
	{
		UBorder* Diagnostic = Surface(
			WidgetTree,
			*FString::Printf(TEXT("BuildDiagnostic%d"), Index),
			AMSim::UITheme::ESurface::Danger,
			FMargin(7.0f, 4.0f));
		UTextBlock* DiagnosticText = Text(
			WidgetTree,
			*FString::Printf(TEXT("BuildDiagnosticText%d"), Index),
			TEXT("CHECK PLACEMENT"),
			9,
			AMSim::UITheme::White(),
			true);
		DiagnosticText->SetJustification(ETextJustify::Center);
		Diagnostic->SetContent(DiagnosticText);
		UCanvasPanelSlot* DiagnosticSlot = Place(
			Root,
			Diagnostic,
			FAnchors(0.5f),
			FMargin(0.0f, 0.0f, 150.0f, 32.0f),
			13);
		Diagnostic->SetVisibility(ESlateVisibility::Collapsed);
		DiagnosticSlots.Add(DiagnosticSlot);
		DiagnosticSurfaces.Add(Diagnostic);
		DiagnosticTexts.Add(DiagnosticText);
	}

	UBorder* ActionsSurface = Surface(
		WidgetTree,
		TEXT("BuildActions"),
		AMSim::UITheme::ESurface::Chrome,
		FMargin(9.0f, 7.0f));
	UHorizontalBox* Actions = WidgetTree->ConstructWidget<UHorizontalBox>();
	ActionsSurface->SetContent(Actions);
	UButton* CancelButton = Button(
		WidgetTree,
		TEXT("CancelProposal"),
		TEXT("CANCEL"),
		AMSim::UITheme::EButton::Quiet,
		12);
	CancelButton->OnClicked.AddDynamic(
		this,
		&UAMSimConstructionProposalView::CancelProposal);
	UHorizontalBoxSlot* CancelSlot =
		Actions->AddChildToHorizontalBox(CancelButton);
	SetFill(CancelSlot, 0.32f);
	CancelSlot->SetPadding(FMargin(0.0f, 0.0f, 6.0f, 0.0f));

	ConfirmButton = WidgetTree->ConstructWidget<UButton>(
		UButton::StaticClass(),
		TEXT("ConfirmProposal"));
	ConfirmButton->SetStyle(AMSim::UITheme::ButtonStyle(
		AMSim::UITheme::EButton::Positive));
	ConfirmLabelText = Text(
		WidgetTree,
		TEXT("ConfirmProposalLabel"),
		TEXT("BUILD AIRFIELD  ·  3.4K CR"),
		13,
		AMSim::UITheme::White(),
		true);
	ConfirmLabelText->SetAutoWrapText(false);
	ConfirmLabelText->SetJustification(ETextJustify::Center);
	ConfirmButton->SetContent(ConfirmLabelText);
	ConfirmButton->OnClicked.AddDynamic(
		this,
		&UAMSimConstructionProposalView::ConfirmProposal);
	UHorizontalBoxSlot* ConfirmSlot =
		Actions->AddChildToHorizontalBox(ConfirmButton);
	SetFill(ConfirmSlot, 0.68f);
	Place(
		Root,
		ActionsSurface,
		FAnchors(0.30f, 0.875f, 0.70f, 0.985f),
		FMargin(),
		14);

	RefreshProposalPresentation();
	CloseProposal();
	return Root->TakeWidget();
}

FReply UAMSimConstructionProposalView::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}
	const FVector2D LocalPosition = InGeometry.AbsoluteToLocal(
		InMouseEvent.GetScreenSpacePosition());
	if (!IsWorldPlacementPosition(LocalPosition, InGeometry.GetLocalSize()))
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}
	DragStartMapPoint = MapLocalPositionToParcel(
		LocalPosition,
		InGeometry.GetLocalSize());
	DragStartProposal = CurrentProposal;
	DragStartPlacementMask = PlacementMask;
	DraggedTaxiwaySegmentIndex = INDEX_NONE;
	DraggedEndpointIndex = ResolveSelectedEndpoint(DragStartMapPoint);
	bPlacementDragging = true;
	bPlacementMoved = false;
	bUndoCapturedForGesture = false;
	bPathStartedByPress = false;
	bHoveringWorld = false;
	const bool bPathTool =
		SelectedPlacementTool == EPlacementTool::Runway ||
		SelectedPlacementTool == EPlacementTool::Taxiway ||
		SelectedPlacementTool == EPlacementTool::RoadAccess;
	const bool bCanStartPath =
		SelectedPlacementTool == EPlacementTool::Taxiway
			? CurrentProposal.TaxiwaySegments.Num() < MaximumTaxiwaySegments
			: !IsToolPlaced(SelectedPlacementTool);
	if (bPathTool && PathPlacementStep == 0 &&
		bCanStartPath &&
		DraggedEndpointIndex == INDEX_NONE &&
		DraggedTaxiwaySegmentIndex == INDEX_NONE)
	{
		CaptureUndo(CurrentProposal);
		if (SelectedPlacementTool == EPlacementTool::Taxiway)
		{
			ActiveTaxiwaySegmentIndex = CurrentProposal.TaxiwaySegments.Add({
				DragStartMapPoint,
				DragStartMapPoint});
		}
		CurrentProposal = SetSelectedToolEndpoint(
			CurrentProposal,
			0,
			DragStartMapPoint);
		CurrentProposal = SetSelectedToolEndpoint(
			CurrentProposal,
			1,
			DragStartMapPoint);
		PathPlacementStep = 1;
		bPathStartedByPress = true;
		RefreshProposalPresentation();
	}
	SetKeyboardFocus();
	return FReply::Handled().CaptureMouse(GetCachedWidget().ToSharedRef());
}

FReply UAMSimConstructionProposalView::NativeOnMouseButtonUp(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (!bPlacementDragging ||
		InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
	{
		return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
	}
	const AMSim::FPhase1Point PointerPoint = MapLocalPositionToParcel(
		InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition()),
		InGeometry.GetLocalSize());
	bool bCommittedPlacement = false;
	if (bPathStartedByPress)
	{
		constexpr int64 MinimumGestureCentimeters = 1000;
		if (AMSimConstructionProposalPrivate::DistanceSquared(
			PointerPoint,
			DragStartMapPoint) >=
			MinimumGestureCentimeters * MinimumGestureCentimeters)
		{
			CurrentProposal = SetSelectedToolEndpoint(
				CurrentProposal,
				1,
				PointerPoint);
			SetToolPlaced(SelectedPlacementTool, true);
			SyncLegacyTaxiwayFields();
			PathPlacementStep = 0;
			ActiveTaxiwaySegmentIndex = INDEX_NONE;
			bCommittedPlacement = true;
		}
		RefreshProposalPresentation();
	}
	else if (!bPlacementMoved && DraggedEndpointIndex == INDEX_NONE &&
		DraggedTaxiwaySegmentIndex == INDEX_NONE)
	{
		ApplyPointerClick(PointerPoint);
	}
	if (bCommittedPlacement || (!bPathStartedByPress && bPlacementMoved))
	{
		AMSim::UIAudio::Play(this, EAMSimUISound::BuildPlace);
	}
	bPlacementDragging = false;
	bPlacementMoved = false;
	bUndoCapturedForGesture = false;
	bPathStartedByPress = false;
	DraggedEndpointIndex = INDEX_NONE;
	DraggedTaxiwaySegmentIndex = INDEX_NONE;
	bHoveringWorld = true;
	HoverMapPoint = PointerPoint;
	RefreshPointerGhost();
	return FReply::Handled().ReleaseMouseCapture();
}

FReply UAMSimConstructionProposalView::NativeOnMouseMove(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	const FVector2D LocalPosition = InGeometry.AbsoluteToLocal(
		InMouseEvent.GetScreenSpacePosition());
	const bool bInsideWorld = IsWorldPlacementPosition(
		LocalPosition,
		InGeometry.GetLocalSize());
	if (!bPlacementDragging)
	{
		bHoveringWorld = bInsideWorld;
		if (bInsideWorld)
		{
			HoverMapPoint = MapLocalPositionToParcel(
				LocalPosition,
				InGeometry.GetLocalSize());
		}
		RefreshPointerGhost();
		return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
	}
	const AMSim::FPhase1Point PointerPoint = MapLocalPositionToParcel(
		LocalPosition,
		InGeometry.GetLocalSize());
	HoverMapPoint = PointerPoint;
	if (PointerPoint != DragStartMapPoint)
	{
		bPlacementMoved = true;
		ApplyPointerDrag(PointerPoint, false);
	}
	return FReply::Handled();
}

void UAMSimConstructionProposalView::NativeOnMouseLeave(
	const FPointerEvent& InMouseEvent)
{
	bHoveringWorld = false;
	RefreshPointerGhost();
	Super::NativeOnMouseLeave(InMouseEvent);
}

FReply UAMSimConstructionProposalView::NativeOnKeyDown(
	const FGeometry& InGeometry,
	const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		CancelProposal();
		return FReply::Handled();
	}
	if (InKeyEvent.GetKey() == EKeys::Z && InKeyEvent.IsControlDown())
	{
		UndoPlacement();
		return FReply::Handled();
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UAMSimConstructionProposalView::ConfirmProposal()
{
	if (!AreAllRequiredPlacementsComplete(PlacementMask) ||
		PathPlacementStep != 0 || !CurrentValidation.bValid)
	{
		AMSim::UIAudio::Play(this, EAMSimUISound::InvalidGeometry);
		return;
	}
	if (UWorld* World = GetWorld())
	{
		if (UAMSimAirportSimulationSubsystem* Subsystem =
			World->GetSubsystem<UAMSimAirportSimulationSubsystem>())
		{
			AMSim::FPhase1Command Command;
			Command.Type = AMSim::EPhase1CommandType::CommitStarterPlan;
			Command.Proposal = AMSim::NormalizeStarterPlanConnections(
				CurrentProposal);
			if (Subsystem->SubmitPhase1Command(MoveTemp(Command)) ==
				AMSim::EPhase1CommandResult::Accepted)
			{
				AMSim::UIAudio::Play(this, EAMSimUISound::BuildConfirm);
				AMSim::FPhase1Command Run;
				Run.Type = AMSim::EPhase1CommandType::SetSpeed;
				Run.SpeedMultiplier = 1;
				Subsystem->SubmitPhase1Command(MoveTemp(Run));
				CloseProposal();
				return;
			}
		}
	}
	AMSim::UIAudio::Play(this, EAMSimUISound::InvalidGeometry);
}

void UAMSimConstructionProposalView::CancelProposal()
{
	AMSim::UIAudio::Play(this, EAMSimUISound::BuildCancel);
	CurrentProposal = MakeEmptyProposal();
	PlacementMask = 0;
	SelectedPlacementTool = EPlacementTool::Runway;
	PathPlacementStep = 0;
	ActiveTaxiwaySegmentIndex = INDEX_NONE;
	DraggedTaxiwaySegmentIndex = INDEX_NONE;
	bCanUndo = false;
	RefreshProposalPresentation();
	CloseProposal();
}

void UAMSimConstructionProposalView::ResetPlacement()
{
	const AMSim::FStarterPlanProposal Reset = MakeEmptyProposal();
	const bool bHadPlacement =
		!AMSimConstructionProposalPrivate::ProposalEquals(CurrentProposal, Reset) ||
		PlacementMask != 0;
	if (bHadPlacement)
	{
		CaptureUndo(CurrentProposal);
		CurrentProposal = Reset;
		PlacementMask = 0;
		AMSim::UIAudio::Play(this, EAMSimUISound::BuildCancel);
	}
	SelectedPlacementTool = EPlacementTool::Runway;
	PathPlacementStep = 0;
	ActiveTaxiwaySegmentIndex = INDEX_NONE;
	DraggedTaxiwaySegmentIndex = INDEX_NONE;
	bSnapSoundActive = false;
	RefreshProposalPresentation();
}

void UAMSimConstructionProposalView::UndoPlacement()
{
	if (!bCanUndo)
	{
		return;
	}
	CurrentProposal = UndoProposal;
	PlacementMask = UndoPlacementMask;
	bCanUndo = false;
	PathPlacementStep = 0;
	ActiveTaxiwaySegmentIndex = INDEX_NONE;
	DraggedTaxiwaySegmentIndex = INDEX_NONE;
	bSnapSoundActive = false;
	AMSim::UIAudio::Play(this, EAMSimUISound::BuildCancel);
	RemoveDegenerateTaxiwaySegments();
	RefreshProposalPresentation();
}

void UAMSimConstructionProposalView::SelectRunwayTool()
{
	SelectTool(EPlacementTool::Runway);
}

void UAMSimConstructionProposalView::SelectTaxiwayTool()
{
	SelectTool(EPlacementTool::Taxiway);
}

void UAMSimConstructionProposalView::SelectRoadAccessTool()
{
	SelectTool(EPlacementTool::RoadAccess);
}

void UAMSimConstructionProposalView::SelectTool(const EPlacementTool Tool)
{
	const bool bChanged = SelectedPlacementTool != Tool;
	SelectedPlacementTool = Tool;
	PathPlacementStep = 0;
	ActiveTaxiwaySegmentIndex = INDEX_NONE;
	DraggedTaxiwaySegmentIndex = INDEX_NONE;
	bPathStartedByPress = false;
	bSnapSoundActive = false;
	if (bChanged)
	{
		AMSim::UIAudio::Play(this, EAMSimUISound::BuildStart);
	}
	RefreshProposalPresentation();
}

AMSim::FStarterPlanProposal
UAMSimConstructionProposalView::SetSelectedToolEndpoint(
	const AMSim::FStarterPlanProposal& Proposal,
	const int32 EndpointIndex,
	const AMSim::FPhase1Point Point)
{
	int32 TaxiwaySegmentIndex = ActiveTaxiwaySegmentIndex;
	if (DraggedTaxiwaySegmentIndex != INDEX_NONE)
	{
		TaxiwaySegmentIndex = DraggedTaxiwaySegmentIndex;
	}
	if (TaxiwaySegmentIndex == INDEX_NONE &&
		!Proposal.TaxiwaySegments.IsEmpty())
	{
		TaxiwaySegmentIndex = Proposal.TaxiwaySegments.Num() - 1;
	}
	const int32 ResolvedTaxiwaySegmentIndex = FMath::Max(TaxiwaySegmentIndex, 0);
	AMSim::FStarterPlanProposal Result = SetToolEndpoint(
		Proposal,
		SelectedPlacementTool,
		EndpointIndex,
		Point,
		IsToolPlaced(EPlacementTool::Runway),
		true,
		ResolvedTaxiwaySegmentIndex);
	if (SelectedPlacementTool == EPlacementTool::Taxiway &&
		Result.TaxiwaySegments.IsValidIndex(ResolvedTaxiwaySegmentIndex))
	{
		const AMSim::FTaxiwaySegment& Segment =
			Result.TaxiwaySegments[ResolvedTaxiwaySegmentIndex];
		const AMSim::FPhase1Point AppliedPoint =
			EndpointIndex == 0 ? Segment.Start : Segment.End;
		const bool bSnapped = AppliedPoint != Point;
		if (bSnapped &&
			(!bSnapSoundActive || AppliedPoint != LastSnapSoundPoint))
		{
			AMSim::UIAudio::Play(this, EAMSimUISound::BuildSnap);
		}
		bSnapSoundActive = bSnapped;
		if (bSnapped)
		{
			LastSnapSoundPoint = AppliedPoint;
		}
	}
	else
	{
		bSnapSoundActive = false;
	}
	return Result;
}

uint8 UAMSimConstructionProposalView::PlacementPartForTool(
	const EPlacementTool Tool) const
{
	switch (Tool)
	{
	case EPlacementTool::Runway:
		return RunwayPart;
	case EPlacementTool::Taxiway:
		return TaxiwayPart;
	case EPlacementTool::RoadAccess:
		return RoadAccessPart;
	}
	return 0;
}

bool UAMSimConstructionProposalView::IsToolPlaced(
	const EPlacementTool Tool) const
{
	return (PlacementMask & PlacementPartForTool(Tool)) != 0;
}

void UAMSimConstructionProposalView::SetToolPlaced(
	const EPlacementTool Tool,
	const bool bPlaced)
{
	const uint8 Part = PlacementPartForTool(Tool);
	PlacementMask = bPlaced
		? PlacementMask | Part
		: PlacementMask & ~Part;
}

FString UAMSimConstructionProposalView::DescribePlacementProgress() const
{
	const int32 Completed =
		(IsToolPlaced(EPlacementTool::Runway) ? 1 : 0) +
		(IsToolPlaced(EPlacementTool::Taxiway) ? 1 : 0);
	const TCHAR* Next = !IsToolPlaced(EPlacementTool::Runway)
		? TEXT("DRAW THE RUNWAY")
		: !IsToolPlaced(EPlacementTool::Taxiway)
			? TEXT("DRAW TAXIWAYS FROM THE RUNWAY TO GATE A OR B")
			: TEXT("CHECK THE COMPLETE TAXI NETWORK");
	return FString::Printf(TEXT("%d / 2 REQUIRED - %s"), Completed, Next);
}

void UAMSimConstructionProposalView::ApplyPointerDrag(
	const AMSim::FPhase1Point& PointerPoint,
	const bool bCenterOnPointer)
{
	if (bPathStartedByPress)
	{
		const AMSim::FStarterPlanProposal Updated = SetSelectedToolEndpoint(
			CurrentProposal,
			1,
			PointerPoint);
		if (!AMSimConstructionProposalPrivate::ProposalEquals(
			Updated,
			CurrentProposal))
		{
			CurrentProposal = Updated;
			RefreshProposalPresentation();
		}
		return;
	}
	AMSim::FStarterPlanProposal Updated = DragStartProposal;
	if (DraggedEndpointIndex != INDEX_NONE)
	{
		Updated = SetSelectedToolEndpoint(
			DragStartProposal,
			DraggedEndpointIndex,
			PointerPoint);
	}
	else
	{
		const AMSim::FPhase1Point Origin = bCenterOnPointer
			? GetSelectedToolCenter()
			: DragStartMapPoint;
		Updated = TranslateToolPlacement(
			DragStartProposal,
			SelectedPlacementTool,
			PointerPoint.X - Origin.X,
			PointerPoint.Y - Origin.Y,
			DraggedTaxiwaySegmentIndex);
	}
	if (!AMSimConstructionProposalPrivate::ProposalEquals(Updated, CurrentProposal))
	{
		CaptureUndo(DragStartProposal);
		CurrentProposal = Updated;
		if (SelectedPlacementTool == EPlacementTool::Taxiway)
		{
			SyncLegacyTaxiwayFields();
		}
		PlacementMask = DragStartPlacementMask;
		PathPlacementStep = 0;
		RefreshProposalPresentation();
	}
}

void UAMSimConstructionProposalView::ApplyPointerClick(
	const AMSim::FPhase1Point& PointerPoint)
{
	AMSim::FStarterPlanProposal Updated = CurrentProposal;
	if (SelectedPlacementTool == EPlacementTool::Runway ||
		SelectedPlacementTool == EPlacementTool::Taxiway ||
		SelectedPlacementTool == EPlacementTool::RoadAccess)
	{
		if (PathPlacementStep == 0)
		{
			if (SelectedPlacementTool == EPlacementTool::Taxiway &&
				CurrentProposal.TaxiwaySegments.Num() >= MaximumTaxiwaySegments)
			{
				RefreshProposalPresentation();
				return;
			}
			CaptureUndo(CurrentProposal);
			if (SelectedPlacementTool == EPlacementTool::Taxiway)
			{
				Updated.TaxiwaySegments = CurrentProposal.TaxiwaySegments;
				Updated.TaxiwaySegments.Add({PointerPoint, PointerPoint});
				ActiveTaxiwaySegmentIndex = Updated.TaxiwaySegments.Num() - 1;
				Updated = SetSelectedToolEndpoint(Updated, 0, PointerPoint);
			}
			else
			{
				Updated = SetSelectedToolEndpoint(CurrentProposal, 0, PointerPoint);
			}
			Updated = SetSelectedToolEndpoint(Updated, 1, PointerPoint);
			SetToolPlaced(SelectedPlacementTool, false);
			PathPlacementStep = 1;
		}
		else
		{
			const AMSim::FPhase1Point Start =
				SelectedPlacementTool == EPlacementTool::Runway
					? CurrentProposal.RunwayStart
					: SelectedPlacementTool == EPlacementTool::RoadAccess
						? CurrentProposal.AccessStart
						: CurrentProposal.TaxiwaySegments[
							ActiveTaxiwaySegmentIndex].Start;
			if (PointerPoint == Start)
			{
				RefreshProposalPresentation();
				return;
			}
			Updated = SetSelectedToolEndpoint(CurrentProposal, 1, PointerPoint);
			SetToolPlaced(SelectedPlacementTool, true);
			if (SelectedPlacementTool == EPlacementTool::Taxiway &&
				!Updated.TaxiwaySegments.IsEmpty())
			{
				Updated.TaxiStart = Updated.TaxiwaySegments[0].Start;
				Updated.TaxiEnd = Updated.TaxiwaySegments[0].End;
			}
			PathPlacementStep = 0;
			ActiveTaxiwaySegmentIndex = INDEX_NONE;
			AMSim::UIAudio::Play(this, EAMSimUISound::BuildPlace);
		}
	}
	if (!AMSimConstructionProposalPrivate::ProposalEquals(Updated, CurrentProposal))
	{
		CurrentProposal = Updated;
	}
	RefreshProposalPresentation();
}

void UAMSimConstructionProposalView::CaptureUndo(
	const AMSim::FStarterPlanProposal& Proposal)
{
	if (!bUndoCapturedForGesture)
	{
		UndoProposal = Proposal;
		UndoPlacementMask = PlacementMask;
		bCanUndo = true;
		bUndoCapturedForGesture = bPlacementDragging;
	}
}

AMSim::FPhase1Point
UAMSimConstructionProposalView::GetSelectedToolCenter() const
{
	switch (SelectedPlacementTool)
	{
	case EPlacementTool::Runway:
		return {
			(CurrentProposal.RunwayStart.X + CurrentProposal.RunwayEnd.X) / 2,
			(CurrentProposal.RunwayStart.Y + CurrentProposal.RunwayEnd.Y) / 2};
	case EPlacementTool::Taxiway:
	{
		const int32 Index = DraggedTaxiwaySegmentIndex != INDEX_NONE
			? DraggedTaxiwaySegmentIndex
			: FMath::Max(ActiveTaxiwaySegmentIndex, 0);
		if (CurrentProposal.TaxiwaySegments.IsValidIndex(Index))
		{
			const AMSim::FTaxiwaySegment& Segment =
				CurrentProposal.TaxiwaySegments[Index];
			return {
				(Segment.Start.X + Segment.End.X) / 2,
				(Segment.Start.Y + Segment.End.Y) / 2};
		}
		return {};
	}
	case EPlacementTool::RoadAccess:
		return {
			(CurrentProposal.AccessStart.X + CurrentProposal.AccessEnd.X) / 2,
			(CurrentProposal.AccessStart.Y + CurrentProposal.AccessEnd.Y) / 2};
	}
	return {};
}

int32 UAMSimConstructionProposalView::ResolveSelectedEndpoint(
	const AMSim::FPhase1Point& Point)
{
	using namespace AMSimConstructionProposalPrivate;
	const int64 HitDistanceSquared = HandleHitCentimeters * HandleHitCentimeters;
	if (SelectedPlacementTool == EPlacementTool::Runway &&
		(IsToolPlaced(EPlacementTool::Runway) || PathPlacementStep == 1))
	{
		if (DistanceSquared(Point, CurrentProposal.RunwayStart) <= HitDistanceSquared)
		{
			return 0;
		}
		if (DistanceSquared(Point, CurrentProposal.RunwayEnd) <= HitDistanceSquared)
		{
			return 1;
		}
	}
	if (SelectedPlacementTool == EPlacementTool::Taxiway &&
		(IsToolPlaced(EPlacementTool::Taxiway) || PathPlacementStep == 1))
	{
		const FTaxiwayEditHit Hit = ResolveTaxiwayEditHit(CurrentProposal, Point);
		DraggedTaxiwaySegmentIndex = Hit.SegmentIndex;
		return Hit.EndpointIndex;
	}
	if (SelectedPlacementTool == EPlacementTool::RoadAccess &&
		(IsToolPlaced(EPlacementTool::RoadAccess) || PathPlacementStep == 1))
	{
		if (DistanceSquared(Point, CurrentProposal.AccessStart) <= HitDistanceSquared)
		{
			return 0;
		}
		if (DistanceSquared(Point, CurrentProposal.AccessEnd) <= HitDistanceSquared)
		{
			return 1;
		}
	}
	return INDEX_NONE;
}

UAMSimConstructionProposalView::FTaxiwayEditHit
UAMSimConstructionProposalView::ResolveTaxiwayEditHit(
	const AMSim::FStarterPlanProposal& Proposal,
	const AMSim::FPhase1Point& Point)
{
	using namespace AMSimConstructionProposalPrivate;
	const int64 HitDistanceSquared = HandleHitCentimeters * HandleHitCentimeters;
	for (int32 Index = Proposal.TaxiwaySegments.Num() - 1; Index >= 0; --Index)
	{
		const AMSim::FTaxiwaySegment& Segment = Proposal.TaxiwaySegments[Index];
		if (DistanceSquared(Point, Segment.Start) <= HitDistanceSquared)
		{
			return {Index, 0};
		}
		if (DistanceSquared(Point, Segment.End) <= HitDistanceSquared)
		{
			return {Index, 1};
		}
	}
	// A body press deliberately returns no edit target. The normal taxiway
	// placement path then snaps a new segment to the existing network instead
	// of translating the selected segment.
	return {};
}

bool UAMSimConstructionProposalView::IsWorldPlacementPosition(
	const FVector2D& LocalPosition,
	const FVector2D& LocalSize) const
{
	if (LocalSize.X <= 0.0 || LocalSize.Y <= 0.0)
	{
		return false;
	}
	const FVector2D Normalized(
		LocalPosition.X / LocalSize.X,
		LocalPosition.Y / LocalSize.Y);
	return Normalized.X >= 0.195f && Normalized.X <= 0.795f &&
		Normalized.Y >= 0.09f && Normalized.Y <= 0.86f;
}
