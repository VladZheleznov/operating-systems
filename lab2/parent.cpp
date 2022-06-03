#include <windows.h>
#include <iostream>
#include <string>
using namespace std;

void setSTP(STARTUPINFO* stp) {
	ZeroMemory(stp, sizeof(STARTUPINFO));
	stp->cb = sizeof(STARTUPINFO);
	//stp->dwX = 300;
	stp->dwXSize = 800;
}

int main() {
	int n;
	cin >> n;
	string st("Child.exe");
	for (int i = 0;i < n;++i) {
		double c;
		cin >> c;
		st.append(' ' + std::to_string(c));
	}
	wstring q(st.begin(),st.end());
	for (int i = 0;i < st.length();++i)
		q[i] = wchar_t(st[i]);

	STARTUPINFO stp;
	PROCESS_INFORMATION pi;
	setSTP(&stp);
	
	CreateProcess(NULL, &q[0], NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &stp, &pi);

	WaitForSingleObject(pi.hProcess,INFINITE);

	TerminateProcess(pi.hProcess, 0);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	ExitProcess(0);
}