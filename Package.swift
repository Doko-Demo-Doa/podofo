// swift-tools-version:5.7
import PackageDescription

let package = Package(
    name: "PoDoFo",
    platforms: [ .iOS(.v17) ],
    products: [ .library(name: "PoDoFo", targets: ["PoDoFo"]) ],
    targets: [
        .binaryTarget(
            name: "PoDoFo",
            url: "https://github.com/Doko-Demo-Doa/podofo/releases/download/v0.0.10/PoDoFo-0.0.10.xcframework.zip",
            checksum: "69027db41410f1bad7ff3680677d69a8712bf98a6c05c5516f4b2836209cd786"
        )
    ]
)
