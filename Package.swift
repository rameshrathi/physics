// swift-tools-version:6.0
import PackageDescription

let package = Package(
	name: "runner",
	targets: [
        .target(
            name: "nwlib",
            path: "runner/nwlib"
        ),
		.target(
			name: "runner",
			dependencies: ["nwlib"],
			path: "runner/run"
		),
        .testTarget(
            name: "Tests",
            dependencies: ["nwlib"],
            path: "runner/tests"
        ),
	]
)
