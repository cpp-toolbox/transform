#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

class Transform {
  public:
    Transform();

    glm::mat4 get_transform_matrix() const;
    void set_transform_matrix(glm::mat4 transform);

    glm::vec3 compute_forward_vector() const;
    glm::vec3 compute_right_vector() const;
    glm::vec3 compute_up_vector() const;
    std::string get_string_repr() const;

    glm::vec3 position; // Position in 3D space
    glm::vec3 rotation; // Euler angles in turns (pitch, yaw, roll)
    glm::vec3 scale;    // Scale factors
};

#endif // TRANSFORM_HPP
