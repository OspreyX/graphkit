{
	"targets": [
		{
			"target_name": "graphkit",
			"cflags_cc": ["-frtti", "-fexceptions"],
			"cflags": ["-std=c++11", "-O3", "-Wall"],
			"include_dirs" : [
				"./src"
			],
			'dependencies': [],
			"sources": [
				"./src/GraphKit.cpp",
				"./src/Export.cpp",
				"./src/Node.cpp",
				"./src/Entity.cpp",
				"./src/Coordinator.cpp",
				"./src/Hub.cpp",
				"./src/Graph.cpp",
				"./src/Cluster.cpp",
				"./src/Index.cpp",
				"./src/Set.cpp",
				"./src/Multiset.cpp"
			],
			"conditions": [
				["OS=='mac'", {
					"xcode_settings": {
						"OTHER_CPLUSPLUSFLAGS" : ["-O3", "-Wall", "-std=c++11", "-stdlib=libc++", "-frtti"],
						"OTHER_LDFLAGS": ["-stdlib=libc++", "-frtti"],
						"MACOSX_DEPLOYMENT_TARGET": "10.7",
						"GCC_ENABLE_CPP_EXCEPTIONS": "YES"
					}
				}]
			]
		}
	]
}
