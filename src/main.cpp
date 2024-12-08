#include <Arduino.h>
#include <painlessMesh.h>

// Defines
#define MESH_PREFIX     "ParkingMesh"
#define MESH_PASSWORD   "password123"
#define MESH_PORT       5555

// Variables
painlessMesh mesh;

// Function declarations:
void sendMessage();
void receivedCallback(uint32_t from, String &msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);

// Setup function
void setup() {
  Serial.begin(115200);
}  

void loop() {

}

// Function definitions:
// Function to broadcast free parking spots to the mesh
void sendMessage() {
  Serial.println("Message broadcasted");
}
// Callback for receiving messages from the mesh
void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Received from %u: msg=%s\n", from, msg.c_str());
}

// Callback for new connections
void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New connection established, nodeId = %u\n", nodeId);
}

// Callback for changed connections
void changedConnectionCallback() {
  Serial.println("Connections updated");
}

// Callback for adjusted node time
void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Node time adjusted. Offset = %d\n", offset);
}