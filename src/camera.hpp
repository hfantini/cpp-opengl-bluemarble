#include <glm.hpp>
#include <ext.hpp>

class Camera
{
	public:
		const glm::vec3 DEFAULT_POSITION{ 0.0f, 0.0f, 10.0f };
		const glm::vec3 DEFAULT_DIRECTION{ 0.0f, 0.0f, -1.0f };
		const glm::vec3 DEFAULT_UP{ 0.0f, 1.0f, 0.0f };
		glm::vec3 position = DEFAULT_POSITION;
		glm::vec3 direction = DEFAULT_DIRECTION;
		glm::vec3 up = DEFAULT_UP;
		float fieldOfView = glm::radians(45.0f);
		float aspectRatio = 0.0f;
		float zNear = 0.01f;
		float zFar = 1000.f;
		float speed = 1.0f;

		Camera(int viewportWidth, int viewportHeight);
		glm::mat4 getViewProjection() const;
		void moveFoward(float amount);
		void moveMouseAxis(glm::vec2 amount, glm::vec3* anchor, bool useX = false);
		void moveRight(float amount);
		void look(glm::vec2 amount);
		void roll(float amount);
		void reset();
 };