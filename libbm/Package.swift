// swift-tools-version:6.0
import PackageDescription

let package = Package(
	name: "libbm",
	targets: [
		.executableTarget(
			name: "libbm",
			path: ".",
			publicHeadersPath: "src/include",
			cSettings: [
				.unsafeFlags(["-std=c11"]) // C11 standard
			]
		)
	]
)
