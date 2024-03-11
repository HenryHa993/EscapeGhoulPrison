#pragma once
#include "Engine/Implementation/BehaviourTree/BehaviourTree.h"
#include "Engine/Transform2D.h"
#include "Engine/Implementation/BehaviourTree/BehaviourTree.h"
#include "Engine/Implementation/GameObjects/Sprite.h"
#include <vector>

class WardenBT : public BehaviourTree
{
public:
	enum Direction
	{
		NORTH,
		EAST,
		SOUTH,
		WEST
	};

	enum IndicatorState
	{
		SEEPLAYER,
		CANNOTSEEPLAYER
	};

	struct Blackboard
	{
		float PositionX;
		float WaitTime;
		float DeltaTime;
		float Speed;

		bool CanSeePlayer;
		bool IsPlayerLockpicking;

		bool Caught;
	};

	// Warden-specific task nodes for behaviour tree
	class WaitNode : public Node
	{
	public:
		WaitNode(WardenBT* ownerBT, float acceptableDifference);
		NodeTask Evaluate() override;

	private:
		WardenBT* OwnerBT;
		float AcceptableDifference;
	};

	class RandomWaypointNode : public Node
	{
	public:
		RandomWaypointNode(WardenBT* ownerBT);
		NodeTask Evaluate() override;
		void AddWaypoint(float wp);

	private:
		WardenBT* OwnerBT;
		std::vector<float> Waypoints;
	};

	class RandomFloatNode : public Node
	{
	public:
		RandomFloatNode(WardenBT* ownerBT, int Min, int Max);
		NodeTask Evaluate() override;

	private:
		WardenBT* OwnerBT;
		int MinBound;
		int MaxBound;
	};

	class MoveToPositionXNode : public Node
	{
	public:
		MoveToPositionXNode(WardenBT* ownerBT, float MinDifference);
		NodeTask Evaluate() override;

	private:
		WardenBT* OwnerBT;
		float AcceptableDifference;
		float Speed;
	};

	// Change sprite state to face either up or down
	class FaceRandomDirectionNode : public Node
	{
	public:
		FaceRandomDirectionNode(WardenBT* ownerBT);
		NodeTask Evaluate() override;

	private:
		WardenBT* OwnerBT;
	};

	class UpdateCanSeePlayerService : public Node
	{
	public:
		UpdateCanSeePlayerService(WardenBT* ownerBT);
		NodeTask Evaluate() override;

	private:
		WardenBT* OwnerBT;
	};

	class CanSeePlayerNode : public Node
	{
	public:
		CanSeePlayerNode(WardenBT* ownerBT);
		NodeTask Evaluate() override;

	private:
		WardenBT* OwnerBT;
	};

	class IsPlayerLockpickingNode : public Node
	{
	public:
		IsPlayerLockpickingNode(WardenBT* ownerBT);
		NodeTask Evaluate() override;

	private:
		WardenBT* OwnerBT;
	};

	class SetGameOverNode : public Node
	{
	public:
		SetGameOverNode(WardenBT* ownerBT);
		NodeTask Evaluate() override;

	private:
		WardenBT* OwnerBT;
	};

	WardenBT(Sprite* wardenSprite);

	Blackboard* WardenBB;
	Sprite* WardenSprite;

private:
	SelectorNode* RootSelector;
	UpdateCanSeePlayerService* UpdateCanSeePlayerRootService;

	SequenceNode* DetectPlayerSequence;
	CanSeePlayerNode* DetectCanSeePlayer;
	IsPlayerLockpickingNode* DetectIsPlayerLockpicking;

	SequenceNode* PatrolSequence;
	RandomWaypointNode* PatrolRandomWayPoint;
	MoveToPositionXNode* PatrolMoveToX;
	FaceRandomDirectionNode* PatrolFaceRandomDirection;
	WaitNode* PatrolWait;

};

