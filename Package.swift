// swift-tools-version:5.7
import PackageDescription

let package = Package(
    name: "PoDoFo",
    platforms: [ .iOS(.v17) ],
    products: [ .library(name: "PoDoFo", targets: ["PoDoFo"]) ],
    targets: [
        .binaryTarget(
            name: "PoDoFo",
            url: "https://github.com/Doko-Demo-Doa/podofo/releases/download/v0.1.0/PoDoFo-0.1.0.xcframework.zip",
            checksum: "da0b560b8c3f5177969a488545198e3f260aca44a70b4a8b078f43d9ac3f059d"
        )
    ]
)
