// swift-tools-version:6.0
import PackageDescription

let package = Package(
	name: "libbm",
	targets: [
		.target(
			name: "libbm",
			path: "lib",
			publicHeadersPath: "include",
			cSettings: [
				.unsafeFlags(["-std=c11"])
			]
		),
		.testTarget(
			name: "libbmTest",
			dependencies: ["libbm"],
			path: "tests"
		)
	]
)
