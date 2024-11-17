#include <nds.h>
#include <stddef.h>
#include "inifile.h"
#include "tonccpy.h"
#include "nds_loader_arm9.h"

#ifdef __cplusplus
extern "C" {
#endif

using namespace std;

volatile char currentNDSFilePath[256];

volatile char iniNDSFilePath[261];
volatile char iniSAVFilePath[261];

const char* bootstrapPath = "/_nds/nds-bootstrap-release.nds";
const char* bootstrapINIPath = "/_nds/nds-bootstrap.ini";
const char* fatString = "fat:";

volatile int bootstrapPathLength = 0;
volatile int ndsPathLength = 0;
volatile int savPathLength = 0;

int runNDSFile (char tbuf[], char* iniPath, char* curPathName, char* ndsName, char* savName, bool isHomebrew) {
	bool usebootstrap = true;
	
	if (isHomebrew || (access(bootstrapPath, F_OK) != 0) || (access(bootstrapINIPath, F_OK) != 0))usebootstrap = false;
	
	sprintf(tbuf, "%s%s", curPathName, ndsName);
	ndsPathLength = (strlen((const char*)tbuf) + 1);
	if (ndsPathLength > 256)return 9;
	
	tonccpy((char*)currentNDSFilePath, (char*)tbuf, ndsPathLength);
	
	if (!usebootstrap || ((ndsPathLength + 4) > 261)) {
		const char *ndsARGArray1[] = { (char*)currentNDSFilePath };
		return runNdsFile(ndsARGArray1[0], 1, ndsARGArray1);
	}
	
	sprintf(tbuf, "%s%s", fatString, currentNDSFilePath);
	
	tonccpy((char*)iniNDSFilePath, (char*)tbuf, (ndsPathLength + 4));
	
	sprintf(tbuf, "%s%s%s%s", fatString, iniPath, "/", savName);
	savPathLength = (strlen((const char*)tbuf) + 1);
	
	if (savPathLength > 262) {
		const char *ndsARGArray2[] = { (char*)currentNDSFilePath };
		return runNdsFile(ndsARGArray2[0], 1, ndsARGArray2);
	}
		
	tonccpy((char*)iniSAVFilePath, (char*)tbuf, savPathLength);
		
	CIniFile bootstrapini(bootstrapINIPath);
	bootstrapini.SetString("NDS-BOOTSTRAP", "NDS_PATH", (char*)iniNDSFilePath);
	bootstrapini.SetString("NDS-BOOTSTRAP", "SAV_PATH", (char*)iniSAVFilePath);
	bootstrapini.SetString("NDS-BOOTSTRAP", "HOMEBREW_ARG", "");
	bootstrapini.SetString("NDS-BOOTSTRAP", "RAM_DRIVE_PATH", "");
	bootstrapini.SetInt("NDS-BOOTSTRAP", "DSI_MODE", 0);
	bootstrapini.SetInt("NDS-BOOTSTRAP", "BOOST_CPU", 0);
	bootstrapini.SetInt("NDS-BOOTSTRAP", "BOOST_VRAM", 0);
	bootstrapini.SaveIniFile(bootstrapINIPath);
	
	swiWaitForVBlank();
	
	const char* ndsARGArrayBootstrap[] = { bootstrapPath };
	return runNdsFile(bootstrapPath, 1, ndsARGArrayBootstrap);
}

#ifdef __cplusplus
}
#endif

