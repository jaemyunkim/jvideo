# jVideo
This project contains C++ classes that allow the grab frames from a source device using OpenCV in Windows, macOS, Linux, etc., in order to process or record the video into a video file.

This project uses CMake to be free from the dependency from operating systems and requests OpenCV, Boost, and QT5 (this doesn't include those libraries or frameworks in this project). Therefore, before trying this project, you need to prepare those libraries or frameworks in your system. 
Those have been tested in the following environments:
- OpenCV 3.4.3
- Boost 1.66.0
- QT 5.11
- Windows 10 and Ubuntu 18.04

Additionally, this project uses OpenCVDeviceEnumerator in order to enumerate the camera devices in Windows (it is excluded on macOS, Linux, and so on). It was referred from https://github.com/studiosi/OpenCVDeviceEnumerator.

## Acknowledgments
Thanks to the individuals and groups below for their contributions to improving this project:
- Dr. Byungyong Ryu
- Minji Park
- Byeongwoo Kim
