#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <stdio.h>
#include "math.h"

#define WINDOW_SIZE 600

static SDL_Window *window;
static SDL_GLContext context;
static float deltaTime;
#define STR(x) #x

enum {
	SHADER_TEXTURELESS_3D = 1,
	NUM_SHADERS,
};

#define SHADERS_POSITION_ATTRIB "pos"

enum {
	POS_LOC = 0,
};

static struct Shader {
	GLuint program;
	GLuint fShader;
	GLuint vShader;
	GLuint modelLoc;
	GLuint projLoc;
	GLuint invViewportLoc;
	GLuint viewLoc;
	GLuint invViewLoc;
} shaders[NUM_SHADERS];

static void CreateShader(struct Shader *shader, const char *vSource, const char *fSource){
	shader->program         = glCreateProgram();
	shader->fShader         = glCreateShader(GL_FRAGMENT_SHADER);
	shader->vShader         = glCreateShader(GL_VERTEX_SHADER);
	const GLchar*   glSrc   = fSource;
	GLint           status;
	char            buffer[512];

	glShaderSource(shader->fShader,1,&glSrc,NULL);
	glCompileShader(shader->fShader);
	glGetShaderiv(shader->fShader, GL_COMPILE_STATUS, &status);
	if(status != GL_TRUE){
		glGetShaderInfoLog(shader->fShader,512,NULL,buffer);
		printf("FSHADER: %s\n", buffer);
		return;
	}

	glAttachShader(shader->program, shader->fShader);

	glSrc  = vSource;
	glShaderSource(shader->vShader,1,&glSrc,NULL);
	glCompileShader(shader->vShader);
	glGetShaderiv(shader->vShader, GL_COMPILE_STATUS, &status);
	if(status != GL_TRUE){
		glGetShaderInfoLog(shader->vShader,512,NULL,buffer);
		printf("VSHADER: %s\n", buffer);
		return;
	}

	glAttachShader(shader->program, shader->vShader);

	glLinkProgram(shader->program);
	glUseProgram(shader->program);

	glBindAttribLocation(shader->program, POS_LOC, SHADERS_POSITION_ATTRIB);

	shader->modelLoc = glGetUniformLocation(shader->program, "model");
	shader->projLoc = glGetUniformLocation(shader->program, "proj");
	shader->viewLoc = glGetUniformLocation(shader->program, "view");
	shader->invViewportLoc = glGetUniformLocation(shader->program, "invViewport");
}


static const char *texturelessVSource = "#version 120\n"
STR(
attribute vec3 pos;
uniform mat4 model = mat4(1);
uniform mat4 view;
uniform mat4 proj;
uniform mat3 invTrans = mat3(1);
void main(){
	 gl_Position = proj * view * vec4(pos,1);
});

static const char *texturelessFSource = "#version 120\n"
STR(
uniform vec4 uniColor = vec4(1,1,1,1);
void main(){
    gl_FragColor = uniColor;    
});

static struct Shader textureless3DShader;
static unsigned int vao, posVbo;

static void Update(){
	
	
}
void DrawLines(Vec3 *lines, int num){
	
	glBindVertexArray(vao);
	glLineWidth(4);
	glBindBuffer(GL_ARRAY_BUFFER, posVbo);
	glBufferData(GL_ARRAY_BUFFER, num*sizeof(Vec3), &lines[0], GL_STATIC_DRAW);
	glDrawArrays(GL_LINE_STRIP, 0, num);
	glBindVertexArray(0);
}

void DrawCube(Cube r){

	Vec3 points[8];

	points[0] = (Vec3){r.x, r.y, r.z};
	points[1] = (Vec3){r.x+r.w, r.y, r.z};
	points[2] = (Vec3){r.x+r.w, r.y+r.h, r.z};
	points[3] = (Vec3){r.x, r.y+r.h, r.z};
	points[4] = (Vec3){r.x, r.y, r.z+r.d};
	points[5] = (Vec3){r.x+r.w, r.y, r.z+r.d};
	points[6] = (Vec3){r.x+r.w, r.y+r.h, r.z+r.d};
	points[7] = (Vec3){r.x, r.y+r.h, r.z+r.d};

	Vec3 lines[18];	

	lines[0] = (Vec3){points[0].x, points[0].y, points[0].z};
	lines[1] = (Vec3){points[1].x, points[1].y, points[1].z};
	lines[2] = (Vec3){points[2].x, points[2].y, points[2].z};
	lines[3] = (Vec3){points[3].x, points[3].y, points[3].z};
	lines[4] = (Vec3){points[0].x, points[0].y, points[0].z};
	lines[5] = (Vec3){points[4].x, points[4].y, points[4].z};
	lines[6] = (Vec3){points[5].x, points[5].y, points[5].z};
	lines[7] = (Vec3){points[6].x, points[6].y, points[6].z};
	lines[8] = (Vec3){points[7].x, points[7].y, points[7].z};
	lines[9] = (Vec3){points[4].x, points[4].y, points[4].z};
	lines[10] = (Vec3){points[7].x, points[7].y, points[7].z};
	lines[11] = (Vec3){points[3].x, points[3].y, points[3].z};
	lines[12] = (Vec3){points[7].x, points[7].y, points[7].z};
	lines[13] = (Vec3){points[6].x, points[6].y, points[6].z};
	lines[14] = (Vec3){points[2].x, points[2].y, points[2].z};
	lines[15] = (Vec3){points[6].x, points[6].y, points[6].z};
	lines[16] = (Vec3){points[5].x, points[5].y, points[5].z};
	lines[17] = (Vec3){points[1].x, points[1].y, points[1].z};

	DrawLines(lines, 18);
}

static void Draw(){

	glUseProgram(textureless3DShader.program);
	
	
	float proj[16], view[16];
		
	Math_Perspective(proj, 60.0f*(3.1415/180), 1, 0.1f, 50.0f);
	glUniformMatrix4fv(textureless3DShader.projLoc, 1, GL_TRUE, proj);
	
	Math_LookAt(view, (Vec3){0,0,3}, (Vec3){0,0,0}, (Vec3){0,1,0});
	glUniformMatrix4fv(textureless3DShader.viewLoc, 1, GL_TRUE, view);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0,0,WINDOW_SIZE, WINDOW_SIZE);
	DrawCube((Cube){-0.4,-0.4,-0.4,1,1,1});
	
}



int main(int argc, char **argv){

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	window = SDL_CreateWindow(
		"ragdoll",
		WINDOW_SIZE,
		WINDOW_SIZE,
		SDL_WINDOW_OPENGL
	);
	
	context = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(0);

	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK) {
		printf("Glew Init Failed\n");
		return 0;
	}

	SDL_ShowCursor();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	glClearColor(0,0,0,1);

	CreateShader(&textureless3DShader, texturelessVSource, texturelessFSource);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &posVbo);
	glBindBuffer(GL_ARRAY_BUFFER,posVbo);
	glEnableVertexAttribArray(POS_LOC);
	glVertexAttribPointer(POS_LOC, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);

	SDL_Event event;

	int breakLoop = 1;
	int lastTime = SDL_GetTicks();
	
    while(breakLoop){

        while(SDL_PollEvent(&event)){

			if(event.type == SDL_EVENT_QUIT){
				breakLoop = 0;
				break;
			}

	     }

		if(!breakLoop) break;

		int currTime = SDL_GetTicks();
		deltaTime = currTime - lastTime;

		if(deltaTime > (1.0/10) * 1000){
			lastTime = currTime;
			continue;
		} 

		if(deltaTime > 0){
			lastTime = currTime;
			Update();
			Draw();
			SDL_GL_SwapWindow(window);
		}
	}


    glDeleteProgram(textureless3DShader.program);
    glDeleteShader(textureless3DShader.fShader);
    glDeleteShader(textureless3DShader.vShader);

	SDL_GL_DestroyContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}