#include <GUIKit/CommandLine.hpp>

#if SIV3D_PLATFORM(WINDOWS)
#include <Siv3D/Windows.hpp>
#endif

namespace s3d::CLI {
	bool Execute(const String& command) {
#if SIV3D_PLATFORM(WINDOWS)
		STARTUPINFO  si;
		PROCESS_INFORMATION pi;

		memset(&si, 0, sizeof(si));
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;

		std::wstring cmd = (U"powershell " + command).toWstr();
		const auto result = CreateProcess(NULL, cmd.data(), NULL, NULL, FALSE,
			CREATE_DEFAULT_ERROR_MODE | NORMAL_PRIORITY_CLASS,
			NULL, NULL, &si, &pi);

		const auto hWndmain = pi.hProcess;
		CloseHandle(pi.hThread);

		WaitForSingleObject(hWndmain, INFINITE);
		CloseHandle(hWndmain);

		return static_cast<bool>(result);

#elif SIV3D_PLATFORM(MACOS)
		return system(command.narrow().c_str()) == 0;
#endif

		return false;
	}

	bool OpenFolder(const String& path) {
		const auto folder = FileSystem::IsDirectory(path) ? path : FileSystem::ParentPath(path);

#if SIV3D_PLATFORM(WINDOWS)
		return Execute(U"Invoke-Item {}"_fmt(folder));
#elif SIV3D_PLATFORM(MACOS)
		return Execute(U"open {}"_fmt(folder));
#endif
	}

	bool Curl(const String& url, const FilePath& output) {
#if SIV3D_PLATFORM(WINDOWS)
		return Execute(U"curl {} -o {} -UseBasicParsing"_fmt(url, output));
#else
		return Execute(U"curl {} -o {}"_fmt(url, output));
#endif
	}
}
