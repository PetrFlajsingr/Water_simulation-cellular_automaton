#include "GlslShaderLoader.h"
#include "geGL_utils.h"
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
                               static_cast<float>(windowWidth) / windowHeight,
                               nearPlane, farPlane);

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

  std::array<std::shared_ptr<Buffer>, 2> cellBuffers{
      createBuffer(cells, GL_DYNAMIC_COPY),
      createBuffer(cells, GL_DYNAMIC_COPY)};

  std::vector<glm::vec4> positions{glm::compMul(tankSize)};
  auto positionsBuffer = createBuffer(positions, GL_DYNAMIC_DRAW);

  std::array<uint32_t, 5> command{cube.indicesCount(), 2, 0, 0, 0};
  auto indirectBuffer = createBuffer(command);

  // vertex array object
  auto vao = std::make_shared<VertexArray>();
  vao->addAttrib(vbo, 0, 3, GL_FLOAT,
                 static_cast<GLsizei>(sizeof(Model::VertexData)),
                 offsetof(Model::VertexData, pos));
  vao->addAttrib(vbo, 1, 3, GL_FLOAT,
                 static_cast<GLsizei>(sizeof(Model::VertexData)),
                 offsetof(Model::VertexData, color));
  vao->addAttrib(vbo, 2, 2, GL_FLOAT,
                 static_cast<GLsizei>(sizeof(Model::VertexData)),
                 offsetof(Model::VertexData, texCoord));
  vao->addAttrib(vbo, 3, 3, GL_FLOAT,
                 static_cast<GLsizei>(sizeof(Model::VertexData)),
                 offsetof(Model::VertexData, normal));

  auto vaoGrid = std::make_shared<VertexArray>();
  vaoGrid->addAttrib(vboGrid, 0, 3, GL_FLOAT, static_cast<GLsizei>(sizeof(glm::vec3)));

  glClearColor(0, 0, 0, 1);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  FPSCounter fpsCounter;
  mainLoop->setIdleCallback([&]() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    computeHorizontalProgram->use();
    cellBuffers[0]->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
    cellBuffers[1]->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
    positionsBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 2);
    indirectBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 3);

    Cell *ptrRD;
    Cell *ptrWR;
    ptrRD = reinterpret_cast<Cell*>(cellBuffers[1]->map(GL_READ_ONLY));
    ptrWR = reinterpret_cast<Cell*>(cellBuffers[0]->map(GL_READ_ONLY));

    cellBuffers[0]->unmap();
    cellBuffers[1]->unmap();

    glDispatchCompute(tankSize.x / 2, tankSize.y / 2, tankSize.z / 2);

    ptrRD = reinterpret_cast<Cell*>(cellBuffers[1]->map(GL_READ_ONLY));
    ptrWR = reinterpret_cast<Cell*>(cellBuffers[0]->map(GL_READ_ONLY));

    cellBuffers[0]->unmap();
    cellBuffers[1]->unmap();

    std::swap(cellBuffers[0], cellBuffers[1]);
    cellBuffers[0]->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
    cellBuffers[1]->bindBase(GL_SHADER_STORAGE_BUFFER, 1);
    computeVerticalProgram->use();
    glDispatchCompute(tankSize.x / 2, tankSize.y / 2, tankSize.z / 2);

    ptrRD = reinterpret_cast<Cell*>(cellBuffers[1]->map(GL_READ_ONLY));
    ptrWR = reinterpret_cast<Cell*>(cellBuffers[0]->map(GL_READ_ONLY));

    cellBuffers[0]->unmap();
    cellBuffers[1]->unmap();

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glMemoryBarrier(GL_COMMAND_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

    cellProgram->use();

    vao->bind();
    drawIdsBuffer->bind(GL_ELEMENT_ARRAY_BUFFER);
    positionsBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
    glm::mat4 view = camera.GetViewMatrix();
    cellProgram->setMatrix4fv("Model", glm::value_ptr(glm::mat4(1.0)), 1,
                              GL_FALSE);
    cellProgram->setMatrix4fv("View", glm::value_ptr(view), 1, GL_FALSE);
    cellProgram->setMatrix4fv("Projection", glm::value_ptr(proj), 1, GL_FALSE);
    cellProgram->set3fv("cameraPosition", glm::value_ptr(camera.Position));
    indirectBuffer->bind(GL_DRAW_INDIRECT_BUFFER);
    glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr);
    vao->unbind();
    drawIdsBuffer->unbind(GL_ELEMENT_ARRAY_BUFFER);
    positionsBuffer->unbind(GL_SHADER_STORAGE_BUFFER);
    indirectBuffer->unbind(GL_DRAW_INDIRECT_BUFFER);


    gridProgram->use();
    vaoGrid->bind();
    gridProgram->setMatrix4fv("Model", glm::value_ptr(glm::mat4(1.0)), 1,
                              GL_FALSE);
    gridProgram->setMatrix4fv("View", glm::value_ptr(view), 1, GL_FALSE);
    gridProgram->setMatrix4fv("Projection", glm::value_ptr(proj), 1, GL_FALSE);
    glDrawArrays(GL_LINES, 0, grid.size());
    vaoGrid->unbind();

    window->swap();
    std::swap(cellBuffers[0], cellBuffers[1]);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    fpsCounter.frame();
    print(fpsCounter);
  });

  (*mainLoop)();

  return 0;
}
