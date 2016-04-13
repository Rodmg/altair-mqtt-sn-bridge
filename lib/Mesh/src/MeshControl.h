#ifndef MESHCONTROL_H
#define MESHCONTROL_H

#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>

#define MESH_ADDRENDPOINT 15
#define MESH_CMD_GETEUI 0
#define MESH_CMD_RESEUI 1

// Forward declaration of AquilaMesh
class AquilaMesh;

class MeshControl
{
	private:
		AquilaMesh *mesh;
	public:
		MeshControl();
		void begin(AquilaMesh *_mesh);
		uint16_t assignAddr(uint8_t *id);
		void sendEUI(uint16_t dest);
		
};

#endif //MESHCONTROL_H