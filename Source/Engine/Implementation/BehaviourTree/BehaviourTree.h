#pragma once

#include <list>
#include <coroutine>

class BehaviourTree {
public:
	// Node return states upon completion
	enum NodeState
	{
		RUNNING, // Running is reserved for nodes which require multiple cycles to complete
		SUCCESS,
		FAILURE
	};

	// This is the wrapper type for the NodeTask coroutine
	struct NodeTask
	{
		// Promise type is used to control what happens within the coroutine
		struct promise_type
		{
			// This is the NodeState returned from a coroutine
			NodeState NodeResult;

			void unhandled_exception() {};
			NodeTask get_return_object() { return NodeTask(this); };

			// Initially suspend function run
			std::suspend_always initial_suspend() noexcept { return{}; }

			// Upon co_yield, suspend action
			// This should only be run when a RUNNING state is returned
			std::suspend_always yield_value(NodeState State) noexcept
			{
				NodeResult = State;
				return{};
			}

			// co_return is run when a definite state (SUCCESS or FAILURE) is reached
			void return_value(NodeState FinalState) noexcept { NodeResult = FinalState; }

			// Suspend before coroutine end, to prevent lifetime errors
			std::suspend_always final_suspend() noexcept { return{}; }
		};

		// These handle controlling the coroutine from the caller side
		using Handle = std::coroutine_handle<promise_type>;
		Handle mCoroHdl{};

		explicit NodeTask(promise_type* p) : mCoroHdl(Handle::from_promise(*p)){}
		NodeTask(NodeTask&& rhs) :mCoroHdl{std::exchange(rhs.mCoroHdl, nullptr)}{}

		~NodeTask()
		{
			if(mCoroHdl)
			{
				mCoroHdl.destroy();
			}
 		}

		// Operation to continue the node
		NodeState Run()
		{
			// Might check 
			if(mCoroHdl && !mCoroHdl.done())
			{
				mCoroHdl.resume();
			}

			return mCoroHdl.promise().NodeResult;

		}

		// Check coroutine finished
		bool Finished() const { return mCoroHdl.done(); }

	};

	// Base for all node types
	// Base node maintains a service to allow behaviour tree to continue to track changes in environment despite despite operating a RUNNING node
	class Node
	{
	public:
		virtual NodeTask Evaluate();
		void AddService(Node* Service);
		void RunServices();
		const std::list<Node*>& GetServices() const;
	private:
		std::list<Node*> Services;
	};

	// Composite nodes which are expected to have multiple children
	class CompositeNode : public Node
	{
	private:
		std::list<Node*> Children;

	public:
		const std::list<Node*>& GetChildren() const;
		void AddChild(Node* Child);
	};

	// Flow Control nodes
	// Selector returns true when the first child does
	class SelectorNode : public CompositeNode
	{
	public:
		virtual NodeTask Evaluate() override;
	};

	//Fails when any child fails
	class SequenceNode : public CompositeNode
	{
	public:
		virtual NodeTask Evaluate() override;
	};

	class RootNode : public Node
	{
	private:
		Node* Child;

	public:
		void SetChild(Node* NewChild);
		NodeTask Evaluate() override;
	};

private:
	RootNode* Root;

public:
	BehaviourTree();
	void SetRootChild(Node* RootChild) const;
	void AddRootService(Node* Service);
	NodeTask Evaluate() const;
};