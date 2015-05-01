#include <vector>
#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include "Gizmos.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "NeuralNetwork.h"

using namespace std;
//not fleshed out yet.  The student can add the code to support this if and when they need to

class AStar
{
	AStar();
	~AStar();
	vector<glm::vec2> getPath();
};