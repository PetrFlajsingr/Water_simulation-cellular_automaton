//
// Created by Igor Frank on 05.12.19.
//

#ifndef GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_MODEL_H
#define GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_MODEL_H

#include <glm/glm.hpp>
#include <string>
#include <vector>

class Model {
public:
  struct VertexData {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
    glm::vec3 normal;

    bool operator==(const VertexData &other) const {
      return pos == other.pos && color == other.color && texCoord == other.texCoord && other.normal == normal;
    }
  };

  Model() = default;

  explicit Model(const std::string &modelPath);

  [[nodiscard]] const std::vector<VertexData> &getVertices() const;

  uint32_t verticesCount();

  [[nodiscard]] const std::vector<uint32_t> &getIndices() const;

  uint32_t indicesCount();

  void loadFromObj(const std::string &modelPath, bool clearBeforeLoad = false);

private:
  std::vector<Model::VertexData> vertices;
  std::vector<uint32_t> indices;
  std::string name = "Unnamed model";
};

#endif // GMU_FLUID_SIMULATION_CELLULAR_AUTOMATA_MODEL_H
