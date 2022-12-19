#include <glm.hpp>
#include <ext.hpp>

class Camera
{
	public:
		glm::vec3 position{ 0.0f, 0.0f, 10.0f };
		glm::vec3 direction{ 0.0f, 0.0f, -1.0f };
		glm::vec3 up{ 0.0f, 1.0f, 0.0f };
		void moveFoward(float amount);
		void moveRight(float amount);
		void look(glm::vec2 amount);
		void roll(float amount);
		float fieldOfView = glm::radians(45.0f);
		float aspectRatio = 0.0f;
		float zNear = 0.01f;
		float zFar = 1000.f;
		float speed = 1.0f;

		Camera(int viewportWidth, int viewportHeight);
		glm::mat4 getViewProjection() const;
 };