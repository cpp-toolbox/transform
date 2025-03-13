#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

class Transform {
  public:
    Transform();
    Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
        : position(position), rotation(rotation), scale(scale) {};

    void set_position(const glm::vec3 &new_position);
    void add_position(const glm::vec3 &add_position);

    void set_rotation(const glm::vec3 &pitch_yaw_roll);

    void set_rotation_pitch(const double &new_pitch);
    void set_rotation_yaw(const double &new_yaw);
    void set_rotation_roll(const double &new_roll);

    void add_rotation_pitch(const double &pitch);
    void add_rotation_yaw(const double &yaw);
    void add_rotation_roll(const double &roll);

    void set_scale(const glm::vec3 &new_scale);

    void set_scale_x(const double &new_scale);
    void set_scale_y(const double &new_scale);
    void set_scale_z(const double &new_scale);

    glm::mat4 get_transform_matrix();
    glm::vec3 get_rotation() const { return rotation; }
    glm::mat4 get_rotation_transform_matrix() const;
    glm::vec3 get_scale() const { return scale; }
    glm::mat4 get_scale_transform_matrix() const;
    glm::vec3 get_translation() const { return position; }
    glm::mat4 get_translation_transform_matrix() const;
    void set_transform_matrix(glm::mat4 transform);

    glm::vec3 compute_forward_vector() const;
    glm::vec3 compute_right_vector() const;
    glm::vec3 compute_up_vector() const;
    std::string get_string_repr() const;

    friend std::ostream &operator<<(std::ostream &os, const Transform &transform) {
        os << transform.get_string_repr();
        return os;
    }

  private:
    glm::mat4 transform_matrix = glm::mat4(1);
    bool transform_needs_update;
    void update_transform_matrix();

    glm::vec3 position; // Position in 3D space
    glm::vec3 rotation; // Euler angles in turns (pitch, yaw, roll)
    glm::vec3 scale;    // Scale factors
};

glm::mat4 create_billboard_transform(const Transform &transform);
glm::mat4 create_position_and_look_transform(const glm::vec3 &position, const glm::vec3 &look_vector,
                                             const glm::vec3 &up_hint = glm::vec3(0.0f, 1.0f, 0.0f));
glm::mat4 create_billboard_transform(const glm::vec3 &right, const glm::vec3 &up, const glm::vec3 &look);
glm::mat4 create_billboard_transform(const glm::vec3 &look);
glm::mat4 create_billboard_transform_with_lock_axis(const glm::vec3 &lock_axis, const glm::vec3 &look);

#endif // TRANSFORM_HPP
