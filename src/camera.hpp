#include <glm.hpp>
#include <ext.hpp>

class Camera
{
	public:
		glm::vec3 location{ 0.0f, 0.0f, 10.0f };
		glm::vec3 direction{ 0.0f, 0.0f, -1.0f };
		glm::vec3 up{ 0.0f, 1.0f, 0.0f };
		float fieldOfView = glm::radians(45.0f);
		float aspectRatio = 0.0f;
		float zNear = 0.01f;
		float zFar = 1000.f;

		Camera(int viewportWidth, int viewportHeight);
		glm::mat4 getViewProjection() const;
 };