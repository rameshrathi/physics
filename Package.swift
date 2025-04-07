// swift-tools-version:6.0
import PackageDescription

let package = Package(
	name: "runner",
	products:[
		.library(
			name: "runner",
			targets: ["llama"]
		),
	],
	targets: [
		.binaryTarget(
			name: "llama",
			path: "runner/library/llama.xcframework"
		),
		.testTarget(
			name: "Tests",
			dependencies: ["llama"],
			path: "runner/tests"
		),
		.executableTarget(
			name: "Run",
			dependencies: ["llama"],
			path: "runner/run"
		)
	]
)
