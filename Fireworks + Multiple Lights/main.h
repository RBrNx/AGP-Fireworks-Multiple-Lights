#if _DEBUG
#pragma comment(linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif

#include "rt3d.h"
#include "rt3dObjLoader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stack>
#include "particleArray.h"
#include <ctime> 
#include "Lights.h"

#include <SDL_ttf.h>

using namespace std;

#define DEG_TO_RADIAN 0.017453293