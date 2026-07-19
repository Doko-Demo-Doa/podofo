// swift-tools-version:5.7
import PackageDescription

let package = Package(
    name: "PoDoFo",
    platforms: [ .iOS(.v17) ],
    products: [ .library(name: "PoDoFo", targets: ["PoDoFo"]) ],
    targets: [
        .binaryTarget(
            name: "PoDoFo",
            url: "https://github.com/Doko-Demo-Doa/podofo/releases/download/v0.0.5/PoDoFo-0.0.5.xcframework.zip",
            checksum: "6cf49661d585e4693af90b2988da1fbf4d715d427b7232e55edaecc5dc5e5c47"
        )
    ]
)
