#pragma once
#include "CmdExecutor.h"

namespace photon 
{

	class PaintItCaller
	{
	public:
		PaintItCaller(const std::wstring& _paintItDir, const std::wstring& _anacondaPython = L"E:/Env/envs/paint_it/python.exe");

		void FillKeyArgs(const std::wstring& modelPath,
			const std::wstring& posPrompt,
			const std::wstring& outputDir, const std::wstring& negPrompt = L"");

		CmdExecutor* ExecuteCommand();
		bool isOver();
		int GetProgress();

		std::wstring GetOutputDiffuseMapPath();
		std::wstring GetOutputSpecularMapPath();
		std::wstring GetOutputNormalMapPath();

		std::wstring Strip(const std::wstring rawString);


		static const std::wstring outputDiffuseName;
		static const std::wstring outputSpecularName;
		static const std::wstring outputNormalName;

		std::wstring paintItDir;
		std::wstring anacondaPython;
		std::wstring rawCmd;

	private:
		std::future<bool> m_bOver;
		std::wstring m_ModelPath;
		std::wstring m_PosPrompt;
		std::wstring m_NegPrompt;
		std::wstring m_OutputDir;
		CmdExecutor m_CmdExecutor;
	};
}
