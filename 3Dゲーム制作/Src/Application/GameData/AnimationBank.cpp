#include "AnimationBank.h"

// 論理名→(ファイル, クリップ)の対応表
// クリップを差し替えたい時はここの1行を書き換えるだけ
const std::unordered_map<std::string, AnimationBank::ClipSource> AnimationBank::kClipTable =
{
	{ "Idle",				{ "Asset/Animations/Idle/Idle.gltf",		"IdleA" } },
	{ "Walk",				{ "Asset/Animations/Walk/Walk.gltf",		"WalkingC" } },
	{ "Dash",				{ "Asset/Animations/Run/Run.gltf",			"RunningA" } },
};


AnimationBank& AnimationBank::Instance()
{
	static AnimationBank instance;
	return instance;
}

const std::shared_ptr<KdAnimationData> AnimationBank::Get(std::string_view name) const
{
	auto it = kClipTable.find(std::string(name));
	if (it == kClipTable.end())
	{
		return nullptr;
	}

	// ファイル読み込み(2回目以降はKdAssetsのキャッシュが返る)
	const std::shared_ptr<KdModelData> animFile = KdAssets::Instance().m_modeldatas.GetData(it->second.m_filePath);
	if (!animFile) { return nullptr; }

	return animFile->GetAnimation(it->second.m_clipName);
}
