#include<iostream>

#include<SDL3/SDL.h>
#include<geGL/geGL.h>
#include<geGL/StaticCalls.h>

#include<bunny.hpp>

using namespace ge::gl;

GLuint createShader(GLenum type,std::string const&src){
  auto s = glCreateShader(type);
  char const*sss[]={
    src.c_str()
  };
  glShaderSource(s,1,sss,0);
  glCompileShader(s);
  return s;
}

GLuint createProgram(std::vector<GLuint>ss){
  auto s = glCreateProgram();

  for(auto const&sh:ss)
    glAttachShader(s,sh);

  glLinkProgram(s);
  return s;
}

int main(int argc,char*argv[]){
  std::cerr << "PAPOUCH ŽERE OŘECHY." << std::endl;
  auto window = SDL_CreateWindow("IZG2026",1024,768,SDL_WINDOW_OPENGL);

  auto context = SDL_GL_CreateContext(window);

  ge::gl::init();

  auto vsSrc = R".(
  #version 460

  layout(location=0)in vec3 position;

  uniform float iTime;

  void main(){
    mat4 model = mat4(1);
    float a = radians(3000*iTime);
    model[0][0] =  cos(a);
    model[0][1] =  sin(a);
    model[1][0] = -sin(a);
    model[1][1] =  cos(a);


    gl_Position = model * vec4(position,1);
  }
  ).";

  auto fsSrc = R".(
  #version 460
  out vec4 fColor;
  void main(){
    fColor = vec4(0,1,0,1);    
  }
  ).";

  auto vs = createShader(GL_VERTEX_SHADER,vsSrc);
  auto fs = createShader(GL_FRAGMENT_SHADER,fsSrc);

  auto prg = createProgram({vs,fs});

  GLuint iTimeLoc = glGetUniformLocation(prg,"iTime");

  GLuint vbo;
  glCreateBuffers(1,&vbo);
  glNamedBufferData(vbo,sizeof(bunnyVertices),bunnyVertices,GL_DYNAMIC_COPY);

  GLuint ebo;
  glCreateBuffers(1,&ebo);
  glNamedBufferData(ebo,sizeof(bunnyIndices),bunnyIndices,GL_DYNAMIC_COPY);
     

  GLuint vao;
  glCreateVertexArrays(1,&vao);
  glVertexArrayVertexBuffer(vao,0,vbo,sizeof(float)*0,sizeof(float)*6);
  glEnableVertexArrayAttrib(vao,0);
  glVertexArrayAttribFormat(vao,0,3,GL_FLOAT,GL_FALSE,0);
  glVertexArrayAttribBinding(vao,0,0);
  glVertexArrayElementBuffer(vao,ebo);


  float iTime = 0.f;

  bool running = true;
  while(running){//main loop
    SDL_Event event;
    while(SDL_PollEvent(&event)){//event loop
      if(event.type == SDL_EVENT_QUIT)running = false;

    }

    iTime += 0.01;
  


    glPointSize(10);

    glClearColor(1,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);


    glBindVertexArray(vao);
    glUseProgram(prg);

    glProgramUniform1f(prg,iTimeLoc,iTime);

    glDrawElements(GL_TRIANGLES,sizeof(bunnyIndices)/sizeof(VertexIndex),GL_UNSIGNED_INT,0);

    SDL_GL_SwapWindow(window);
  }

  SDL_GL_DestroyContext(context);
  SDL_DestroyWindow(window);
  return 0;
}
