// swift-tools-version:6.0
import PackageDescription

let package = Package(
	name: "LibLlama",
	products:[
		.library(
			name: "LibLlma",
			targets: ["LibLlma"]
		),
	],
	targets: [
		.binaryTarget(
			name: "llama",
			path: "Sources/library/llama.xcframework"
		),
		.target(
			name: "LibLlma",
			dependencies: ["llama"]
		),
		.testTarget(
			name: "LibLlmaTest",
			dependencies: ["LibLlma"]
		),
		.executableTarget(
			name: "Run",
			dependencies: ["llama"],
			path: "lib"
		)
	]
)
