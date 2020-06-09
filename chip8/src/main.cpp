#include <iostream>
#include <atomic>
#include <thread>
#include <gl/gl3w.h>
#include <GLFW/glfw3.h>

#include "hardware/Chip8.h"

#include <chrono>

/*
void ReadCh(std::atomic<bool>& run)
{
	std::string buffer;
	int input;

	while (run.load())
	{
		input = _getch();

		if (input == '0')
		{
			run.store(false);
		}
	}
}
*/

GLuint          program;
GLuint          vao;
GLint           mv_location;
GLint           proj_ortho;
GLint 			position_location;

Chip8 chip8;


void startup()
{

	static const char * vs_source[] =
			{
					"#version 420 core                             \n"
					"                                              \n"
					"in vec2 position;\n"
					"uniform mat4 proj_ortho;                                            \n"
					"void main(void)                               \n"
					"{                                             \n"
					"    gl_Position = proj_ortho * vec4(position.xy, 0.0, 1.0);   \n"
					"}                                             \n"
			};

	static const char * fs_source[] =
			{
					"#version 420 core                             \n"
					"                                              \n"
					"out vec4 color;                               \n"
					"                                              \n"
					"void main(void)                               \n"
					"{                                             \n"
					"    color = vec4(0.0, 0.8, 1.0, 1.0);         \n"
					"}                                             \n"
			};

	program = glCreateProgram();
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, fs_source, NULL);
	glCompileShader(fs);

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, vs_source, NULL);
	glCompileShader(vs);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);

	proj_ortho = glGetUniformLocation(program, "proj_ortho");
	position_location = glGetAttribLocation(program, "position");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);


	glEnableVertexAttribArray(position_location);
	glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (GLvoid*)0); // offset set.
}

constexpr float HEIGHT = 320.f;
constexpr float WIDTH  = 640.f;


void draw( float x, float y)
{
	const float ortho_projection[4][4] =
			{
					{ 2.0f/ WIDTH, 0.0f, 0.0f, 0.0f },
					{ 0.0f, 2.0f / -HEIGHT, 0.0f, 0.0f },
					{ 0.0f, 0.0f, -1.0f, 0.0f },
					{-1.0f,                  1.0f,                   0.0f, 1.0f },
			};

	glUseProgram(program);
	glPointSize(10.0f);
	glUniformMatrix4fv(proj_ortho, 1, GL_FALSE, &ortho_projection[0][0]);
	glVertexAttrib2f(position_location, x, y);
	glDrawArrays(GL_POINTS, 0, 1);
}

void render( Chip8 & chip8 )
{

	for( int y = 0; y < 32; y++ )
	{
		for (int x = 0; x < 64; x++)
		{
			if( chip8.getScreenData( x, y ) != 0 )
			{
				draw( x * 10, y * 10 );
			}
		}
	}
}


struct KeyBind
{
	int key; int bind;
};

static KeyBind keyMap[] = {
		{ GLFW_KEY_1, 0x0 },
		{ GLFW_KEY_2, 0x1 },
		{ GLFW_KEY_3, 0x2 },
		{ GLFW_KEY_4, 0x3 },
		{ GLFW_KEY_Q, 0x4 },
		{ GLFW_KEY_W, 0x5 },
		{ GLFW_KEY_E, 0x6 },
		{ GLFW_KEY_R, 0x7 },
		{ GLFW_KEY_A, 0x8 },
		{ GLFW_KEY_S, 0x9 },
		{ GLFW_KEY_D, 0xA },
		{ GLFW_KEY_F, 0xB },
		{ GLFW_KEY_Z, 0xC },
		{ GLFW_KEY_X, 0xD },
		{ GLFW_KEY_C, 0xE },
		{ GLFW_KEY_V, 0xF },
};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	BYTE bind = 0xff;
	for ( auto & key_bind : keyMap )
	{
		if( key == key_bind.key )
		{
			bind = key_bind.bind;
			break;
		}
	}

	if(bind == 0xff)
	{
		return;
	}

	if( action == GLFW_PRESS )
	{

		chip8.addInput( bind );
		return;
	}


	if ( action == GLFW_RELEASE )
	{
		chip8.upInput( bind  );
		return;
	}


	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
	}

}


#ifdef  _CHIP8_DISASM_BUILD

int main() // 디스어셈블러 빌드
{
	chip8.reset();
	chip8.loadRom();

	while( !chip8.isEOF() )
	{
		chip8.nextStep();
	}

	chip8.createDisASMFile();
}


#endif


#ifndef _CHIP8_DISASM_BUILD

int main()
{
	float color[4] = {0.51f, 0.44f, 0.51f, 1.0f};




	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, false);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "CHIP - 8", nullptr, nullptr);
	glfwMakeContextCurrent(window);



	if (gl3wInit()) {
		fprintf(stderr, "failed to initialize OpenGL\n");
		return -1;
	}



	std::atomic<bool> run(true);


	chip8.reset();
	chip8.loadRom();


	startup();


	double lastTime = 0.0;
	const double maxFPS = 240.0;
	const double maxPeriod = 1.0 / maxFPS;

	glfwSetKeyCallback(window, key_callback);

	static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	while (!glfwWindowShouldClose(window))
	{
		double time = glfwGetTime();
		//glViewport(0, 0, 640, 320);
		//glClearColor(color[0], color[1], color[2], color[3]);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glfwPollEvents();

		glClearBufferfv(GL_COLOR, 0, black);

		render( chip8 );

		glfwSwapBuffers(window);

#ifdef __CHIP8_DEBUG
		if ( glfwGetKey ( window,  GLFW_KEY_SPACE ) )
		{
#endif
			chip8.nextStep();

#ifdef __CHIP8_DEBUG
		}
#endif
		while ( time - lastTime < maxPeriod )
		{
			time = glfwGetTime();
			glfwPollEvents();
		}

		lastTime = glfwGetTime();
	}

	return 0;
}





#endif //  _CHIP8_DISASM_BUILD