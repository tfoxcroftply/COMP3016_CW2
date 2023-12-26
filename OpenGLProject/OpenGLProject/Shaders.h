#include <GL/glew.h>;
#include <fstream>;
#include <iostream>;

using namespace std;

/* string LoadFile(string requestName) {
	ifstream in(requestName, ios::binary);
	string content = "";
	if (in) {

	}
} */

class Shader {
public:
	GLuint ID;

	Shader(string vertFile, string fragFile) {};

	void Enable();
};

void Shader::Enable() {
	cout << "Enabling shader";
}