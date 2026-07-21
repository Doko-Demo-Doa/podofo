// swift-tools-version:5.7
import PackageDescription

let package = Package(
    name: "PoDoFo",
    platforms: [ .iOS(.v17) ],
    products: [ .library(name: "PoDoFo", targets: ["PoDoFo"]) ],
    targets: [
        .binaryTarget(
            name: "PoDoFo",
            url: "https://github.com/Doko-Demo-Doa/podofo/releases/download/v0.0.12/PoDoFo-0.0.12.xcframework.zip",
            checksum: "340c653c9dcd9d2890f8cb3b08eb424fe2af9672893e1d329d2b459bbe2dcea0"
        )
    ]
)
