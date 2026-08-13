# VSLAM_cpp

A C++ project for building a Visual Simultaneous Localization and Mapping (VSLAM) system from the ground up using OpenCV, OpenGL, GLFW, GLEW, and Dear ImGui.

The project currently provides the application framework, camera input pipeline, monocular ORB feature extraction, basic stereo-camera support, and an ImGui-based interface for visualizing processed camera frames.

> **Project status:** Early development. The repository currently implements the frontend foundation for a VSLAM system, but does not yet perform complete visual odometry, mapping, bundle adjustment, or loop closure.

---

## Features

### Currently implemented

- C++17 project structure
- OpenCV camera capture
- Webcam support
- HTTP / HTTPS / RTSP camera stream support
- Monocular camera pipeline
- ORB feature detection
- ORB descriptor extraction
- Feature visualization
- Configurable number of ORB features
- Basic stereo-camera class
- Grayscale image preprocessing
- Histogram equalization for stereo frames
- Dear ImGui user interface
- GLFW window management
- OpenGL rendering
- Camera frames displayed as OpenGL textures
- Start / Stop controls
- Camera connection status
- Feature count display
- Basic SLAM-status interface
- CMake build system

---

## Current Pipeline

The monocular processing pipeline currently follows:

```text
Camera Input
    │
    ▼
OpenCV VideoCapture
    │
    ▼
Frame Acquisition
    │
    ▼
Resize
    │
    ▼
Grayscale Conversion
    │
    ▼
ORB Feature Extraction
    │
    ├── Keypoints
    └── Descriptors
    │
    ▼
Feature Visualization
    │
    ▼
OpenGL Texture
    │
    ▼
Dear ImGui Interface
```

At this stage, the project should be considered a **VSLAM frontend foundation** rather than a complete SLAM implementation.

---

## Repository Structure

```text
VSLAM_cpp/
├── include/
│   ├── Window.h
│   ├── camera.h
│   ├── mono.h
│   ├── orb_cv.h
│   └── stero.h
│
├── src/
│   ├── Window.cpp
│   ├── camera.cpp
│   ├── mono.cpp
│   ├── orb_cv.cpp
│   └── stereo.cpp
│
├── extra/
│   └── Imgui/
│       ├── imgui_base/
│       └── backends_glfw_opengl/
│
├── main.cpp
├── CMakeLists.txt
├── comp.sh
├── run.sh
├── imgui.ini
├── .gitignore
└── LICENSE
```

---

## Main Components

### Window

The `Window` component manages the application window and rendering environment.

Responsibilities include:

- GLFW initialization
- OpenGL context creation
- GLEW initialization
- Dear ImGui initialization
- Frame rendering
- Window resizing
- VSync
- Application shutdown

---

### Camera

The camera layer provides common camera configuration and camera-source handling.

Supported input types include:

- Local webcams
- HTTP streams
- HTTPS streams
- RTSP streams

The camera configuration currently includes parameters such as:

- Frame width
- Frame height
- ORB feature count
- Number of frames to drop for network cameras

---

### MonoCamera

The monocular camera pipeline is currently the most developed vision component.

It performs:

- Camera connection
- Frame acquisition
- Network-stream latency handling
- Frame resizing
- Grayscale conversion
- ORB feature extraction
- Descriptor generation
- Keypoint visualization

---

### ORBExtractor

`ORBExtractor` wraps OpenCV's ORB implementation.

It is responsible for:

- Detecting ORB keypoints
- Computing ORB descriptors
- Managing ORB feature-extraction parameters

Keeping feature extraction separate from camera handling makes it easier to replace or extend the feature extractor later.

---

### StereoCamera

The repository also contains an early stereo-camera implementation.

Current stereo functionality includes:

- Left camera input
- Right camera input
- Frame acquisition from both cameras
- Grayscale conversion
- Histogram equalization
- Left/right image processing

Full stereo integration into the main GUI and SLAM pipeline is still under development.

---

## Dependencies

The project uses:

- **C++17**
- **OpenCV**
- **OpenGL**
- **GLEW**
- **GLFW**
- **Dear ImGui**
- **CMake**

---

## Building

The project currently targets a Linux-style development environment.

Install the required development libraries using your distribution's package manager.

On Ubuntu/Debian-based systems, the required packages typically include:

```bash
sudo apt update
sudo apt install build-essential cmake libopencv-dev libglfw3-dev libglew-dev libgl1-mesa-dev
```

Clone the repository:

```bash
git clone https://github.com/KanhavVohra/VSLAM_cpp.git
cd VSLAM_cpp
```

The repository includes a build helper script:

```bash
chmod +x comp.sh
./comp.sh
```

Alternatively, the project can be built manually with CMake:

```bash
mkdir -p build
cd build
cmake ..
cmake --build . --config Release
```

---

## Running

The repository includes a run helper:

```bash
chmod +x run.sh
./run.sh
```

The generated executable is named:

```text
VSlam
```

---

## Camera Sources

The application is designed to accept multiple types of camera input.

Examples include:

```text
0
1
http://camera-address
https://camera-address
rtsp://camera-address/stream
```

Numeric values represent local webcam indices.

---

## User Interface

The current ImGui application provides controls for:

- Starting processing
- Stopping processing
- Exiting the application
- Selecting monocular or stereo operation
- Entering camera sources
- Connecting to a camera
- Viewing the processed frame
- Monitoring feature count
- Monitoring camera connection state
- Monitoring SLAM/application state

---

## VSLAM Development Status

| Component | Status |
|---|---|
| Camera acquisition | ✅ Implemented |
| Webcam support | ✅ Implemented |
| Network camera support | ✅ Implemented |
| Image preprocessing | ✅ Implemented |
| ORB keypoint detection | ✅ Implemented |
| ORB descriptors | ✅ Implemented |
| Feature visualization | ✅ Implemented |
| ImGui interface | ✅ Implemented |
| OpenGL frame rendering | ✅ Implemented |
| Stereo camera foundation | 🟡 In progress |
| Feature matching | ❌ Not implemented |
| Feature tracking | ❌ Not implemented |
| Camera calibration pipeline | ❌ Not implemented |
| Essential matrix estimation | ❌ Not implemented |
| Relative pose estimation | ❌ Not implemented |
| Visual odometry | ❌ Not implemented |
| Triangulation | ❌ Not implemented |
| 3D landmarks | ❌ Not implemented |
| Keyframe management | ❌ Not implemented |
| Local mapping | ❌ Not implemented |
| Bundle adjustment | ❌ Not implemented |
| Relocalization | ❌ Not implemented |
| Loop closure | ❌ Not implemented |
| Pose graph optimization | ❌ Not implemented |
| Map persistence | ❌ Not implemented |

---

## Development Roadmap

A possible development path for the project is:

### Phase 1 — Feature Correspondence

- Match ORB descriptors between consecutive frames
- Add descriptor filtering
- Add ratio-test filtering
- Reject incorrect feature correspondences
- Visualize feature matches

### Phase 2 — Camera Motion Estimation

- Estimate the essential matrix
- Reject geometric outliers with RANSAC
- Recover relative camera rotation and translation
- Track camera pose between frames

At this stage, the project begins functioning as a basic **monocular visual odometry system**.

### Phase 3 — Map Initialization

- Triangulate matched features
- Generate initial 3D landmarks
- Validate reconstructed points
- Maintain landmark observations

### Phase 4 — Keyframes and Local Mapping

- Introduce keyframes
- Select keyframes based on camera motion and tracking quality
- Match new observations against map landmarks
- Add newly triangulated landmarks
- Remove low-quality landmarks

### Phase 5 — Optimization

- Add reprojection-error calculation
- Implement pose-only optimization
- Implement local bundle adjustment

Libraries such as Ceres Solver or g2o may be considered for nonlinear optimization.

### Phase 6 — Loop Closure

- Detect previously visited locations
- Estimate loop constraints
- Add pose-graph optimization
- Correct accumulated drift

### Phase 7 — Visualization

Extend the existing OpenGL/ImGui interface with:

- Real-time camera trajectory
- 3D landmark visualization
- Keyframe visualization
- Tracking statistics
- Reprojection-error statistics
- Local map visualization

---

## Intended Architecture

As the project develops, the system can evolve toward a modular structure similar to:

```text
Camera
   │
   ▼
Feature Extraction
   │
   ▼
Feature Matching
   │
   ▼
Tracking
   │
   ├──────────────► Pose Estimation
   │
   ▼
Keyframes
   │
   ▼
Local Mapping
   │
   ├──────────────► Triangulation
   │
   └──────────────► Bundle Adjustment
   │
   ▼
Loop Detection
   │
   ▼
Pose Graph Optimization
   │
   ▼
3D Map
```

---

## Goals

The long-term goal of this repository is to implement the major components of a VSLAM system directly in C++ while keeping the implementation modular and understandable.

Rather than wrapping an existing complete SLAM framework, the project is intended to progressively implement components such as:

- Feature extraction
- Feature correspondence
- Visual odometry
- Pose estimation
- Triangulation
- Landmark management
- Keyframes
- Local mapping
- Bundle adjustment
- Loop closure
- Map optimization
- Real-time visualization

---

## Current Limitations

The current implementation does **not** yet perform full SLAM.

In particular:

- Detected ORB features are not yet matched across frames.
- Camera motion is not estimated.
- No persistent 3D landmarks are created.
- No keyframe database exists.
- No map optimization is performed.
- No loop detection or loop closure is implemented.
- Stereo mode is not yet fully integrated into the main application.

The current `Start SLAM` control therefore starts the existing processing pipeline rather than a complete SLAM backend.

---

## License

See the repository's `LICENSE` file for licensing information.

---

## Author

**Kanhav Vohra**

GitHub: `KanhavVohra`

Repository: `VSLAM_cpp`
