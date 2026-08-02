#include "AMSimPhase1AircraftPresentation.h"

#include "AMSimPhase1Fixture.h"
#include "AMSimPhase1WorldGeometry.h"
#include "Algo/Reverse.h"

namespace
{
	constexpr int64 ConnectionToleranceCentimeters = 500;

	double DistanceSquared(
		const AMSim::FPhase1Point& Left,
		const AMSim::FPhase1Point& Right)
	{
		const double DeltaX = static_cast<double>(Left.X - Right.X);
		const double DeltaY = static_cast<double>(Left.Y - Right.Y);
		return DeltaX * DeltaX + DeltaY * DeltaY;
	}

	AMSim::FPhase1Point Midpoint(
		const AMSim::FPhase1Point& Left,
		const AMSim::FPhase1Point& Right)
	{
		return {
			(Left.X + Right.X) / 2,
			(Left.Y + Right.Y) / 2};
	}

	bool TryJoinSegments(
		const AMSim::FTaxiwaySegment& Left,
		const AMSim::FTaxiwaySegment& Right,
		AMSim::FPhase1Point& OutJoin)
	{
		if (AMSim::TryFindSegmentIntersection(
			Left.Start,
			Left.End,
			Right.Start,
			Right.End,
			OutJoin))
		{
			return true;
		}

		double BestDistance = TNumericLimits<double>::Max();
		AMSim::FPhase1Point BestLeft;
		AMSim::FPhase1Point BestRight;
		for (const AMSim::FPhase1Point Candidate : {Left.Start, Left.End})
		{
			const AMSim::FPhase1Point Other = AMSim::ClosestPointOnSegment(
				Candidate,
				Right.Start,
				Right.End);
			const double CandidateDistance = DistanceSquared(Candidate, Other);
			if (CandidateDistance < BestDistance)
			{
				BestDistance = CandidateDistance;
				BestLeft = Candidate;
				BestRight = Other;
			}
		}
		for (const AMSim::FPhase1Point Candidate : {Right.Start, Right.End})
		{
			const AMSim::FPhase1Point Other = AMSim::ClosestPointOnSegment(
				Candidate,
				Left.Start,
				Left.End);
			const double CandidateDistance = DistanceSquared(Candidate, Other);
			if (CandidateDistance < BestDistance)
			{
				BestDistance = CandidateDistance;
				BestLeft = Other;
				BestRight = Candidate;
			}
		}
		if (BestDistance >
			ConnectionToleranceCentimeters * ConnectionToleranceCentimeters)
		{
			return false;
		}
		OutJoin = Midpoint(BestLeft, BestRight);
		return true;
	}

	bool TryRunwayJoin(
		const AMSim::FStarterPlanProposal& Proposal,
		const AMSim::FTaxiwaySegment& Segment,
		AMSim::FPhase1Point& OutJoin)
	{
		if (AMSim::TryFindSegmentIntersection(
			Proposal.RunwayStart,
			Proposal.RunwayEnd,
			Segment.Start,
			Segment.End,
			OutJoin))
		{
			return true;
		}
		double BestDistance = TNumericLimits<double>::Max();
		AMSim::FPhase1Point BestSegmentPoint;
		AMSim::FPhase1Point BestRunwayPoint;
		for (const AMSim::FPhase1Point Candidate : {Segment.Start, Segment.End})
		{
			const AMSim::FPhase1Point RunwayPoint = AMSim::ClosestPointOnSegment(
				Candidate,
				Proposal.RunwayStart,
				Proposal.RunwayEnd);
			const double CandidateDistance = DistanceSquared(Candidate, RunwayPoint);
			if (CandidateDistance < BestDistance)
			{
				BestDistance = CandidateDistance;
				BestSegmentPoint = Candidate;
				BestRunwayPoint = RunwayPoint;
			}
		}
		if (BestDistance >
			ConnectionToleranceCentimeters * ConnectionToleranceCentimeters)
		{
			return false;
		}
		OutJoin = Midpoint(BestSegmentPoint, BestRunwayPoint);
		return true;
	}

	bool TryGateJoin(
		const AMSim::FTaxiwaySegment& Segment,
		const AMSim::FPhase1Point& Gate,
		AMSim::FPhase1Point& OutJoin)
	{
		const AMSim::FPhase1Point Closest = AMSim::ClosestPointOnSegment(
			Gate,
			Segment.Start,
			Segment.End);
		if (DistanceSquared(Gate, Closest) >
			ConnectionToleranceCentimeters * ConnectionToleranceCentimeters)
		{
			return false;
		}
		OutJoin = Gate;
		return true;
	}

	void AddUniquePoint(TArray<AMSim::FPhase1Point>& Points, const AMSim::FPhase1Point Point)
	{
		if (Points.IsEmpty() || Points.Last() != Point)
		{
			Points.Add(Point);
		}
	}

	FVector PointAlongPath(
		const TConstArrayView<FVector> Points,
		const float Progress,
		FVector& OutDirection)
	{
		if (Points.IsEmpty())
		{
			return FVector::ZeroVector;
		}
		if (Points.Num() == 1)
		{
			return Points[0];
		}
		double TotalLength = 0.0;
		for (int32 Index = 1; Index < Points.Num(); ++Index)
		{
			TotalLength += FVector::Dist2D(Points[Index - 1], Points[Index]);
		}
		const double Target = FMath::Clamp(Progress, 0.0f, 1.0f) * TotalLength;
		double Traversed = 0.0;
		for (int32 Index = 1; Index < Points.Num(); ++Index)
		{
			const FVector Segment = Points[Index] - Points[Index - 1];
			const double SegmentLength = Segment.Size2D();
			if (Target <= Traversed + SegmentLength || Index == Points.Num() - 1)
			{
				OutDirection = Segment.GetSafeNormal2D();
				const double LocalProgress = SegmentLength > UE_DOUBLE_SMALL_NUMBER
					? FMath::Clamp((Target - Traversed) / SegmentLength, 0.0, 1.0)
					: 0.0;
				return FMath::Lerp(Points[Index - 1], Points[Index], LocalProgress);
			}
			Traversed += SegmentLength;
		}
		return Points.Last();
	}

	float StateProgress(
		const AMSim::FPhase1QuerySnapshot& Query,
		const AMSim::FPhase1State& State)
	{
		const int64 Arrival = State.Flight.ScheduledArrivalGameMilliseconds;
		const int64 Start = Arrival +
			AMSim::GetPhase1FlightStateOffsetMilliseconds(Query.FlightState);
		const int64 End = Arrival +
			AMSim::GetPhase1FlightStateEndOffsetMilliseconds(Query.FlightState);
		return End > Start
			? FMath::Clamp(
				static_cast<float>(Query.GameTimeMilliseconds - Start) /
					static_cast<float>(End - Start),
				0.0f,
				1.0f)
			: 1.0f;
	}
}

namespace AMSim
{
	TArray<FVector> FPhase1AircraftPresentation::BuildTaxiPath(
		const FStarterPlanProposal& Proposal,
		const double Height)
	{
		const TArray<FTaxiwaySegment> Segments = GetTaxiwaySegments(Proposal);
		if (Segments.IsEmpty())
		{
			return {};
		}

		const FTaxiwayNetworkValidation Network = ValidateTaxiwayNetwork(Proposal);
		const int32 GateIndex = Network.ConnectedGateIndex != INDEX_NONE
			? Network.ConnectedGateIndex
			: 0;
		const FPhase1Point Gate = GetStarterGatePoints().IsValidIndex(GateIndex)
			? GetStarterGatePoints()[GateIndex]
			: Proposal.StandCenter;

		TArray<int32> Parent;
		Parent.Init(INDEX_NONE, Segments.Num());
		TArray<FPhase1Point> RunwayJoins;
		RunwayJoins.SetNum(Segments.Num());
		TArray<int32> Open;
		for (int32 Index = 0; Index < Segments.Num(); ++Index)
		{
			if (TryRunwayJoin(Proposal, Segments[Index], RunwayJoins[Index]))
			{
				Parent[Index] = -2;
				Open.Add(Index);
			}
		}

		int32 Goal = INDEX_NONE;
		for (int32 Cursor = 0; Cursor < Open.Num() && Goal == INDEX_NONE; ++Cursor)
		{
			const int32 Current = Open[Cursor];
			FPhase1Point GateJoin;
			if (TryGateJoin(Segments[Current], Gate, GateJoin))
			{
				Goal = Current;
				break;
			}
			for (int32 Candidate = 0; Candidate < Segments.Num(); ++Candidate)
			{
				FPhase1Point Join;
				if (Parent[Candidate] == INDEX_NONE &&
					TryJoinSegments(Segments[Current], Segments[Candidate], Join))
				{
					Parent[Candidate] = Current;
					Open.Add(Candidate);
				}
			}
		}
		if (Goal == INDEX_NONE)
		{
			return {};
		}

		TArray<int32> Route;
		for (int32 Index = Goal; Index >= 0; Index = Parent[Index])
		{
			Route.Add(Index);
			if (Parent[Index] == -2)
			{
				break;
			}
		}
		Algo::Reverse(Route);

		TArray<FPhase1Point> RoutePoints;
		AddUniquePoint(RoutePoints, RunwayJoins[Route[0]]);
		for (int32 Index = 1; Index < Route.Num(); ++Index)
		{
			FPhase1Point Join;
			if (TryJoinSegments(
				Segments[Route[Index - 1]],
				Segments[Route[Index]],
				Join))
			{
				AddUniquePoint(RoutePoints, Join);
			}
		}
		AddUniquePoint(RoutePoints, Gate);

		TArray<FVector> WorldPoints;
		WorldPoints.Reserve(RoutePoints.Num());
		for (const FPhase1Point& Point : RoutePoints)
		{
			WorldPoints.Add(MapPhase1PointToWorld(Point, Height));
		}
		return WorldPoints;
	}

	FPhase1AircraftVisualState FPhase1AircraftPresentation::Derive(
		const FPhase1QuerySnapshot& Query,
		const FPhase1State& State)
	{
		FPhase1AircraftVisualState Result;
		Result.bVisible = Query.FlightState >= EFlightState::Inbound &&
			Query.FlightState <= EFlightState::Outbound;
		if (!Result.bVisible)
		{
			return Result;
		}

		const FStarterPlanProposal& Proposal = State.Project.Proposal;
		const FVector RunwayStart = MapPhase1PointToWorld(Proposal.RunwayStart, 60.0);
		const FVector RunwayEnd = MapPhase1PointToWorld(Proposal.RunwayEnd, 60.0);
		const FVector RunwayDirection = (RunwayEnd - RunwayStart).GetSafeNormal2D();
		TArray<FVector> TaxiPath = BuildTaxiPath(Proposal);
		if (TaxiPath.Num() < 2)
		{
			TaxiPath = {
				FMath::Lerp(RunwayStart, RunwayEnd, 0.5),
				MapPhase1PointToWorld(Proposal.StandCenter, 60.0)};
		}
		Result.TaxiPathPointCount = TaxiPath.Num();
		const FVector RunwayExit = FMath::ClosestPointOnSegment(
			TaxiPath[0],
			RunwayStart,
			RunwayEnd);
		const FVector RollStart = FMath::Lerp(RunwayStart, RunwayExit, 0.35);
		const FVector ApproachFix = RunwayStart - RunwayDirection * 25000.0;
		const FVector EntryFix = RunwayStart - RunwayDirection * 70000.0;
		const FVector OutboundFix = RunwayEnd + RunwayDirection * 70000.0;

		Result.StateProgress = StateProgress(Query, State);
		TArray<FVector> Path;
		switch (Query.FlightState)
		{
		case EFlightState::Inbound:
			Path = {EntryFix, ApproachFix};
			break;
		case EFlightState::Approach:
			Path = {ApproachFix, RunwayStart};
			break;
		case EFlightState::Landing:
			Path = {RunwayStart, RollStart};
			break;
		case EFlightState::RunwayRoll:
			Path = {RollStart, RunwayExit};
			break;
		case EFlightState::TaxiIn:
			Path.Add(RunwayExit);
			Path.Append(TaxiPath);
			break;
		case EFlightState::Parked:
		case EFlightState::Turnaround:
		case EFlightState::Ready:
			Path = {TaxiPath[TaxiPath.Num() - 2], TaxiPath.Last()};
			Result.StateProgress = 1.0f;
			break;
		case EFlightState::TaxiOut:
			for (int32 Index = TaxiPath.Num() - 1; Index >= 0; --Index)
			{
				Path.Add(TaxiPath[Index]);
			}
			Path.Add(RunwayExit);
			Path.Add(RunwayStart);
			break;
		case EFlightState::Takeoff:
			Path = {RunwayStart, RunwayEnd};
			break;
		case EFlightState::Outbound:
			Path = {RunwayEnd, OutboundFix};
			break;
		default:
			break;
		}
		Result.Location = PointAlongPath(
			Path,
			Result.StateProgress,
			Result.Direction);
		return Result;
	}
}
