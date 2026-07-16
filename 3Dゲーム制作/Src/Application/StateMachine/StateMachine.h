#pragma once

// State の基底クラス
template<class TOwner>
class StateBase
{
public:
	virtual ~StateBase() = default;

	virtual void Enter(TOwner&) {};
	virtual void Update(TOwner&) = 0;
	virtual void Exit(TOwner&) {};
};

template<class TOwner>
class StateMachine
{
public:

	// 即時遷移（割り込み）
	void ChangeStateImmediate(std::unique_ptr<StateBase<TOwner>> nextState, TOwner& owner)
	{
		if (m_currentState)
		{
			m_currentState->Exit(owner);
		}
		m_currentState = std::move(nextState);
		m_currentState->Enter(owner);
	}

	// 予約遷移
	void ChangeState(std::unique_ptr<StateBase<TOwner>> nextState)
	{
		m_nextState = std::move(nextState);
	}

	// 現在のステート更新
	void Update(TOwner& owner)
	{
		if (m_nextState)
		{
			if (m_currentState)
			{
				m_currentState->Exit(owner);
			}

			m_currentState = std::move(m_nextState);
			m_currentState->Enter(owner);
		}

		if (m_currentState)
		{
			m_currentState->Update(owner);
		}
	}

	// 現在のステート取得
	StateBase<TOwner>* GetCurrentState() const
	{
		return m_currentState.get();
	}

	// 現在のステートが T 型か？
	template<class T>
	bool IsCurrent() const
	{
		return dynamic_cast<T*>(m_currentState.get()) != nullptr;
	}

	// 次のステートが予約されているか？
	bool HasNextState() const
	{
		return m_nextState != nullptr;
	}

private:
	std::unique_ptr<StateBase<TOwner>> m_currentState = nullptr;
	std::unique_ptr<StateBase<TOwner>> m_nextState = nullptr;
};
