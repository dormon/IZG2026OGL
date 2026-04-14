#include<iostream>

#include<SDL3/SDL.h>
#include<geGL/geGL.h>
#include<geGL/StaticCalls.h>

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
  void main(){
    gl_Position = vec4(gl_VertexID%2,gl_VertexID/2,0,1);
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

  bool running = true;
  while(running){//main loop
    SDL_Event event;
    while(SDL_PollEvent(&event)){//event loop
      if(event.type == SDL_EVENT_QUIT)running = false;

    }
  


    glPointSize(10);

    glClearColor(1,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);


    glUseProgram(prg);
    glDrawArrays(GL_TRIANGLES,0,3);

    SDL_GL_SwapWindow(window);
  }

  SDL_GL_DestroyContext(context);
  SDL_DestroyWindow(window);
  return 0;
}
