#version 320 es
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec4 a_color;

struct helper {
    float circle_center_x;
    float circle_center_y;
    float phase;
};

uniform helper u_helper;

out vec4 v_color;

void main() {
    v_color = a_color;

    vec4 result_position = a_position;

    float new_x;
    float new_y;

    // Vertex is changing only for OY axis.
    if (abs(result_position.x - u_helper.circle_center_x) == 0.0) {
      new_x = result_position.x;
      new_y = result_position.y + u_helper.phase * result_position.y;
    }
    // Vertex is changing only for OX axis.
    else if (abs(result_position.y - u_helper.circle_center_y) == 0.0) {
      new_x = result_position.x + u_helper.phase * result_position.x;
      new_y = result_position.y;
    } else {
      // First we increment x value for vertex.
      // Then we can easily calculate y value
      // basing on the school equation for lines.
      // (x - x1) / (x2 - x1) = (y - y1) / (y2 - y1)
      new_x = result_position.x + u_helper.phase * result_position.x;
      new_y = (new_x - u_helper.circle_center_x)
              * (result_position.y - u_helper.circle_center_y) / (result_position.x - u_helper.circle_center_x)
          + u_helper.circle_center_y;
    }

    result_position.x = new_x;
    result_position.y = new_y;

    gl_Position = result_position;
}
