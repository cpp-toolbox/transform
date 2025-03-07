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
    glm::mat4 get_rotation_transform_matrix() const;
    glm::mat4 get_scale_transform_matrix() const;
    glm::mat4 get_translation_transform_matrix() const;
    void set_transform_matrix(glm::mat4 transform);

    glm::vec3 compute_forward_vector() const;
    glm::vec3 compute_right_vector() const;
    glm::vec3 compute_up_vector() const;
    std::string get_string_repr() const;

    glm::vec3 position; // Position in 3D space
    glm::vec3 rotation; // Euler angles in turns (pitch, yaw, roll)
    glm::vec3 scale;    // Scale factors

    friend std::ostream &operator<<(std::ostream &os, const Transform &transform) {
        os << transform.get_string_repr();
        return os;
    }
};

glm::mat4 create_billboard_transform(const glm::vec3 &right, const glm::vec3 &up, const glm::vec3 &look);
glm::mat4 create_billboard_transform_with_lock_axis(const glm::vec3 &lock_axis, const glm::vec3 &look);

#endif // TRANSFORM_HPP
