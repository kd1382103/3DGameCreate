
#pragma once

// State の基底クラス
template<class TOwner>
class StateBase
{
public:

	//派生クラスのデストラクタから基底クラスのデストラクタまで、
	//正しく呼び出すためにvirtualをつける
	virtual ~StateBase() = default;

	virtual void Enter(TOwner&) {};
	virtual void Update(TOwner&) = 0;
	virtual void Exit(TOwner&) {};
};

//========================================
// StateMachine 共通値
//========================================
class StateMachineParameter
{
public:
	static constexpr float KnockBackPower = 0.5f;
	//constexpr指定子：コンパイル時に値を確定できる定数や関数を指定するためのキーワード指定子

	//const指定子も定数を表すキーワードでどちらも値を変更はできない

	//constexprの場合はコンパイル時に値を確定することも要求し、
	//ヘッダー内だけで済むためこちらを採用
};

template<class TOwner>
class StateMachine
{
public:

	// 即時遷移（割り込み）
	void ChangeStateImmediate(std::unique_ptr<StateBase<TOwner>> nextState, TOwner& owner)
	{
		ApplyStateChange(std::move(nextState), owner);
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
			ApplyStateChange(std::move(m_nextState), owner);
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
		//現在のステートが指定したクラスなのかを安全に判定するためにdynamic_castを使用
	}

	// 次のステートが予約されているか？
	bool HasNextState() const
	{
		return m_nextState != nullptr;
	}

private:

	// ステート切り替え処理
	void ApplyStateChange(std::unique_ptr<StateBase<TOwner>> nextState, TOwner& owner)
	{
		if (m_currentState)
		{
			m_currentState->Exit(owner);
		}

		m_currentState = std::move(nextState);
		m_currentState->Enter(owner);
	}

private:
	std::unique_ptr<StateBase<TOwner>> m_currentState = nullptr;	//現在のステートを所有
	std::unique_ptr<StateBase<TOwner>> m_nextState = nullptr;		//次に切り替えるステートを所有

	//整数の０と混合せず、ポインタの値であることを明示するためにnullptrを使用
	//ステートの生成・切り替え・破棄を安全に管理するためにunique_ptrを使用

};
