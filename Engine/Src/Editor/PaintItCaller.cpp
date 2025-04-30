#include "PaintItCaller.h"


namespace photon 
{
	PaintItCaller::PaintItCaller(const std::wstring& _paintItDir, const std::wstring& _anacondaPython /*= L"E:/Env/envs/paint_it/python.exe"*/)
	{
		paintItDir = _paintItDir;
		anacondaPython = _anacondaPython;
		rawCmd = L"e: & cd " + _paintItDir + L" & " + _anacondaPython + L" -u " + _paintItDir + L"/ExecutePython.py" + L" --paint_it_dir=\"" + _paintItDir + L"\"";
	}

	void PaintItCaller::FillKeyArgs(const std::wstring& modelPath, const std::wstring& posPrompt, const std::wstring& outputDir, const std::wstring& negPrompt)
	{
		if (!modelPath.empty())
			m_ModelPath = L"--model_path=\"" + modelPath + L"\"";
		if (!posPrompt.empty())
			m_PosPrompt = L"--pos_prompt=\"" + posPrompt + L"\"";
		if (!outputDir.empty())
			m_OutputDir = L"--output_dir=\"" + outputDir + L"\"";
		if (!negPrompt.empty())
			m_NegPrompt = L"--neg_prompt=\"" + negPrompt + L"\"";
	}

	CmdExecutor* PaintItCaller::ExecuteCommand()
	{
		if (m_ModelPath.empty() || m_PosPrompt.empty() || m_OutputDir.empty())
			return nullptr;
		std::wstring finalCmd = rawCmd + L" " + m_ModelPath +
			L" " + m_PosPrompt +
			L" " + m_OutputDir +
			L" " + m_NegPrompt;
		m_bOver = m_CmdExecutor.Execute(finalCmd.c_str());
		return &m_CmdExecutor;
	}

	bool PaintItCaller::isOver()
	{
		if (m_bOver.valid())
		{
			return m_bOver.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
		}
		return true;
	}

	int PaintItCaller::GetProgress()
	{
		return m_CmdExecutor.GetProgress();
	}

	

	std::wstring PaintItCaller::GetOutputDiffuseMapPath()
	{
		return Strip(m_OutputDir) + L"/" + outputDiffuseName;
	}

	std::wstring PaintItCaller::GetOutputSpecularMapPath()
	{
		return Strip(m_OutputDir) + L"/" + outputSpecularName;
	}

	std::wstring PaintItCaller::GetOutputNormalMapPath()
	{
		return Strip(m_OutputDir) + L"/" + outputNormalName;
	}

	std::wstring PaintItCaller::Strip(const std::wstring rawString)
	{
		auto firstQuat = rawString.find_first_of(L'\"');
		auto lastQuat = rawString.find_last_of(L'\"');
		return rawString.substr(firstQuat + 1, lastQuat - firstQuat - 1);
	}

	const std::wstring PaintItCaller::outputDiffuseName = L"texture_kd.png";

	const std::wstring PaintItCaller::outputSpecularName = L"texture_ks.png";

	const std::wstring PaintItCaller::outputNormalName = L"texture_n.png";
}


