#include "AMSimPhase1Fixture.h"

namespace AMSim
{
	namespace
	{
		constexpr int64 ParcelMinimum = 0;
		constexpr int64 ParcelMaximum = 100000;
		constexpr int64 ConnectionTolerance = 500;
		constexpr int32 MaximumTaxiwaySegments = 16;
		constexpr int64 RoadBenefitTolerance = 15000;

		bool IsInsideParcel(const FPhase1Point Point)
		{
			return Point.X >= ParcelMinimum &&
				Point.Y >= ParcelMinimum &&
				Point.X <= ParcelMaximum &&
				Point.Y <= ParcelMaximum;
		}

		int64 DistanceSquared(const FPhase1Point Left, const FPhase1Point Right)
		{
			const int64 DeltaX = Left.X - Right.X;
			const int64 DeltaY = Left.Y - Right.Y;
			return DeltaX * DeltaX + DeltaY * DeltaY;
		}

		double CrossProduct(
			const FPhase1Point& Origin,
			const FPhase1Point& First,
			const FPhase1Point& Second)
		{
			return static_cast<double>(First.X - Origin.X) *
				static_cast<double>(Second.Y - Origin.Y) -
				static_cast<double>(First.Y - Origin.Y) *
				static_cast<double>(Second.X - Origin.X);
		}

		bool IsBetween(
			const int64 Value,
			const int64 First,
			const int64 Second)
		{
			return Value >= FMath::Min(First, Second) &&
				Value <= FMath::Max(First, Second);
		}

		bool IsOnSegment(
			const FPhase1Point& Point,
			const FPhase1Point& Start,
			const FPhase1Point& End)
		{
			return FMath::IsNearlyZero(CrossProduct(Start, End, Point), 0.5) &&
				IsBetween(Point.X, Start.X, End.X) &&
				IsBetween(Point.Y, Start.Y, End.Y);
		}

		bool AreSegmentsConnected(
			const FTaxiwaySegment& First,
			const FTaxiwaySegment& Second,
			const int64 ToleranceCentimeters)
		{
			return DoSegmentsIntersect(
				First.Start,
				First.End,
				Second.Start,
				Second.End) ||
				IsPointNearSegment(First.Start, Second.Start, Second.End, ToleranceCentimeters) ||
				IsPointNearSegment(First.End, Second.Start, Second.End, ToleranceCentimeters) ||
				IsPointNearSegment(Second.Start, First.Start, First.End, ToleranceCentimeters) ||
				IsPointNearSegment(Second.End, First.Start, First.End, ToleranceCentimeters);
		}

		FDefinition MakeDefinition(
			const TCHAR* StableId,
			const TCHAR* Tag,
			const EProvenanceStatus Provenance = EProvenanceStatus::Approved)
		{
			FDefinition Definition;
			Definition.StableId = StableId;
			Definition.OwningPhase = TEXT("Phase1");
			Definition.Tags.Add(Tag);
			Definition.Provenance = Provenance;
			Definition.SourcePath = TEXT("ProjectAuthored");
			Definition.SourceChecksum = TEXT("internal-phase1");
			return Definition;
		}
	}

	const FPhase1Fixture& GetPhase1Fixture()
	{
		static const FPhase1Fixture Fixture = []
		{
			FPhase1Fixture Value;
			Value.ContentDefinitions = {
				MakeDefinition(TEXT("Map.TemperateStarter"), TEXT("Facility.Type.Map")),
				MakeDefinition(TEXT("Aircraft.LightPiston.Starter"), TEXT("Aircraft.Role.GeneralAviation")),
				MakeDefinition(TEXT("Operator.RiverbendFlyingClub"), TEXT("Capability.Path.GeneralAviation")),
				MakeDefinition(TEXT("Facility.GrassRunway.Starter"), TEXT("Facility.Type.Runway")),
				MakeDefinition(TEXT("Facility.GrassTaxiway.Starter"), TEXT("Network.Type.AircraftMovement")),
				MakeDefinition(TEXT("Facility.GAStand.Starter"), TEXT("Facility.Type.Stand")),
				MakeDefinition(TEXT("Facility.OperationsHut.Starter"), TEXT("Facility.Type.Operations")),
				MakeDefinition(TEXT("Staff.Construction.Starter"), TEXT("Staff.Role.Construction")),
				MakeDefinition(TEXT("Staff.Ramp.Starter"), TEXT("Staff.Role.Ramp")),
				MakeDefinition(TEXT("Staff.Fueling.Starter"), TEXT("Staff.Role.Fueling"))
			};
			return Value;
		}();
		return Fixture;
	}

	FStarterPlanProposal CreateDefaultStarterPlan()
	{
		FStarterPlanProposal Proposal;
		Proposal.RunwayStart = {20000, 40000};
		Proposal.RunwayEnd = {80000, 40000};
		Proposal.RunwayWidthCentimeters = 2000;
		Proposal.TaxiStart = {45000, 40000};
		Proposal.TaxiEnd = {45000, 74000};
		Proposal.TaxiwaySegments.Add({Proposal.TaxiStart, Proposal.TaxiEnd});
		Proposal.StandCenter = GetStarterGatePoints()[0];
		Proposal.AccessStart = {50000, 82000};
		Proposal.AccessEnd = {50000, 100000};
		Proposal.OperationsHutCenter = GetStarterTerminalCenter();
		return Proposal;
	}

	const TArray<FPhase1Point>& GetStarterGatePoints()
	{
		static const TArray<FPhase1Point> GatePoints = {
			{45000, 74000},
			{55000, 74000}};
		return GatePoints;
	}

	FPhase1Point GetStarterTerminalCenter()
	{
		return {50000, 82000};
	}

	TArray<FTaxiwaySegment> GetTaxiwaySegments(
		const FStarterPlanProposal& Proposal)
	{
		if (!Proposal.TaxiwaySegments.IsEmpty())
		{
			return Proposal.TaxiwaySegments;
		}
		if (Proposal.TaxiStart != Proposal.TaxiEnd)
		{
			return {{Proposal.TaxiStart, Proposal.TaxiEnd}};
		}
		return {};
	}

	FRunwayDesignation CalculateRunwayDesignation(
		const FPhase1Point& Start,
		const FPhase1Point& End)
	{
		const double DeltaX = static_cast<double>(End.X - Start.X);
		const double DeltaY = static_cast<double>(End.Y - Start.Y);
		FRunwayDesignation Result;
		Result.HeadingDegrees = FMath::RadiansToDegrees(
			FMath::Atan2(DeltaX, -DeltaY));
		if (Result.HeadingDegrees < 0.0)
		{
			Result.HeadingDegrees += 360.0;
		}
		Result.PrimaryNumber = FMath::RoundToInt(Result.HeadingDegrees / 10.0);
		Result.PrimaryNumber = Result.PrimaryNumber == 0
			? 36
			: FMath::Clamp(Result.PrimaryNumber, 1, 36);
		Result.ReciprocalNumber =
			((Result.PrimaryNumber + 18 - 1) % 36) + 1;
		return Result;
	}

	FPhase1Point ClosestPointOnSegment(
		const FPhase1Point& Point,
		const FPhase1Point& SegmentStart,
		const FPhase1Point& SegmentEnd)
	{
		const double DeltaX = static_cast<double>(SegmentEnd.X - SegmentStart.X);
		const double DeltaY = static_cast<double>(SegmentEnd.Y - SegmentStart.Y);
		const double LengthSquared = DeltaX * DeltaX + DeltaY * DeltaY;
		if (LengthSquared <= UE_DOUBLE_SMALL_NUMBER)
		{
			return SegmentStart;
		}
		const double Projection =
			(static_cast<double>(Point.X - SegmentStart.X) * DeltaX +
				static_cast<double>(Point.Y - SegmentStart.Y) * DeltaY) /
			LengthSquared;
		const double T = FMath::Clamp(Projection, 0.0, 1.0);
		return {
			FMath::RoundToInt64(static_cast<double>(SegmentStart.X) + DeltaX * T),
			FMath::RoundToInt64(static_cast<double>(SegmentStart.Y) + DeltaY * T)};
	}

	bool IsPointNearSegment(
		const FPhase1Point& Point,
		const FPhase1Point& SegmentStart,
		const FPhase1Point& SegmentEnd,
		const int64 ToleranceCentimeters)
	{
		return DistanceSquared(
			Point,
			ClosestPointOnSegment(Point, SegmentStart, SegmentEnd)) <=
			ToleranceCentimeters * ToleranceCentimeters;
	}

	bool DoSegmentsIntersect(
		const FPhase1Point& FirstStart,
		const FPhase1Point& FirstEnd,
		const FPhase1Point& SecondStart,
		const FPhase1Point& SecondEnd)
	{
		const double A = CrossProduct(FirstStart, FirstEnd, SecondStart);
		const double B = CrossProduct(FirstStart, FirstEnd, SecondEnd);
		const double C = CrossProduct(SecondStart, SecondEnd, FirstStart);
		const double D = CrossProduct(SecondStart, SecondEnd, FirstEnd);
		const bool bProperIntersection =
			((A > 0.0 && B < 0.0) || (A < 0.0 && B > 0.0)) &&
			((C > 0.0 && D < 0.0) || (C < 0.0 && D > 0.0));
		return bProperIntersection ||
			(FMath::IsNearlyZero(A, 0.5) && IsOnSegment(SecondStart, FirstStart, FirstEnd)) ||
			(FMath::IsNearlyZero(B, 0.5) && IsOnSegment(SecondEnd, FirstStart, FirstEnd)) ||
			(FMath::IsNearlyZero(C, 0.5) && IsOnSegment(FirstStart, SecondStart, SecondEnd)) ||
			(FMath::IsNearlyZero(D, 0.5) && IsOnSegment(FirstEnd, SecondStart, SecondEnd));
	}

	bool TryFindSegmentIntersection(
		const FPhase1Point& FirstStart,
		const FPhase1Point& FirstEnd,
		const FPhase1Point& SecondStart,
		const FPhase1Point& SecondEnd,
		FPhase1Point& OutIntersection)
	{
		if (!DoSegmentsIntersect(FirstStart, FirstEnd, SecondStart, SecondEnd))
		{
			return false;
		}
		const double FirstX = static_cast<double>(FirstStart.X);
		const double FirstY = static_cast<double>(FirstStart.Y);
		const double FirstDeltaX = static_cast<double>(FirstEnd.X - FirstStart.X);
		const double FirstDeltaY = static_cast<double>(FirstEnd.Y - FirstStart.Y);
		const double SecondDeltaX = static_cast<double>(SecondEnd.X - SecondStart.X);
		const double SecondDeltaY = static_cast<double>(SecondEnd.Y - SecondStart.Y);
		const double Denominator =
			FirstDeltaX * SecondDeltaY - FirstDeltaY * SecondDeltaX;
		if (!FMath::IsNearlyZero(Denominator, UE_DOUBLE_SMALL_NUMBER))
		{
			const double OffsetX = static_cast<double>(SecondStart.X - FirstStart.X);
			const double OffsetY = static_cast<double>(SecondStart.Y - FirstStart.Y);
			const double T =
				(OffsetX * SecondDeltaY - OffsetY * SecondDeltaX) / Denominator;
			OutIntersection = {
				FMath::RoundToInt64(FirstX + FirstDeltaX * T),
				FMath::RoundToInt64(FirstY + FirstDeltaY * T)};
			return true;
		}
		for (const FPhase1Point Candidate : {
			FirstStart,
			FirstEnd,
			SecondStart,
			SecondEnd})
		{
			if (IsOnSegment(Candidate, FirstStart, FirstEnd) &&
				IsOnSegment(Candidate, SecondStart, SecondEnd))
			{
				OutIntersection = Candidate;
				return true;
			}
		}
		return false;
	}

	bool IsTaxiwayConnectedToRunway(
		const FStarterPlanProposal& Proposal,
		const int64 ToleranceCentimeters)
	{
		return ValidateTaxiwayNetwork(Proposal, ToleranceCentimeters)
			.bHasRunwayConnection;
	}

	FTaxiwayNetworkValidation ValidateTaxiwayNetwork(
		const FStarterPlanProposal& Proposal,
		const int64 ToleranceCentimeters)
	{
		FTaxiwayNetworkValidation Result;
		const TArray<FTaxiwaySegment> Segments = GetTaxiwaySegments(Proposal);
		if (Segments.IsEmpty() || Segments.Num() > MaximumTaxiwaySegments)
		{
			return Result;
		}

		TArray<bool> Visited;
		Visited.Init(false, Segments.Num());
		TArray<int32> Open;
		for (int32 Index = 0; Index < Segments.Num(); ++Index)
		{
			const FTaxiwaySegment& Segment = Segments[Index];
			const bool bTouchesRunway =
				DoSegmentsIntersect(
					Proposal.RunwayStart,
					Proposal.RunwayEnd,
					Segment.Start,
					Segment.End) ||
				IsPointNearSegment(
					Segment.Start,
					Proposal.RunwayStart,
					Proposal.RunwayEnd,
					ToleranceCentimeters) ||
				IsPointNearSegment(
					Segment.End,
					Proposal.RunwayStart,
					Proposal.RunwayEnd,
					ToleranceCentimeters);
			if (bTouchesRunway)
			{
				Result.bHasRunwayConnection = true;
				Visited[Index] = true;
				Open.Add(Index);
			}
		}

		for (int32 Cursor = 0; Cursor < Open.Num(); ++Cursor)
		{
			const int32 Current = Open[Cursor];
			for (int32 Candidate = 0; Candidate < Segments.Num(); ++Candidate)
			{
				if (!Visited[Candidate] && AreSegmentsConnected(
					Segments[Current],
					Segments[Candidate],
					ToleranceCentimeters))
				{
					Visited[Candidate] = true;
					Open.Add(Candidate);
				}
			}
		}

		for (int32 Index = 0; Index < Segments.Num(); ++Index)
		{
			if (Visited[Index])
			{
				++Result.ConnectedSegmentCount;
				for (int32 GateIndex = 0;
					GateIndex < GetStarterGatePoints().Num();
					++GateIndex)
				{
					if (IsPointNearSegment(
						GetStarterGatePoints()[GateIndex],
						Segments[Index].Start,
						Segments[Index].End,
						ToleranceCentimeters))
					{
						Result.bHasGateConnection = true;
						if (Result.ConnectedGateIndex == INDEX_NONE)
						{
							Result.ConnectedGateIndex = GateIndex;
						}
					}
				}
			}
			else if (Result.FirstDisconnectedSegment == INDEX_NONE)
			{
				Result.FirstDisconnectedSegment = Index;
			}
		}
		Result.bAllSegmentsConnected =
			Result.ConnectedSegmentCount == Segments.Num();
		return Result;
	}

	FStarterPlanProposal NormalizeStarterPlanConnections(
		const FStarterPlanProposal& Proposal)
	{
		FStarterPlanProposal Result = Proposal;
		const TArray<FTaxiwaySegment> Segments = GetTaxiwaySegments(Result);
		Result.TaxiwaySegments = Segments;
		if (!Segments.IsEmpty())
		{
			Result.TaxiStart = Segments[0].Start;
			Result.TaxiEnd = Segments[0].End;
		}
		const FTaxiwayNetworkValidation Network =
			ValidateTaxiwayNetwork(Result, ConnectionTolerance);
		if (GetStarterGatePoints().IsValidIndex(Network.ConnectedGateIndex))
		{
			Result.StandCenter =
				GetStarterGatePoints()[Network.ConnectedGateIndex];
		}
		Result.OperationsHutCenter = GetStarterTerminalCenter();
		return Result;
	}

	bool HasConstructionRoadBenefit(const FStarterPlanProposal& Proposal)
	{
		if (Proposal.AccessStart == Proposal.AccessEnd)
		{
			return false;
		}
		const FTaxiwaySegment Road{Proposal.AccessStart, Proposal.AccessEnd};
		const FTaxiwaySegment Runway{Proposal.RunwayStart, Proposal.RunwayEnd};
		if (AreSegmentsConnected(Road, Runway, RoadBenefitTolerance) ||
			IsPointNearSegment(
				GetStarterTerminalCenter(),
				Road.Start,
				Road.End,
				RoadBenefitTolerance))
		{
			return true;
		}
		for (const FTaxiwaySegment& Segment : GetTaxiwaySegments(Proposal))
		{
			if (AreSegmentsConnected(Road, Segment, RoadBenefitTolerance))
			{
				return true;
			}
		}
		return false;
	}

	FPhase1Validation ValidateStarterPlan(const FStarterPlanProposal& Proposal)
	{
		const FPhase1Fixture& Fixture = GetPhase1Fixture();
		FPhase1Validation Result;
		Result.QuotedCost = Fixture.StarterPlanCost;
		if (Proposal.RunwayWidthCentimeters < 1800 ||
			DistanceSquared(Proposal.RunwayStart, Proposal.RunwayEnd) < 60000ll * 60000ll)
		{
			Result.Result = EPhase1CommandResult::RejectedInvalidGeometry;
			Result.ReasonCode = TEXT("Build.Runway.TooSmall");
			Result.Cause = TEXT("The grass runway is too short or narrow for the starter aircraft.");
			Result.Remedy = TEXT("Use a runway at least 600 m long and 18 m wide.");
			return Result;
		}

		TArray<FPhase1Point> RequiredPoints = {
			Proposal.RunwayStart,
			Proposal.RunwayEnd,
			GetStarterTerminalCenter()
		};
		const TArray<FTaxiwaySegment> TaxiwaySegments =
			GetTaxiwaySegments(Proposal);
		for (const FTaxiwaySegment& Segment : TaxiwaySegments)
		{
			RequiredPoints.Add(Segment.Start);
			RequiredPoints.Add(Segment.End);
			if (DistanceSquared(Segment.Start, Segment.End) < 1000ll * 1000ll)
			{
				Result.Result = EPhase1CommandResult::RejectedInvalidGeometry;
				Result.ReasonCode = TEXT("Build.Taxiway.TooShort");
				Result.Cause = TEXT("A taxiway segment is too short to use.");
				Result.Remedy = TEXT("Draw the segment at least 10 m long or undo it.");
				return Result;
			}
		}
		if (Proposal.AccessStart != Proposal.AccessEnd)
		{
			RequiredPoints.Add(Proposal.AccessStart);
			RequiredPoints.Add(Proposal.AccessEnd);
		}
		for (const FPhase1Point Point : RequiredPoints)
		{
			if (!IsInsideParcel(Point))
			{
				Result.Result = EPhase1CommandResult::RejectedOutsideOwnedLand;
				Result.ReasonCode = TEXT("Build.OutsideOwnedLand");
				Result.Cause = TEXT("Part of the starter airfield is outside the owned parcel.");
				Result.Remedy = TEXT("Move the highlighted part inside the cyan parcel boundary.");
				return Result;
			}
		}

		const FTaxiwayNetworkValidation Network =
			ValidateTaxiwayNetwork(Proposal, ConnectionTolerance);
		if (!Network.bHasRunwayConnection)
		{
			Result.Result = EPhase1CommandResult::RejectedDisconnected;
			Result.ReasonCode = TEXT("Build.Network.NoRunwayConnection");
			Result.Cause = TEXT("The taxiway network does not reach the runway.");
			Result.Remedy = TEXT("Join or cross the highlighted runway with a taxiway.");
			return Result;
		}
		if (!Network.bHasGateConnection)
		{
			Result.Result = EPhase1CommandResult::RejectedDisconnected;
			Result.ReasonCode = TEXT("Build.Network.NoGateConnection");
			Result.Cause = TEXT("The taxiway network does not reach a terminal gate.");
			Result.Remedy = TEXT("Continue the network to Gate A or Gate B.");
			return Result;
		}
		if (!Network.bAllSegmentsConnected)
		{
			Result.Result = EPhase1CommandResult::RejectedDisconnected;
			Result.ReasonCode = TEXT("Build.Network.OrphanSegment");
			Result.Cause = TEXT("A proposed taxiway segment is outside the usable network.");
			Result.Remedy = TEXT("Join the highlighted segment to another taxiway or remove it.");
			return Result;
		}

		Result.bValid = true;
		Result.Result = EPhase1CommandResult::Accepted;
		Result.ReasonCode = TEXT("Build.Valid");
		Result.Cause = TEXT("The runway connects through the taxiway network to a terminal gate.");
		Result.Remedy = TEXT("Commit the project when ready.");
		return Result;
	}

	FDefinitionValidation ValidatePhase1FixtureDefinitions()
	{
		return ValidateDefinitionRegistry(GetPhase1Fixture().ContentDefinitions, true);
	}

	int32 CalculateContingencyPercent(const int64 StartingCredits, const int64 PlanCost)
	{
		if (StartingCredits <= 0 || PlanCost < 0 || PlanCost > StartingCredits)
		{
			return 0;
		}
		return static_cast<int32>(((StartingCredits - PlanCost) * 100) / StartingCredits);
	}
}
