﻿#include <Windows.h>
#include "Driver.h"

#include <cstdio>

void Driver::SendCommand(Command* cmd)
{
	DWORD returned = 0;
	bool status = DeviceIoControl(DriverHandle, IOCTL_COMMAND, cmd, sizeof(Command), cmd, sizeof(Command), &returned, nullptr);

	if (!status)
	{
		printf("[BENCHMARK] GetLastError(): %x\n", GetLastError());
	}	
}

void Driver::CopyVirtual(bool read, uint64_t destination, uint64_t source, SIZE_T size)
{
	Command cmd;
	cmd.ForceOverwrite = Force;
	cmd.Source = read ? TargetProcessPid : CurrentProcessPid;
	cmd.Target = read ? CurrentProcessPid : TargetProcessPid;
	cmd.SourceAddress = source;
	cmd.TargetAddress = destination;
	cmd.Size = size;

	SendCommand(&cmd);

	if (Force)
		Force = false;
}

bool Driver::Init(int targetPid)
{
	DriverHandle = CreateFileA("\\\\.\\PhysicalDrive0", GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, NULL, nullptr);
	if (DriverHandle == INVALID_HANDLE_VALUE)
		return false;

	CurrentProcessPid = GetCurrentProcessId();
	TargetProcessPid = targetPid;
	Force = true;

	return true;
}
