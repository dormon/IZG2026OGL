#include<iostream>
#include<map>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

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
  GLint status;
  glGetShaderiv(s,GL_COMPILE_STATUS,&status);
  if(status == GL_FALSE){
    char buf[2024];
    glGetShaderInfoLog(s,2024,0,buf);
    std::cerr << buf << std::endl;
  }

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
  layout(location=1)in vec3 normal  ;

  out vec3 vPosition;
  out vec3 vNormal;
  out vec3 vPositionModelSpace;

  uniform float iTime;
  uniform mat4 modelMatrix = mat4(1);
  uniform mat4 viewMatrix  = mat4(1);
  uniform mat4 projMatrix  = mat4(1);

  void main(){
    mat4 mvp = projMatrix * viewMatrix * modelMatrix;

    gl_Position = mvp * vec4(position,1);
    vPosition = vec3(modelMatrix * vec4(position,1));
    vNormal = vec3(inverse(transpose(modelMatrix))*vec4(normal,0));
    vPositionModelSpace = position;
  }
  ).";

  auto fsSrc = R".(
  #version 460
  out vec4 fColor;

  in vec3 vPosition;
  in vec3 vNormal;
  in vec3 vPositionModelSpace;

  uniform mat4 viewMatrix  = mat4(1);

  uniform vec3 lightPosition     = vec3(0,0,10);
  uniform vec3 lightColor        = vec3(1);
  uniform vec3 ambientLightColor = vec3(0.2);
  uniform float shininess = 40.f;

  vec3 checkerBoard(vec2 uv){
    return vec3(fract(uv.x*4)<0.5 == fract(uv.y*4)<0.5,0,0);
  }

  vec3 circle(vec2 uv){
    if(length(uv)<0.5)
      return vec3(1,0,0);
    else
      return vec3(0,1,0);
  }

  void main(){
    vec3 N = normalize(vNormal);

    vec3 cameraPosition = vec3(inverse(viewMatrix) * vec4(0,0,0,1));

    vec3 materialColor = 
       checkerBoard(vPositionModelSpace.xz) * dot(N,vec3(0,1,0))+
       circle      (vPositionModelSpace.xy) * dot(N,vec3(0,0,1));



    vec3 L = normalize(lightPosition - vPosition);
    vec3 V = normalize(cameraPosition - vPosition);
    vec3 R = -reflect(L,N);

    float dF = max(dot(N,L),0);
    float sF = pow(max(dot(V,R),0),shininess);

    vec3 ambient  = ambientLightColor * materialColor;
    vec3 diffuse  = dF * lightColor * materialColor;
    vec3 specular = sF * lightColor;


    vec3 lambert = ambient + diffuse;
    vec3 phong   = ambient + diffuse + specular;

    fColor = vec4(phong,1);    
  }
  ).";

  auto vs = createShader(GL_VERTEX_SHADER,vsSrc);
  auto fs = createShader(GL_FRAGMENT_SHADER,fsSrc);

  auto prg = createProgram({vs,fs});

  GLuint iTimeLoc       = glGetUniformLocation(prg,"iTime"      );
  GLuint modelMatrixLoc = glGetUniformLocation(prg,"modelMatrix");
  GLuint viewMatrixLoc  = glGetUniformLocation(prg,"viewMatrix" );
  GLuint projMatrixLoc  = glGetUniformLocation(prg,"projMatrix" );

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

  glVertexArrayVertexBuffer(vao,1,vbo,sizeof(float)*3,sizeof(float)*6);
  glEnableVertexArrayAttrib(vao,1);
  glVertexArrayAttribFormat(vao,1,3,GL_FLOAT,GL_FALSE,0);
  glVertexArrayAttribBinding(vao,1,1);

  glVertexArrayElementBuffer(vao,ebo);


  glEnable(GL_DEPTH_TEST);

  float iTime = 0.f;

  glm::vec3 cameraPosition = glm::vec3(0.f);
  glm::vec3 cameraAngle    = glm::vec3(0.f);

  float sensitivity = 0.01f;

  bool running = true;

  std::map<uint32_t,bool>keys;

  while(running){//main loop
    SDL_Event event;

    auto viewRotationMatrix = 
      glm::rotate(glm::mat4(1.f),cameraAngle.x,glm::vec3(1.f,0.f,0.f))*
      glm::rotate(glm::mat4(1.f),cameraAngle.y,glm::vec3(0.f,1.f,0.f));

    while(SDL_PollEvent(&event)){//event loop
      if(event.type == SDL_EVENT_QUIT)running = false;
      if(event.type == SDL_EVENT_KEY_DOWN){
        keys[event.key.key] = true;
      }
      if(event.type == SDL_EVENT_KEY_UP){
        keys[event.key.key] = false;
      }
      if(event.type == SDL_EVENT_MOUSE_MOTION){
        if(event.motion.state&SDL_BUTTON_LEFT){
          cameraAngle.y += event.motion.xrel * sensitivity;
          cameraAngle.x += event.motion.yrel * sensitivity;
        }
      }

    }
    auto vrtm = glm::transpose(viewRotationMatrix);

    glm::vec3 X = glm::vec3(vrtm[0]);
    glm::vec3 Y = glm::vec3(vrtm[1]);
    glm::vec3 Z = glm::vec3(vrtm[2]);
    if(keys[SDLK_W      ]) cameraPosition += Z*0.1f;
    if(keys[SDLK_S      ]) cameraPosition -= Z*0.1f;
    if(keys[SDLK_A      ]) cameraPosition += X*0.1f;
    if(keys[SDLK_D      ]) cameraPosition -= X*0.1f;
    if(keys[SDLK_SPACE  ]) cameraPosition -= Y*0.1f;
    if(keys[SDLK_LSHIFT ]) cameraPosition += Y*0.1f;
    if(keys[SDLK_R      ]){cameraPosition*=0.f;cameraAngle*=0.f;}

    iTime += 0.01;
  
    auto modelMatrix = glm::rotate(glm::mat4(1.f),glm::half_pi<float>()*iTime,glm::vec3(0.f,1.f,0.f));


    auto viewLocationMatrix = glm::translate(glm::mat4(1.f),cameraPosition);

    auto viewMatrix = viewRotationMatrix*viewLocationMatrix;
    auto projMatrix = glm::perspective(glm::half_pi<float>(),1.f,0.1f,1000.f);


    glPointSize(10);

    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


    glBindVertexArray(vao);
    glUseProgram(prg);

    glProgramUniform1f(prg,iTimeLoc,iTime);
    glProgramUniformMatrix4fv(prg,modelMatrixLoc,1,GL_FALSE,(float*)&modelMatrix);
    glProgramUniformMatrix4fv(prg,viewMatrixLoc ,1,GL_FALSE,(float*)&viewMatrix );
    glProgramUniformMatrix4fv(prg,projMatrixLoc ,1,GL_FALSE,(float*)&projMatrix );

    glDrawElements(GL_TRIANGLES,sizeof(bunnyIndices)/sizeof(VertexIndex),GL_UNSIGNED_INT,0);

    SDL_GL_SwapWindow(window);
  }

  SDL_GL_DestroyContext(context);
  SDL_DestroyWindow(window);
  return 0;
}
