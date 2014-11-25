project "MinTriangle"
--language "C
language "C++"
--kind "StaticLib"
--kind "SharedLib"
--kind "ConsoleApp"
kind "WindowedApp"

flags {
    "Unicode",
    "WinMain",
}
files {
    "*.cpp",
    "*.rc",
    "*.h",
    "*.fx",
}
includedirs {
}
buildoptions {
}
defines {
}
libdirs {
}
links {
    "d3d11",
    "d3dcompiler",
}

