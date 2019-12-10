//
// Created by Igor Frank on 05.12.19.
//

#include "Model.h"
#include <glm/gtx/hash.hpp>
#include <tiny_obj_loader.h>
#include <unordered_map>

const std::vector<Model::VertexData> &Model::getVertices() const {
    return vertices;
}

const std::vector<uint32_t> &Model::getIndices() const {
    return indices;
}

namespace std {
    template<>
    struct hash<Model::VertexData> {
        size_t operator()(Model::VertexData const &vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                     (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                   (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

void Model::loadFromObj(const std::string &modelPath, bool clearBeforeLoad) {
    if (clearBeforeLoad) {
        vertices.clear();
        indices.clear();
        name = "Unnamed model";
    }

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str())) {
        throw std::runtime_error(warn + err);
    }

    name = "";

    for (const auto &shape : shapes) {
        const auto color = glm::vec3{0.0, 0.0, 1.0};
        std::unordered_map<Model::VertexData, uint32_t> uniqueVertices{};
        name += shape.name;
        if (&shape != &shapes.back()) {
            name += " + ";
        }
        for (const auto &index : shape.mesh.indices) {
            Model::VertexData vertex{};

            vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.color = color;

            if (uniqueVertices.count(vertex) == 0) {
                vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                };
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.emplace_back(vertex);

            } else {
                vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                };
                vertices[uniqueVertices[vertex]].normal += vertex.normal;
            }

            indices.emplace_back(uniqueVertices[vertex]);
        }
    }

}

Model::Model(const std::string &modelPath) {
    loadFromObj(modelPath);
}

uint32_t Model::verticesCount() {
    return vertices.size();
}

uint32_t Model::indicesCount() {
    return indices.size();
}
