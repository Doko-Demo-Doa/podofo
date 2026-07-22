// swift-tools-version:5.7
import PackageDescription

let package = Package(
    name: "PoDoFo",
    platforms: [ .iOS(.v17) ],
    products: [ .library(name: "PoDoFo", targets: ["PoDoFo"]) ],
    targets: [
        .binaryTarget(
            name: "PoDoFo",
            url: "https://github.com/Doko-Demo-Doa/podofo/releases/download/v0.0.13/PoDoFo-0.0.13.xcframework.zip",
            checksum: "c5d70faf75ec4436ccd0a30afda3d655f4c46c39904ddfa88f51b1a8d672b2a9"
        )
    ]
)
