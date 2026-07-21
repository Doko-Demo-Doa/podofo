// swift-tools-version:5.7
import PackageDescription

let package = Package(
    name: "PoDoFo",
    platforms: [ .iOS(.v17) ],
    products: [ .library(name: "PoDoFo", targets: ["PoDoFo"]) ],
    targets: [
        .binaryTarget(
            name: "PoDoFo",
            url: "https://github.com/Doko-Demo-Doa/podofo/releases/download/v0.0.9/PoDoFo-0.0.9.xcframework.zip",
            checksum: "4b174ebacf0037d8e3d085fe10bec48f1b2e11392e95df6eca8155f118d3859b"
        )
    ]
)
