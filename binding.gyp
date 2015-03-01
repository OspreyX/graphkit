{
	"targets": [
		{
			"target_name": "graphkit",
			"cflags_cc": [],
			"cflags": ["-std=c++11", "-O3", "-Wall", "-fpermissive"],
			"include_dirs" : [
				"./src"
			],
			'dependencies': [
				"node_modules/hiredis/deps/hiredis.gyp:hiredis"
			],
			"sources": [
				"./src/GraphKit.cpp",
				"./src/Export.cpp",
				"./src/Node.cpp",
				"./src/Entity.cpp",
				"./src/Redis.cpp"
			],
			"conditions": [
				["OS=='mac'", {
					"xcode_settings": {
						"OTHER_CPLUSPLUSFLAGS" : ["-O3", "-Wall", "-std=c++11", "-stdlib=libc++", "-fpermissive"],
						"OTHER_LDFLAGS": ["-stdlib=libc++"],
						"MACOSX_DEPLOYMENT_TARGET": "10.7",
						"GCC_ENABLE_CPP_EXCEPTIONS": "YES"
					}
				}]
			]
		}
	]
}
