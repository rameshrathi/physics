// swift-tools-version:6.0
import PackageDescription

let package = Package(
	name: "runner",
	targets: [
        .target(
            name: "nwlib",
            path: "nwlib/lib"
        ),
		.executableTarget(
			name: "runner",
			dependencies: ["nwlib"],
			path: "nwlib/run"
		),
        .testTarget(
            name: "tests",
            dependencies: ["nwlib"],
            path: "nwlib/tests"
        ),
	]
)
