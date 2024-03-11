#include "WardenBT.h"
#include "random"
#include "cmath"
#include "Engine/Implementation/BehaviourTree/BehaviourTree.h"
#include "iostream"
#include "Engine/Implementation/BehaviourTree/BehaviourTree.h"
#include <random>

// Generate a random float between a max and min value
// and save it to PositionX on the blackboard
WardenBT::RandomFloatNode::RandomFloatNode(WardenBT* ownerBT, int Min, int Max) : OwnerBT(ownerBT), MinBound(Min), MaxBound(Max)
{
}

BehaviourTree::NodeTask WardenBT::RandomFloatNode::Evaluate()
{
    std::cout << "RandomFloat" << std::endl;
    OwnerBT->WardenBB->PositionX = static_cast<float>(rand() % (MaxBound - MinBound + 1) + MinBound);
    co_return NodeState::SUCCESS;
}

// Moves sprite to the PositionX on the blackboard
WardenBT::MoveToPositionXNode::MoveToPositionXNode(WardenBT* ownerBT, float MinDifference) : OwnerBT(ownerBT), AcceptableDifference(MinDifference)
{
}

BehaviourTree::NodeTask WardenBT::MoveToPositionXNode::Evaluate()
{
    //std::cout << "MoveToX" << std::endl;
    // When walking, the player is not seen
    OwnerBT->WardenBB->CanSeePlayer = false;
    OwnerBT->WardenSprite->SetIndicatorState(CANNOTSEEPLAYER);

    float spriteXPosition = OwnerBT->WardenSprite->GetTransform().PositionX;
    float randomXPosition = OwnerBT->WardenBB->PositionX;

    bool isAtLocation = std::abs(randomXPosition - spriteXPosition) < AcceptableDifference;

    // Coroutines allow functions that require multiple frames to unsuspend
    // and continue where they left off in the behaviour tree
    while(!isAtLocation)
    {
        float direction;
        float speed = OwnerBT->WardenBB->Speed;

        if (randomXPosition < spriteXPosition)
        {
            direction = -1;
            OwnerBT->WardenSprite->SetSpriteState(WEST);
        }
        else
        {
            direction = 1;
            OwnerBT->WardenSprite->SetSpriteState(EAST);
        }

        OwnerBT->WardenSprite->SetPosition(OwnerBT->WardenSprite->GetTransform().PositionX + (direction * speed * OwnerBT->WardenBB->DeltaTime), OwnerBT->WardenSprite->GetTransform().PositionY);
        spriteXPosition = OwnerBT->WardenSprite->GetTransform().PositionX;
        isAtLocation = std::abs(randomXPosition - spriteXPosition) < AcceptableDifference;
        co_yield NodeState::RUNNING;

    }

    co_return NodeState::SUCCESS;
}

// Pick a random waypoint out of the list and set it to the blackboard
WardenBT::RandomWaypointNode::RandomWaypointNode(WardenBT* ownerBT) : OwnerBT(ownerBT), Waypoints()
{
}

BehaviourTree::NodeTask WardenBT::RandomWaypointNode::Evaluate()
{
    int numWaypoints = Waypoints.size();
    int randInt = rand() % numWaypoints;

    OwnerBT->WardenBB->PositionX = Waypoints[randInt];

    co_return NodeState::SUCCESS;
}

void WardenBT::RandomWaypointNode::AddWaypoint(float wp)
{
    Waypoints.push_back(wp);
}

// Wait for a selected amount of time
WardenBT::WaitNode::WaitNode(WardenBT* ownerBT, float acceptableDifference) : OwnerBT(ownerBT), AcceptableDifference(acceptableDifference)
{
}

BehaviourTree::NodeTask WardenBT::WaitNode::Evaluate()
{
    float timer = 0;
    float waitTime = OwnerBT->WardenBB->WaitTime;
    bool timerDone = std::abs(waitTime - timer) < AcceptableDifference;

    while(!timerDone)
    {
        timer += OwnerBT->WardenBB->DeltaTime;
        timerDone = std::abs(waitTime - timer) < AcceptableDifference;
        co_yield NodeState::RUNNING;

        // Break out of loop if player lockpicking
        bool canSeePlayer = OwnerBT->WardenBB->CanSeePlayer;
        bool isPlayerLockpicking = OwnerBT->WardenBB->IsPlayerLockpicking;

        if(canSeePlayer && isPlayerLockpicking)
        {
            co_return NodeState::FAILURE;
        }
    }

    co_return NodeState::SUCCESS;
}

WardenBT::FaceRandomDirectionNode::FaceRandomDirectionNode(WardenBT* ownerBT) : OwnerBT(ownerBT)
{
}

BehaviourTree::NodeTask WardenBT::FaceRandomDirectionNode::Evaluate()
{
    int randInt = rand() % 2;

    if(randInt == 0)
    {
        OwnerBT->WardenSprite->SetSpriteState(NORTH);
    }
    else
    {
        OwnerBT->WardenSprite->SetSpriteState(SOUTH);
    }

    co_return NodeState::SUCCESS;
}

WardenBT::UpdateCanSeePlayerService::UpdateCanSeePlayerService(WardenBT* ownerBT) : OwnerBT(ownerBT)
{
}

BehaviourTree::NodeTask WardenBT::UpdateCanSeePlayerService::Evaluate()
{
    float spritePosition = OwnerBT->WardenSprite->GetTransform().PositionX;

    bool isPlayerSeen = std::abs(spritePosition) < 20;
    bool isFacingPlayer = (OwnerBT->WardenSprite->GetSpriteState() == SOUTH);

    if(isPlayerSeen && isFacingPlayer)
    {
        OwnerBT->WardenBB->CanSeePlayer = true;
        OwnerBT->WardenSprite->SetIndicatorState(SEEPLAYER);
        //co_return NodeState::SUCCESS;
    }
    else
    {
        OwnerBT->WardenBB->CanSeePlayer = false;
        OwnerBT->WardenSprite->SetIndicatorState(CANNOTSEEPLAYER);
        //co_return NodeState::FAILURE;
    }
    co_return NodeState::SUCCESS;

}

WardenBT::CanSeePlayerNode::CanSeePlayerNode(WardenBT* ownerBT) :OwnerBT(ownerBT)
{
}

BehaviourTree::NodeTask WardenBT::CanSeePlayerNode::Evaluate()
{
    bool canSeePlayer = OwnerBT->WardenBB->CanSeePlayer;

    if(canSeePlayer)
    {
        co_return NodeState::SUCCESS;
    }

    co_return NodeState::FAILURE;
}

WardenBT::IsPlayerLockpickingNode::IsPlayerLockpickingNode(WardenBT* ownerBT) : OwnerBT(ownerBT)
{
}

BehaviourTree::NodeTask WardenBT::IsPlayerLockpickingNode::Evaluate()
{
    bool isPlayerLockpicking = OwnerBT->WardenBB->IsPlayerLockpicking;

    if (isPlayerLockpicking)
    {
        std::cout << "Caught" << std::endl;
        OwnerBT->WardenBB->Caught = true;
        co_return NodeState::SUCCESS;
    }
    OwnerBT->WardenBB->Caught = false;
    co_return NodeState::FAILURE;

}

BehaviourTree::NodeTask WardenBT::SetGameOverNode::Evaluate()
{
    OwnerBT->WardenBB->Caught = true;
    co_return NodeState::SUCCESS;
}

// Setup behaviour tree structure upon init
WardenBT::WardenBT(Sprite* wardenSprite) : BehaviourTree(), WardenSprite(wardenSprite), WardenBB(new Blackboard)
{
    WardenBB->IsPlayerLockpicking = false;
    WardenBB->Caught = false;
    WardenBB->DeltaTime = 0.016f;
    WardenBB->Speed = 500.0f;
    WardenBB->WaitTime = 4.0f;
    
    // Initialise nodes
    RootSelector = new SelectorNode;
    UpdateCanSeePlayerRootService = new UpdateCanSeePlayerService(this);

    DetectPlayerSequence = new SequenceNode;
    DetectCanSeePlayer = new CanSeePlayerNode(this);
    DetectIsPlayerLockpicking = new IsPlayerLockpickingNode(this);

    PatrolSequence = new SequenceNode;
    PatrolRandomWayPoint = new RandomWaypointNode(this);
    PatrolRandomWayPoint->AddWaypoint(-500);
    PatrolRandomWayPoint->AddWaypoint(0);
    PatrolRandomWayPoint->AddWaypoint(500);
    PatrolMoveToX = new MoveToPositionXNode(this, 10);
    PatrolFaceRandomDirection = new FaceRandomDirectionNode(this);
    PatrolWait = new WaitNode(this, 1);

    // Set up behaviour tree
    RootSelector->AddChild(DetectPlayerSequence);
    RootSelector->AddChild(PatrolSequence);

    DetectPlayerSequence->AddChild(DetectCanSeePlayer);
    DetectPlayerSequence->AddChild(DetectIsPlayerLockpicking);

    PatrolSequence->AddChild(PatrolRandomWayPoint);
    PatrolSequence->AddChild(PatrolMoveToX);
    PatrolSequence->AddChild(PatrolFaceRandomDirection);
    PatrolSequence->AddChild(PatrolWait);

    SetRootChild(RootSelector);
    AddRootService(UpdateCanSeePlayerRootService);
}
