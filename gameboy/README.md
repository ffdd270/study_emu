# HOW TO BUILD 
vcpkg가 필요합니다.  https://github.com/microsoft/vcpkg 이 곳에서 설치한 후, CLion 기준으로 Setting -> Build -> CMake 에서. CMake Option에 -DCMAKE_TOOLCHAIN_FILE='{vcpkg 경로}'를 적어주세요. 

그 이후 vcpkg에서 vcpkg install glfw3:x64-windows를 하시면 빌드 준비가 완료됩니다. 
