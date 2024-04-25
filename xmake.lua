-- 设置项目名称
set_project("Glacier")

-- 设置编译选项
add_cxflags(
	"-rdynamic",
	"-O0",
	"-ggdb",
	"-Wall",
	"-Wno-deprecated",
	"-Werror",
	"-Wno-unused-function",
	{ force = true }
)

add_requires("boost", "yaml-cpp")

-- 设置 C++ 标准
set_languages("c++11")

-- 定义 Glacier 作为共享库
target("Glacier")
set_kind("shared")
add_files("Glacier/*.cpp") -- 添加库源文件
add_includedirs("$(projectdir)")
set_targetdir("$(projectdir)/lib") -- 设置库文件的输出目录
add_packages("boost", "yaml-cpp")

-- 定义测试可执行文件
target("test")
set_kind("binary")
add_deps("Glacier") -- 添加依赖，确保 Glacier 先编译
add_files("tests/test.cpp") -- 添加测试源文件
add_includedirs("$(projectdir)")
add_links("Glacier") -- 链接 Glacier 库
set_targetdir("$(projectdir)/bin") -- 设置生成的可执行文件目录

target("test_config")
set_kind("binary")
add_deps("Glacier") -- 添加依赖，确保 Glacier 先编译
add_files("tests/test_config.cpp") -- 添加测试源文件
add_includedirs("$(projectdir)")
add_links("Glacier") -- 链接 Glacier 库
set_targetdir("$(projectdir)/bin") -- 设置生成的可执行文件目录

target("test_yaml")
set_kind("binary")
add_deps("Glacier") -- 添加依赖，确保 Glacier 先编译
add_files("tests/test_yaml.cpp") -- 添加测试源文件
add_includedirs("$(projectdir)")
add_links("Glacier") -- 链接 Glacier 库
set_targetdir("$(projectdir)/bin") -- 设置生成的可执行文件目录
add_packages("boost", "yaml-cpp")

target("test_thread")
set_kind("binary")
add_deps("Glacier") -- 添加依赖，确保 Glacier 先编译
add_files("tests/test_thread.cpp") -- 添加测试源文件
add_includedirs("$(projectdir)")
add_links("Glacier") -- 链接 Glacier 库
set_targetdir("$(projectdir)/bin") -- 设置生成的可执行文件目录
add_packages("boost", "yaml-cpp")

--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--
