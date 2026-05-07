include("C:/Users/gaurd/Desktop/kiran_cs/Projects/C++/ECS-Physics-SDL/SDL-Engine/vendor/tracy/cmake/CPM.cmake")
CPMAddPackage("NAME;zstd;GITHUB_REPOSITORY;facebook/zstd;GIT_TAG;v1.5.7;OPTIONS;ZSTD_BUILD_SHARED OFF;EXCLUDE_FROM_ALL;TRUE;SOURCE_SUBDIR;build/cmake")
set(zstd_FOUND TRUE)