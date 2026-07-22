// swift-tools-version:5.7
import PackageDescription

let package = Package(
    name: "PoDoFo",
    platforms: [ .iOS(.v17) ],
    products: [ .library(name: "PoDoFo", targets: ["PoDoFo"]) ],
    targets: [
        .binaryTarget(
            name: "PoDoFo",
            url: "https://github.com/Doko-Demo-Doa/podofo/releases/download/v0.0.14/PoDoFo-0.0.14.xcframework.zip",
            checksum: "caba1d5805898223528940ab4711593258e491d88a9088a2b0473057c074ae39"
        )
    ]
)
