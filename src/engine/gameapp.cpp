#include <sys/statvfs.h>
#include <pwd.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>

#include "gameapp.h"

namespace genesis {

const int MEGABYTE = 1024 * 1024;

GameApp* g_pApp = 0;

GameApp::GameApp()
{
	g_pApp = this;
}//GameApp::GameApp

//diskSpaceNeeded is in MB
bool GameApp::checkStorageMB( const unsigned long long diskSpaceNeeded ) {
	//Determine free space in the user's home directory
	struct statvfs stat;
	struct passwd* pw = getpwuid(getuid());
	unsigned long long freeBytes;
	if( pw != 0 && statvfs(pw->pw_dir, &stat) == 0 ) {
		freeBytes = (unsigned long long)stat.f_bavail * stat.f_frsize;
	}

	//calculate the number of Bytes needed based on MegaBytes needed
	unsigned long long neededBytes = diskSpaceNeeded * MEGABYTE;

	if( freeBytes < neededBytes ) {
		//if you get here you don't have enough disk space!
		return false;
	}
	return true;
}//GameApp::checkStorage

//physicalRAMNeeded is in MB
bool GameApp::checkMemoryMB( const unsigned long long physicalRAMNeeded ) {
	//Determine total system RAM
	unsigned long long pages = sysconf(_SC_PHYS_PAGES);
	unsigned long long page_size = sysconf(_SC_PAGESIZE);
	unsigned long long totalRAM = pages * page_size;

	//calculate the number of Bytes needed based on MedaBytes needed
	unsigned long long neededBytes = physicalRAMNeeded * MEGABYTE;

	if( totalRAM < neededBytes ) {
		//if you get here you don't have enough RAM!
		return false;
	}

	return true;
}//GameApp::checkMemoryMB

bool GameApp::checkCPUSpeed( const unsigned long cpuSpeedNeeded ) {
	//grab the max cpu frequency  from linux system file and read into string
	FILE* pipe = popen("cat /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", "r");
	if( !pipe )
		return false;
	char buffer[128];
	std::string result = "";
	while( !feof(pipe) ) {
		if( fgets(buffer, 128, pipe) != 0 )
			result += buffer;
	}
	pclose(pipe);

	//translate from string to a number
	unsigned long cpuspeed = std::stoul(result, nullptr, 0);

	if( cpuspeed < cpuSpeedNeeded ) {
		//if you get here the cpu is not fast enough!
		return false;
	}

	return true;
}//GameApp::checkCPUSpeed

const std::string GameApp::getSaveGameDirectory( const std::string gameAppDirectory ) {
	struct passwd* pw = getpwuid(getuid());
	if( pw != 0 ) {
		m_SaveGameDirectory += pw->pw_dir;
		m_SaveGameDirectory += "/";
		m_SaveGameDirectory += gameAppDirectory;
	}
	std::cout << m_SaveGameDirectory << std::endl;
	std::string cmd = "mkdir -p ";
	cmd += m_SaveGameDirectory;
	system(cmd.c_str());

	return m_SaveGameDirectory;
}//getSaveGameDirectory

} //namespace genesis
