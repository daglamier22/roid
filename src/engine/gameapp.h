#ifndef GAMEAPP_H
#define GAMEAPP_H

namespace genesis {

class GameApp
{
private:
	std::string		m_SaveGameDirectory;

public:
	GameApp();

	bool checkStorageMB( const unsigned long long diskSpaceNeeded );
	bool checkMemoryMB( const unsigned long long physicalRAMNeeded );
	bool checkCPUSpeed( const unsigned long cpuSpeedNeeded );
	const std::string getSaveGameDirectory( const std::string gameAppDirectory );
};

extern GameApp* g_pApp;

}

#endif // GAMEAPP_H
