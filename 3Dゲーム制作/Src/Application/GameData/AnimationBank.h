#pragma once

class AnimationBank
{
public:

	static AnimationBank& Instance();

	// 論理名からアニメーションデータを取得（未登録名は nullptr）
	const std::shared_ptr<KdAnimationData>Get(std::string_view name)const;

private:

	AnimationBank()		= default;
	~AnimationBank()	= default;

	// クリップの所在
	struct ClipSource
	{
		std::string m_filePath;	// gltf ファイルパス
		std::string m_clipName;	// ファイル内のアニメーション名
	};

	static const std::unordered_map<std::string, ClipSource> kClipTable;
};