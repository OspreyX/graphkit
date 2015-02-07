{
	"targets": [
		{
			"target_name": "graphkit",
			"cflags_cc": [],
			"cflags": ["-std=c++11", "-O3", "-Wall"],
			"include_dirs" : [
				"./src"
			],
			"sources": [
				"./src/GraphKit.cpp",
				"./src/ObjectWrapPolicy.cpp",
				"./src/Node.cpp",
				"./src/Entity.cpp"
			],
			"conditions": [
				["OS=='mac'", {
					"xcode_settings": {
						"OTHER_CPLUSPLUSFLAGS" : ["-O3", "-Wall", "-std=c++11", "-stdlib=libc++"],
						"OTHER_LDFLAGS": ["-stdlib=libc++"],
						"MACOSX_DEPLOYMENT_TARGET": "10.7",
						"GCC_ENABLE_CPP_EXCEPTIONS": "YES"
					}
				}]
			]
		}
	]
}
