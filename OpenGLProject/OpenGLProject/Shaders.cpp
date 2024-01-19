//////////////////////////////////////////////////////////////////////////////
//
//  --- LoadShaders.cxx ---
//
//////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>

#include "GL/glew.h"
#include "Shaders.h"
#include "Log.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	//----------------------------------------------------------------------------

	static const GLchar*
		ReadShader(const char* filename)
	{

		FILE* infile;
		fopen_s(&infile, filename, "rb");


		if (!infile) {
#ifdef _DEBUG
			log("Unable to open shader file: '" + std::string(filename) + "'", LogType::Fatal);
#endif /* DEBUG */
			return NULL;
		}

		fseek(infile, 0, SEEK_END);
		int len = ftell(infile);
		fseek(infile, 0, SEEK_SET);

		GLchar* source = new GLchar[len + 1];

		fread(source, 1, len, infile);
		fclose(infile);

		source[len] = 0;

		return const_cast<const GLchar*>(source);
	}

	//----------------------------------------------------------------------------

	GLuint
		LoadShaders(ShaderInfo* shaders)
	{
		if (shaders == NULL) { return 0; }

		GLuint program = glCreateProgram();

		ShaderInfo* entry = shaders;
		while (entry->type != GL_NONE) {
			GLuint shader = glCreateShader(entry->type);

			entry->shader = shader;

			const GLchar* source = ReadShader(entry->filename);
			if (source == NULL) {
				for (entry = shaders; entry->type != GL_NONE; ++entry) {
					glDeleteShader(entry->shader);
					entry->shader = 0;
				}

				return 0;
			}

			glShaderSource(shader, 1, &source, NULL);
			delete[] source;

			glCompileShader(shader);

			GLint compiled;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
			if (!compiled) {
#ifdef _DEBUG
				GLsizei len;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

				GLchar* logstring = new GLchar[len + 1];
				glGetShaderInfoLog(shader, len, &len, logstring);
				log("Shader compilation failed: '" + std::string(logstring) + "'", LogType::Fatal);
				delete[] logstring;
#endif /* DEBUG */

				return 0;
			}

			log("Shader successfully loaded.");
			glAttachShader(program, shader);

			++entry;
		}

		glLinkProgram(program);

		GLint linked;
		glGetProgramiv(program, GL_LINK_STATUS, &linked);
		if (!linked) {
#ifdef _DEBUG
			GLsizei len;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

			GLchar* logstring = new GLchar[len + 1];
			glGetProgramInfoLog(program, len, &len, logstring);
			log("Shader linking failed: '" + std::string(logstring) + "'", LogType::Fatal);
			delete[] logstring;
#endif /* DEBUG */

			for (entry = shaders; entry->type != GL_NONE; ++entry) {
				glDeleteShader(entry->shader);
				entry->shader = 0;
			}

			return 0;
		}

		return program;
	}

	//----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif // __cplusplus