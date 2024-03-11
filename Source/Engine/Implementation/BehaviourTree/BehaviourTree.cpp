#include "BehaviourTree.h"

// Default behaviour for empty node
// Intended to be overriden, for the user to implement their own behaviours
BehaviourTree::NodeTask BehaviourTree::Node::Evaluate()
{
	co_return NodeState::FAILURE;
}

// Service always return SUCCESS and allows the BT to track states during RUNNING
void BehaviourTree::Node::AddService(Node* Service)
{
	Services.emplace_back(Service);
}

// Iterates through services, running them one by one
void BehaviourTree::Node::RunServices()
{
	if(GetServices().empty())
	{
		return;
	}

	for (Node* Service : GetServices())
	{
		NodeTask Task = Service->Evaluate();
		NodeState Result = Task.Run();
	}
}

const std::list<BehaviourTree::Node*>& BehaviourTree::Node::GetServices() const
{
	return Services;
}


// Return children of composite node
const std::list<BehaviourTree::Node*>& BehaviourTree::CompositeNode::GetChildren() const
{
	return Children;
}

// Add child to back of the list
void BehaviourTree::CompositeNode::AddChild(Node* Child)
{
	Children.emplace_back(Child);
}

// Selector node
BehaviourTree::NodeTask BehaviourTree::SelectorNode::Evaluate()
{
	for(Node* Child : GetChildren())
	{
		NodeTask Task = Child->Evaluate();
		NodeState Result = Task.Run();

		switch (Result)
		{
			case NodeState::FAILURE:
				continue;

			case NodeState::SUCCESS:
				co_return NodeState::SUCCESS;

			case NodeState::RUNNING:
				//co_yield NodeState(RUNNING);
				// Here it needs to know to rerun
				while (Result == NodeState::RUNNING)
				{
					co_yield NodeState::RUNNING;
					Result = Task.Run();
				}

		}
	}

	co_return NodeState::FAILURE;
}

// Sequence node
BehaviourTree::NodeTask BehaviourTree::SequenceNode::Evaluate()
{

	for (Node* Child : GetChildren())
	{
		NodeTask Task = Child->Evaluate();
		NodeState Result = Task.Run();

		switch (Result)
		{
			case NodeState::SUCCESS:
				continue;

			case NodeState::FAILURE:
				co_return NodeState::FAILURE;

			case NodeState::RUNNING:
				//co_yield NodeState(RUNNING);
				// Here it needs to know to rerun
				while(Result == NodeState::RUNNING)
				{
					co_yield NodeState::RUNNING;
					Result = Task.Run();
				}
		}
	}

	co_return NodeState::SUCCESS;
}

void BehaviourTree::RootNode::SetChild(Node* NewChild)
{
	Child = NewChild;
}

// Evaluate root node
BehaviourTree::NodeTask BehaviourTree::RootNode::Evaluate()
{
	NodeTask ChildTask = Child->Evaluate();

	// Run services here
	RunServices();
	NodeState Result = ChildTask.Run();

	// If initial state is RUNNING
	while (Result == NodeState::RUNNING)
	{
		// Return result and suspend the tree until the next tick where it will run again
		co_yield Result;

		// Also run services here
		RunServices();
		Result = ChildTask.Run();
	}
	co_return Result;
}

BehaviourTree::BehaviourTree() : Root(new RootNode)
{
}

// Set root node
void BehaviourTree::SetRootChild(Node* RootChild) const
{
	Root->SetChild(RootChild);
}

void BehaviourTree::AddRootService(Node* Service)
{
	Root->AddService(Service);
}

// Evaluate entire tree
BehaviourTree::NodeTask BehaviourTree::Evaluate() const
{
	// This loop allows the behaviour tree to refresh -- as coroutine handlers themselves cannot do so
	// Setup and run new coroutine
	while(true)
	{
		NodeTask RootTask = Root->Evaluate();

		NodeState Result = RootTask.Run();

		// If initial state is RUNNING
		while (Result == NodeState::RUNNING)
		{
			// Return result and suspend the tree until the next tick where it will run again
			co_yield Result;
			Result = RootTask.Run();
		}
		//co_return Result;
		co_yield Result;
	}
}
