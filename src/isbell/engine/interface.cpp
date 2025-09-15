#include "interface.hpp"

void BackStageInterface::connect_system(const ShaderProgram &shader, const shared_ptr<DynamicalInterface> &system, const shared_ptr<MaterialPhong> &material) {
	addMeshStep(shader, system, material);
	addCustomAction([&system](float t, float delta) {
		system->update(t);
	});
}
