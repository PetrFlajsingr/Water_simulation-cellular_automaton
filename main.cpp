#include "GlslShaderLoader.h"
#include "io/print.h"
#include "shader_literals.h"
#include "time/FPSCounter.h"
#include <Camera.h>
#include <Cell.h>
#include <Model.h>
#include <SDL2CPP/MainLoop.h>
#include <SDL2CPP/Window.h>
#include <Utilities.h>
#include <chrono>
#include <geGL/StaticCalls.h>
#include <geGL/geGL.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/component_wise.hpp>
#include <iostream>
#include <thread>
#include "geGL_utils.h"

using namespace ge::gl;
using namespace std::string_literals;

auto camera = Camera(glm::vec3(1.0, 1.0, 5.0));

bool SDLHandler(const SDL_Event &event) {
  static bool mousePressed = false;
  if (event.type == SDL_KEYDOWN) {
    switch (event.key.keysym.sym) {
    case SDLK_UP:
    case SDLK_w:
      camera.ProcessKeyboard(Camera_Movement::FORWARD, 0.1);
      return true;
    case SDLK_DOWN:
    case SDLK_s:
      camera.ProcessKeyboard(Camera_Movement::BACKWARD, 0.1);
      return true;
    case SDLK_LEFT:
    case SDLK_a:
      camera.ProcessKeyboard(Camera_Movement::LEFT, 0.1);
      return true;
    case SDLK_RIGHT:
    case SDLK_d:
      camera.ProcessKeyboard(Camera_Movement::RIGHT, 0.1);
      return true;
    default:
      return false;
    }
  } else if (event.type == SDL_MOUSEBUTTONDOWN and
             event.button.button == SDL_BUTTON_LEFT) {
    mousePressed = true;
    return true;
  } else if (event.type == SDL_MOUSEMOTION and mousePressed) {
    camera.ProcessMouseMovement(-event.motion.xrel, event.motion.yrel);
    return true;
  } else if (event.type == SDL_MOUSEBUTTONUP and
             event.button.button == SDL_BUTTON_LEFT) {
    mousePressed = false;
    return true;
  }
  return false;
}

std::vector<glm::vec3> generateLines(int xLength, int yLength, int zLength) {
  std::vector<glm::vec3> result{};
  using namespace MakeRange;
  for (auto [z, y] : range<int, 2>({zLength + 1, yLength + 1})) {
    result.emplace_back(glm::vec3{0.0, y, z});
    result.emplace_back(glm::vec3{xLength, y, z});
  }
  for (auto [x, y] : range<int, 2>({xLength + 1, yLength + 1})) {
    result.emplace_back(glm::vec3{x, y, 0.0});
    result.emplace_back(glm::vec3{x, y, zLength});
  }
  for (auto [x, z] : range<int, 2>({xLength + 1, zLength + 1})) {
    result.emplace_back(glm::vec3{x, 0.0, z});
    result.emplace_back(glm::vec3{x, yLength, z});
  }
  return result;
}

std::vector<glm::vec3> generateLines(glm::vec3 length) {
  return generateLines(length.x, length.y, length.z);
}

int main() {
  using namespace ShaderLiterals;
  auto tankSize = glm::uvec3(4, 4, 4);
  const auto fieldOfView = 45.f;
  const auto windowWidth = 640;
  const auto windowHeight = 480;
  const auto nearPlane = 0.1f;
  const auto farPlane = 100.f;
  auto proj = glm::perspective(glm::radians(fieldOfView),
                               static_cast<float>(windowWidth) / windowHeight, nearPlane, farPlane);

  /*Create Window*/
  auto mainLoop = std::make_shared<sdl2cpp::MainLoop>();
  auto window = std::make_shared<sdl2cpp::Window>(windowWidth, windowHeight);
  window->createContext("rendering");
  mainLoop->addWindow("mainWindow", window);
  mainLoop->setEventHandler(SDLHandler);

  /*init OpenGL*/
  ge::gl::init(SDL_GL_GetProcAddress);
  ge::gl::setHighDebugMessage();

  setShaderLocation(SRC_DIR + "/Shaders"s);

  auto cellProgram =
      std::make_shared<ge::gl::Program>("basic"_vert, "basic"_frag);
  auto gridProgram =
      std::make_shared<ge::gl::Program>("grid"_vert, "grid"_frag);
  auto computeHorizontalProgram =
      std::make_shared<ge::gl::Program>("basic-horizontal"_comp);
  auto computeVerticalProgram =
      std::make_shared<ge::gl::Program>("basic-vertical"_comp);

  auto cube = Model(SRC_DIR + "/Resources/Models/cube.obj"s);
  auto cells = std::vector<Cell>(glm::compMul(tankSize));
  cells[15].setFluidVolume(1.0);
  cells[7].setFluidVolume(1.0);

  auto vbo = createBuffer(cube.getVertices());

  const auto grid = generateLines(tankSize);
  auto vboGrid = createBuffer(grid);

  auto drawIdsBuffer = createBuffer(cube.getIndices());

  std::array<std::shared_ptr<Buffer>, 2> cellBuffers {
    createBuffer(cells, GL_DYNAMIC_COPY),
    createBuffer(cells, GL_DYNAMIC_COPY)
  };

  std::vector<glm::vec4> positions{glm::compMul(tankSize)};
  auto positionsBuffer = createBuffer(positions, GL_DYNAMIC_DRAW);


  std::array<uint32_t, 5> command{cube.indicesCount(), 2, 0, 0, 0};
  auto indirectBuffer = createBuffer(command);

  // vertex array object
  //auto vao = std::make_shared<VertexArray>();
  //vao->addAttrib(vao, 0, 3, GL_FLOAT, static_cast<GLsizei>(sizeof(Model::VertexData)), offsetof(Model::VertexData, pos));
  GLuint vao;
  glCreateVertexArrays(1, &vao);
  // attrib 0. is vertex positions
  glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE,
                            offsetof(Model::VertexData, pos));
  glVertexArrayVertexBuffer(vao, 0, vbo->getId(), 0,
                            static_cast<GLsizei>(sizeof(Model::VertexData)));
  glVertexArrayAttribBinding(vao, 0, 0);
  glEnableVertexArrayAttrib(vao, 0);

  glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE,
                            offsetof(Model::VertexData, color));
  glVertexArrayVertexBuffer(vao, 0, vbo->getId(), offsetof(Model::VertexData, color),
                            static_cast<GLsizei>(sizeof(Model::VertexData)));
  glVertexArrayAttribBinding(vao, 1, 0);
  glEnableVertexArrayAttrib(vao, 1);

  glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, GL_FALSE,
                            offsetof(Model::VertexData, texCoord));
  glVertexArrayVertexBuffer(vao, 0, vbo->getId(), 0,
                            static_cast<GLsizei>(sizeof(Model::VertexData)));
  glVertexArrayAttribBinding(vao, 2, 0);
  glEnableVertexArrayAttrib(vao, 2);

  glVertexArrayAttribFormat(vao, 3, 3, GL_FLOAT, GL_FALSE,
                            offsetof(Model::VertexData, normal));
  glVertexArrayVertexBuffer(vao, 0, vbo->getId(), 0,
                            static_cast<GLsizei>(sizeof(Model::VertexData)));
  glVertexArrayAttribBinding(vao, 3, 0);
  glEnableVertexArrayAttrib(vao, 3);

  GLuint vaoGrid;
  glCreateVertexArrays(1, &vaoGrid);
  glVertexArrayAttribFormat(vaoGrid, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayVertexBuffer(vaoGrid, 0, vboGrid->getId(), 0,
                            static_cast<GLsizei>(sizeof(glm::vec3)));
  glVertexArrayAttribBinding(vaoGrid, 0, 0);
  glEnableVertexArrayAttrib(vaoGrid, 0);

  glClearColor(0, 0, 0, 1);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  FPSCounter fpsCounter;
  mainLoop->setIdleCallback([&]() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    computeHorizontalProgram->use();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, cellBuffers[0]->getId());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, cellBuffers[1]->getId());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, positionsBuffer->getId());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, indirectBuffer->getId());

    Cell *ptrRD;
    Cell *ptrWR;
    ptrRD = (Cell *)glMapNamedBuffer(cellBuffers[1]->getId(), GL_READ_ONLY);
    ptrWR = (Cell *)glMapNamedBuffer(cellBuffers[0]->getId(), GL_READ_ONLY);

    glUnmapNamedBuffer(cellBuffers[0]->getId());
    glUnmapNamedBuffer(cellBuffers[1]->getId());

    glDispatchCompute(tankSize.x / 2, tankSize.y / 2, tankSize.z / 2);

    ptrRD = (Cell *)glMapNamedBuffer(cellBuffers[1]->getId(), GL_READ_ONLY);
    ptrWR = (Cell *)glMapNamedBuffer(cellBuffers[0]->getId(), GL_READ_ONLY);

    glUnmapNamedBuffer(cellBuffers[0]->getId());
    glUnmapNamedBuffer(cellBuffers[1]->getId());

    std::swap(cellBuffers[0], cellBuffers[1]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, cellBuffers[0]->getId());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, cellBuffers[1]->getId());
    computeVerticalProgram->use();
    glDispatchCompute(tankSize.x / 2, tankSize.y / 2, tankSize.z / 2);

    ptrRD = (Cell *)glMapNamedBuffer(cellBuffers[1]->getId(), GL_READ_ONLY);
    ptrWR = (Cell *)glMapNamedBuffer(cellBuffers[0]->getId(), GL_READ_ONLY);

    glUnmapNamedBuffer(cellBuffers[0]->getId());
    glUnmapNamedBuffer(cellBuffers[1]->getId());

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

    cellProgram->use();

    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawIdsBuffer->getId());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, positionsBuffer->getId());
    glm::mat4 view = camera.GetViewMatrix();
    cellProgram->setMatrix4fv("Model", glm::value_ptr(glm::mat4(1.0)), 1, GL_FALSE);
    cellProgram->setMatrix4fv("View", glm::value_ptr(view), 1, GL_FALSE);
    cellProgram->setMatrix4fv("Projection", glm::value_ptr(proj), 1, GL_FALSE);
    cellProgram->set3fv("cameraPosition", glm::value_ptr(camera.Position));
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer->getId());
    glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    gridProgram->use();
    glBindVertexArray(vaoGrid);
    gridProgram->setMatrix4fv("Model", glm::value_ptr(glm::mat4(1.0)), 1, GL_FALSE);
    gridProgram->setMatrix4fv("View", glm::value_ptr(view), 1, GL_FALSE);
    gridProgram->setMatrix4fv("Projection", glm::value_ptr(proj), 1, GL_FALSE);
    glDrawArrays(GL_LINES, 0, grid.size());
    glBindVertexArray(0);

    window->swap();
    std::swap(cellBuffers[0], cellBuffers[1]);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    fpsCounter.frame();
    print(fpsCounter);
  });

  (*mainLoop)();

  return 0;
}
