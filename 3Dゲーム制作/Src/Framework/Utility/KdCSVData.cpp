#include "KdCSVData.h"

const std::vector<std::string> KdCSVData::c_nullDataList;

//============================================================
// CSV読み込み
//============================================================
bool KdCSVData::Load(const std::string_view filename)
{
	if (filename.empty()) { return false; }

	m_filePass = filename.data();

	// 再読み込み時に古いデータを消す
	m_dataLines.clear();

	std::ifstream ifs(m_filePass);

	if (!ifs)
	{ 
		assert(0 && "CSVDataが見つかりません");

		return false;
	}

	// 行ごとに分けてデータ格納
	while (1)
	{
		std::string rawLineData;
		if (!getline(ifs, rawLineData)) { break; }

		// [,]で分けて単語ごとにデータ格納
		std::vector<std::string> lineData;
		CommaSeparatedValue(rawLineData, lineData);

		m_dataLines.push_back(lineData);
	}

	return true;
}

//============================================================
// CSV保存
//============================================================
bool KdCSVData::Save() const
{
	if (m_filePass.empty())
	{
		return false;
	}

	std::ofstream ofs(m_filePass);

	if (!ofs)
	{
		return false;
	}

	for (size_t lineIndex = 0;
		lineIndex < m_dataLines.size();
		++lineIndex)
	{
		const auto& line = m_dataLines[lineIndex];

		for (size_t dataIndex = 0;
			dataIndex < line.size();
			++dataIndex)
		{
			ofs << line[dataIndex];

			// 最後の要素以外はカンマ
			if (dataIndex + 1 < line.size())
			{
				ofs << ",";
			}
		}

		// 最後の行以外は改行
		if (lineIndex + 1 < m_dataLines.size())
		{
			ofs << "\n";
		}
	}

	return true;
}

//============================================================
// CSVデータ変更
//============================================================
bool KdCSVData::SetData(
	size_t lineIndex,
	size_t dataIndex,
	const std::string& value)
{
	// 行が存在しない
	if (lineIndex >= m_dataLines.size())
	{
		return false;
	}

	// データが存在しない
	if (dataIndex >= m_dataLines[lineIndex].size())
	{
		return false;
	}

	m_dataLines[lineIndex][dataIndex] = value;

	return true;
}

// 行データを取得
const std::vector<std::string>& KdCSVData::GetLine(size_t index) const
{
	if (index >= m_dataLines.size()) { return c_nullDataList; }

	return m_dataLines[index];
}

// [,]で分けて単語リスト作成
void KdCSVData::CommaSeparatedValue(std::string_view line, std::vector<std::string>& result)
{
	std::istringstream stream(line.data());
	std::string element;

	while (getline(stream, element, ','))
	{
		result.push_back(element);
	}
}
