#include <Arduino.h>
#include <painlessMesh.h>

// Defines
#define MESH_PREFIX     "ParkingMesh"
#define MESH_PASSWORD   "password123"
#define MESH_PORT       5555

// Variables
painlessMesh mesh;
Scheduler userScheduler;

// Struct to store received data
struct SectionData {
  int freeSpots = -1;
  int entranceScore = 0;
  int elevatorScore = 0;
};

std::map<String, SectionData> receivedSpots;

// Function declarations:
void sendMessage();
void receivedCallback(uint32_t from, String &msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);

// Task for sending messages
Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessage);

// Setup function
void setup() {
  Serial.begin(115200);

  // Mesh initialization
  mesh.setDebugMsgTypes(ERROR | STARTUP); // Debug messages
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  // Add and enable tasks
  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
}  

void loop() {
  mesh.update();
}

// Function definitions:
// Function to broadcast free parking spots to the mesh
void sendMessage() {
  // local Section message
  String msg;

  // Append data for each section: "A 5 B 3 C 4 D 6 E 2"
  if(receivedSpots.size() > 0) {
    for (const auto &section : receivedSpots) {
      if (section.second.freeSpots != -1) {
        msg += section.first + " " + String(section.second.freeSpots) + " " +
              String(section.second.entranceScore) + " " + String(section.second.elevatorScore) + " ";
      }
    }
    receivedSpots.clear();
  }
  if (msg.length() > 0) {
    mesh.sendBroadcast(msg); 
    Serial.println("Sending: " + msg); 
  }
  taskSendMessage.setInterval(random(TASK_SECOND * 1, TASK_SECOND * 2)); // Randomize broadcast interval
}

// Callback for receiving messages from the mesh
void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());

  unsigned int index = 0;
  while (index < msg.length()) {
    int spaceIdx = msg.indexOf(' ', index);
    if (spaceIdx == -1) break;

    String section = msg.substring(index, spaceIdx);
    index = spaceIdx + 1;

    spaceIdx = msg.indexOf(' ', index);
    int spots = msg.substring(index, spaceIdx).toInt();
    index = spaceIdx + 1;

    spaceIdx = msg.indexOf(' ', index);
    int entranceScore = msg.substring(index, spaceIdx).toInt();
    index = spaceIdx + 1;
    
    // read entrance Score
    // Note: The End condition is for last score -> (spaceIdx == -1)
    spaceIdx = msg.indexOf(' ', index); 
    int elevatorScore = (spaceIdx == -1) ? msg.substring(index).toInt() : msg.substring(index, spaceIdx).toInt();
    index = (spaceIdx == -1) ? msg.length() : spaceIdx + 1;
     
    receivedSpots[section] = {spots, entranceScore, elevatorScore};
    Serial.printf("Updated section %s: spots=%d, entrance=%d, elevator=%d\n", section.c_str(), spots, entranceScore, elevatorScore);
  }
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